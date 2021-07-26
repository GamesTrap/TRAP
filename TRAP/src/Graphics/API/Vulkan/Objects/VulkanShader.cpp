#include "TRAPPCH.h"
#include "VulkanShader.h"

#include "VulkanDevice.h"
#include "Graphics/API/ShaderReflection.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/VulkanShaderReflection.h"
#include "Graphics/API/Vulkan/Objects/VulkanRootSignature.h"
#include "Graphics/API/Objects/RootSignature.h"
#include "Graphics/API/Objects/DescriptorPool.h"
#include "Graphics/API/Objects/DescriptorSet.h"
#include "Graphics/Buffers/VertexBufferLayout.h"

TRAP::Graphics::API::VulkanShader::VulkanShader(const std::string& name, const RendererAPI::BinaryShaderDesc& desc)
	: m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer().get())->GetDevice()),
	  m_numThreadsPerGroup(),
	  m_shaderModules(static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)),
	  m_reflection(nullptr),
	  m_entryNames(static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)),
	  m_firstUBOBind(true)
{
	m_name = name;

	TRAP_ASSERT(m_device, "device is nullptr");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanShaderPrefix, "Creating Shader: \"", m_name, "\"");
#endif

	uint32_t counter = 0;

	m_shaderStages = desc.Stages;

	std::array<ShaderReflection::ShaderReflection,
	           static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> stageReflections{};

	for(uint32_t i = 0; i < stageReflections.size(); i++)
	{
		const RendererAPI::ShaderStage stageMask = static_cast<RendererAPI::ShaderStage>(BIT(i));
		if(stageMask == (m_shaderStages & stageMask))
		{
			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;

			const RendererAPI::BinaryShaderStageDesc* stageDesc = nullptr;

			switch(stageMask)
			{
				case RendererAPI::ShaderStage::Vertex:
				{
					stageReflections[counter] = VkCreateShaderReflection(desc.Vertex.ByteCode, stageMask);

					createInfo.codeSize = desc.Vertex.ByteCode.size() * sizeof(uint32_t);
					createInfo.pCode = desc.Vertex.ByteCode.data();
					stageDesc = &desc.Vertex;
					VkCall(vkCreateShaderModule(m_device->GetVkDevice(), &createInfo, nullptr,
					                            &m_shaderModules[counter]));
					break;
				}

				case RendererAPI::ShaderStage::TessellationControl:
				{
					stageReflections[counter] = VkCreateShaderReflection(desc.TessellationControl.ByteCode,
					                                                     stageMask);

					createInfo.codeSize = desc.TessellationControl.ByteCode.size() * sizeof(uint32_t);
					createInfo.pCode = desc.TessellationControl.ByteCode.data();
					stageDesc = &desc.TessellationControl;
					VkCall(vkCreateShaderModule(m_device->GetVkDevice(), &createInfo, nullptr,
					                            &m_shaderModules[counter]));
					break;
				}

				case RendererAPI::ShaderStage::TessellationEvaluation:
				{
					stageReflections[counter] = VkCreateShaderReflection(desc.TessellationEvaluation.ByteCode,
					                                                     stageMask);

					createInfo.codeSize = desc.TessellationEvaluation.ByteCode.size() * sizeof(uint32_t);
					createInfo.pCode = desc.TessellationEvaluation.ByteCode.data();
					stageDesc = &desc.TessellationEvaluation;
					VkCall(vkCreateShaderModule(m_device->GetVkDevice(), &createInfo, nullptr,
					                            &m_shaderModules[counter]));
					break;
				}

				case RendererAPI::ShaderStage::Geometry:
				{
					stageReflections[counter] = VkCreateShaderReflection(desc.Geometry.ByteCode, stageMask);

					createInfo.codeSize = desc.Geometry.ByteCode.size() * sizeof(uint32_t);
					createInfo.pCode = desc.Geometry.ByteCode.data();
					stageDesc = &desc.Geometry;
					VkCall(vkCreateShaderModule(m_device->GetVkDevice(), &createInfo, nullptr,
					                            &m_shaderModules[counter]));
					break;
				}

				case RendererAPI::ShaderStage::Fragment:
				{
					stageReflections[counter] = VkCreateShaderReflection(desc.Fragment.ByteCode, stageMask);

					createInfo.codeSize = desc.Fragment.ByteCode.size() * sizeof(uint32_t);
					createInfo.pCode = desc.Fragment.ByteCode.data();
					stageDesc = &desc.Fragment;
					VkCall(vkCreateShaderModule(m_device->GetVkDevice(), &createInfo, nullptr,
					                            &m_shaderModules[counter]));
					break;
				}

				case RendererAPI::ShaderStage::Compute:
				case RendererAPI::ShaderStage::RayTracing:
				{
					stageReflections[counter] = VkCreateShaderReflection(desc.Compute.ByteCode, stageMask);

					createInfo.codeSize = desc.Compute.ByteCode.size() * sizeof(uint32_t);
					createInfo.pCode = desc.Compute.ByteCode.data();
					stageDesc = &desc.Compute;
					VkCall(vkCreateShaderModule(m_device->GetVkDevice(), &createInfo, nullptr,
					                            &m_shaderModules[counter]));
					break;
				}

				default:
					TRAP_ASSERT(false, "Shader Stage not supported!");
					break;
			}
			m_entryNames[counter] = stageDesc->EntryPoint;
			++counter;
		}
	}

	m_reflection = CreatePipelineReflection(stageReflections, counter);

	RendererAPI::RootSignatureDesc rootDesc{};
	rootDesc.Shaders.push_back(this);
	m_rootSignature = RootSignature::Create(rootDesc);

	//Static Descriptors
	if(dynamic_cast<VulkanRootSignature*>
		(
			m_rootSignature.get()
		)->GetVkDescriptorSetLayouts()[static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::None)] != VK_NULL_HANDLE)
	{
		RendererAPI::DescriptorSetDesc setDesc{};
		setDesc.MaxSets = 1; //TODO What does this do?
		setDesc.RootSignature = m_rootSignature;
		setDesc.UpdateFrequency = RendererAPI::DescriptorUpdateFrequency::None;
		m_descriptorSets.StaticDescriptors = RendererAPI::GetDescriptorPool()->RetrieveDescriptorSet(setDesc);
	}

	//Per Frame Descriptors
	if(dynamic_cast<VulkanRootSignature*>
		(
			m_rootSignature.get()
		)->GetVkDescriptorSetLayouts()[static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::PerFrame)] != VK_NULL_HANDLE)
	{
		RendererAPI::DescriptorSetDesc setDesc{};
		setDesc.MaxSets = RendererAPI::ImageCount; //TODO What does this do?
		setDesc.RootSignature = m_rootSignature;
		setDesc.UpdateFrequency = RendererAPI::DescriptorUpdateFrequency::PerFrame;
		m_descriptorSets.PerFrameDescriptors = RendererAPI::GetDescriptorPool()->RetrieveDescriptorSet(setDesc);
	}

	for(const auto& resource : m_reflection->ShaderResources)
	{
		if(resource.Type == RendererAPI::DescriptorType::UniformBuffer)
		{
			BufferUsage usage = BufferUsage::Static;
			if (resource.Set != static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::None))
				usage = BufferUsage::Dynamic;

			m_UBOs[resource.Set][resource.Reg] = UniformBuffer::Create(resource.Name, resource.Size, usage);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::~VulkanShader()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanShaderPrefix, "Destroying Shader: \"", m_name, "\"");
#endif

	if (static_cast<uint32_t>(m_shaderStages & RendererAPI::ShaderStage::Vertex))
		vkDestroyShaderModule(m_device->GetVkDevice(), m_shaderModules[m_reflection->VertexStageIndex], nullptr);

	if (static_cast<uint32_t>(m_shaderStages & RendererAPI::ShaderStage::TessellationControl))
		vkDestroyShaderModule(m_device->GetVkDevice(), m_shaderModules[m_reflection->TessellationControlStageIndex],
		                      nullptr);

	if (static_cast<uint32_t>(m_shaderStages & RendererAPI::ShaderStage::TessellationEvaluation))
		vkDestroyShaderModule(m_device->GetVkDevice(),
		                      m_shaderModules[m_reflection->TessellationEvaluationStageIndex], nullptr);

	if (static_cast<uint32_t>(m_shaderStages & RendererAPI::ShaderStage::Geometry))
		vkDestroyShaderModule(m_device->GetVkDevice(), m_shaderModules[m_reflection->GeometryStageIndex], nullptr);

	if (static_cast<uint32_t>(m_shaderStages & RendererAPI::ShaderStage::Fragment))
		vkDestroyShaderModule(m_device->GetVkDevice(), m_shaderModules[m_reflection->FragmentStageIndex], nullptr);

	if (static_cast<uint32_t>(m_shaderStages & RendererAPI::ShaderStage::Compute))
		vkDestroyShaderModule(m_device->GetVkDevice(), m_shaderModules[0], nullptr);

	if (static_cast<uint32_t>(m_shaderStages & RendererAPI::ShaderStage::RayTracing))
		vkDestroyShaderModule(m_device->GetVkDevice(), m_shaderModules[0], nullptr);

	m_reflection.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::array<uint32_t, 3>& TRAP::Graphics::API::VulkanShader::GetNumThreadsPerGroup() const
{
	return m_numThreadsPerGroup;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkShaderModule>& TRAP::Graphics::API::VulkanShader::GetVkShaderModules() const
{
	return m_shaderModules;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::API::ShaderReflection::PipelineReflection> TRAP::Graphics::API::VulkanShader::GetReflection() const
{
	return m_reflection;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::string>& TRAP::Graphics::API::VulkanShader::GetEntryNames() const
{
	return m_entryNames;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Use(Window* window)
{
	dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer().get())->BindShader(this, window);

	if(!m_firstUBOBind)
		return;

	for(const auto& UBOSet : m_UBOs)
	{
		for (const auto& UBOBinding : UBOSet.second)
		{
			//Bind UBO
			//Currently assumes "UpdateFreqNone" for Static & "UpdateFreqPerFrame" for Dynamic
			std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(1);
			params[0].Name = UBOBinding.second->GetName().c_str();
			params[0].Offset = TRAP::Graphics::RendererAPI::DescriptorData::BufferOffset{};

			if(UBOBinding.second->GetBufferUsage() == BufferUsage::Static)
			{
				params[0].Resource = std::vector<TRAP::Graphics::Buffer*>{UBOBinding.second->GetUBOs()[0].get()};
				GetDescriptorSets().StaticDescriptors->Update(0, params);
			}
			else
			{
				for(uint32_t i = 0; i < TRAP::Graphics::RendererAPI::ImageCount; ++i)
				{
					params[0].Resource = std::vector<TRAP::Graphics::Buffer*>{UBOBinding.second->GetUBOs()[i].get()};
					GetDescriptorSets().PerFrameDescriptors->Update(i, params);
				}
			}
		}
	}

	m_firstUBOBind = false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::UseTexture(const uint32_t set, const uint32_t binding,
                                                   TRAP::Graphics::Texture* const texture)
{
	TRAP_ASSERT(texture, "Texture is nullptr!");

	//OPTIMIZE Use index into root signature instead of name
	std::string name = RetrieveDescriptorName(set, binding, RendererAPI::DescriptorType::Texture);

	if(name.empty())
	{
		TP_ERROR(Log::RendererVulkanShaderPrefix, "Texture with invalid set and/or binding provided!");
		return;
	}

	std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(1);
	params[0].Name = name.c_str();
	params[0].Resource = std::vector<TRAP::Graphics::TextureBase*>{texture->GetTexture().get()};
	if(set == 0) //None
		GetDescriptorSets().StaticDescriptors->Update(0, params);
	else if(set == 1) //Per Frame
	{
		for(uint32_t i = 0; i < TRAP::Graphics::RendererAPI::ImageCount; ++i)
			GetDescriptorSets().PerFrameDescriptors->Update(i, params);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::UseTextures(const uint32_t set, const uint32_t binding,
													const std::vector<TRAP::Graphics::Texture*>& textures)
{
	TRAP_ASSERT(!textures.empty(), "Textures are empty!");

	//OPTIMIZE Use index into root signature instead of name
	std::string name = RetrieveDescriptorName(set, binding, RendererAPI::DescriptorType::Texture,
	                                          static_cast<uint32_t>(textures.size()));

	if(name.empty())
	{
		TP_ERROR(Log::RendererVulkanShaderPrefix, "Textures with invalid set and/or binding provided!");
		return;
	}

	std::vector<TRAP::Graphics::TextureBase*> textureBases(textures.size());
	for(uint32_t i = 0; i < textureBases.size(); ++i)
		textureBases[i] = textures[i]->GetTexture().get();

	std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(1);
	params[0].Name = name.c_str();
	params[0].Resource = textureBases;
	params[0].Count = static_cast<uint32_t>(textures.size());
	if(set == 0) //None
		GetDescriptorSets().StaticDescriptors->Update(0, params);
	else if(set == 1) //Per Frame
	{
		for(uint32_t i = 0; i < TRAP::Graphics::RendererAPI::ImageCount; ++i)
			GetDescriptorSets().PerFrameDescriptors->Update(i, params);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::UseSampler(const uint32_t set, const uint32_t binding,
	                                               TRAP::Graphics::Sampler* const sampler)
{
	TRAP_ASSERT(sampler, "Sampler is nullptr!");

	//OPTIMIZE Use index into root signature instead of name
	std::string name = RetrieveDescriptorName(set, binding, RendererAPI::DescriptorType::Sampler);

	if(name.empty())
	{
		TP_ERROR(Log::RendererVulkanShaderPrefix, "Sampler with invalid set and/or binding provided!");
		return;
	}

	std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(1);
	params[0].Name = name.c_str();
	params[0].Resource = std::vector<TRAP::Graphics::Sampler*>{sampler};
	if(set == 0) //None
		GetDescriptorSets().StaticDescriptors->Update(0, params);
	else if(set == 1) //Per Frame
	{
		for(uint32_t i = 0; i < TRAP::Graphics::RendererAPI::ImageCount; ++i)
			GetDescriptorSets().PerFrameDescriptors->Update(i, params);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::UseSamplers(const uint32_t set, const uint32_t binding,
	                                                const std::vector<TRAP::Graphics::Sampler*>& samplers)
{
	TRAP_ASSERT(!samplers.empty(), "Samplers are empty!");

	//OPTIMIZE Use index into root signature instead of name
	std::string name = RetrieveDescriptorName(set, binding, RendererAPI::DescriptorType::Sampler,
	                                          static_cast<uint32_t>(samplers.size()));

	if(name.empty())
	{
		TP_ERROR(Log::RendererVulkanShaderPrefix, "Samplers with invalid set and/or binding provided!");
		return;
	}

	std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(1);
	params[0].Name = name.c_str();
	params[0].Resource = samplers;
	params[0].Count = static_cast<uint32_t>(samplers.size());
	if(set == 0) //None
		GetDescriptorSets().StaticDescriptors->Update(0, params);
	else if(set == 1) //Per Frame
	{
		for(uint32_t i = 0; i < TRAP::Graphics::RendererAPI::ImageCount; ++i)
			GetDescriptorSets().PerFrameDescriptors->Update(i, params);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::VulkanShader::RetrieveDescriptorName(const uint32_t set, const uint32_t binding,
																	  RendererAPI::DescriptorType type,
																	  const uint32_t size)
{
	for(const auto& resource : m_reflection->ShaderResources)
	{
		if(resource.Type == type && resource.Set == set && resource.Reg == binding && resource.Size == size)
			return resource.Name;
	}

	return "";
}