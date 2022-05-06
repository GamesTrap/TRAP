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
#include "Graphics/API/Objects/Buffer.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Textures/TextureManager.h"
#include "Application.h"

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, const RendererAPI::BinaryShaderDesc& desc)
	: m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice()),
	  m_numThreadsPerGroup(),
	  m_shaderModules(static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)),
	  m_reflection(nullptr),
	  m_entryNames(static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT))
{
	m_name = std::move(name);

	TRAP_ASSERT(m_device, "device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
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

			std::string stageName = m_name + "_";
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
#ifdef ENABLE_GRAPHICS_DEBUG
					if (!m_name.empty())
						SetShaderStageName(m_name + "_Vertex", m_shaderModules[counter]);
#endif
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
#ifdef ENABLE_GRAPHICS_DEBUG
					if (!m_name.empty())
						SetShaderStageName(m_name + "_TessellationControl", m_shaderModules[counter]);
#endif
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
#ifdef ENABLE_GRAPHICS_DEBUG
					if (!m_name.empty())
						SetShaderStageName(m_name + "_TessellationEvaluation", m_shaderModules[counter]);
#endif
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
#ifdef ENABLE_GRAPHICS_DEBUG
					if (!m_name.empty())
						SetShaderStageName(m_name + "_Geometry", m_shaderModules[counter]);
#endif
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
#ifdef ENABLE_GRAPHICS_DEBUG
					if (!m_name.empty())
						SetShaderStageName(m_name + "_Fragment", m_shaderModules[counter]);
#endif
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
#ifdef ENABLE_GRAPHICS_DEBUG
					if (!m_name.empty())
						SetShaderStageName(m_name + "_Compute/RayTracing", m_shaderModules[counter]);
#endif
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

	//Create DescriptorSets
	for(uint32_t i = 0; i < m_descriptorSets.size(); ++i)
	{
		if(dynamic_cast<VulkanRootSignature*>
		(
			m_rootSignature.get()
		)->GetVkDescriptorSetLayouts()[i] != VK_NULL_HANDLE)
		{
			RendererAPI::DescriptorSetDesc setDesc{};
			setDesc.MaxSets = (i == 0) ? 1 : RendererAPI::ImageCount;
			setDesc.RootSignature = m_rootSignature;
			setDesc.Set = i;
			m_descriptorSets[i] = RendererAPI::GetDescriptorPool()->RetrieveDescriptorSet(setDesc);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::~VulkanShader()
{
#ifdef VERBOSE_GRAPHICS_DEBUG
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
	dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->BindShader(this, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::UseTexture(const uint32_t set, const uint32_t binding,
                                                   TRAP::Graphics::Texture* const texture, Window* window)
{
	TRAP_ASSERT(texture, "Texture is nullptr!");

	if(!window)
		window = TRAP::Application::GetWindow();

	//OPTIMIZE Use index into root signature instead of name
	const std::string name = RetrieveDescriptorName(set, binding, RendererAPI::DescriptorType::Texture);

	if(name.empty())
	{
		//TP_ERROR(Log::RendererVulkanShaderPrefix, "Texture with invalid set and/or binding provided!");
		return;
	}

	std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(1);
	params[0].Name = name.c_str();
	params[0].Resource = std::vector<TRAP::Graphics::TextureBase*>{texture->GetTexture().get()};
	if(set == static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::Static))
		GetDescriptorSets()[set]->Update(0, params);
	else
	{
		const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(window);
		GetDescriptorSets()[set]->Update(imageIndex, params);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::UseTextures(const uint32_t set, const uint32_t binding,
													const std::vector<TRAP::Graphics::Texture*>& textures,
													Window* window)
{
	TRAP_ASSERT(!textures.empty(), "Textures are empty!");

	if(!window)
		window = TRAP::Application::GetWindow();

	//OPTIMIZE Use index into root signature instead of name
	const std::string name = RetrieveDescriptorName(set, binding, RendererAPI::DescriptorType::Texture,
	                                                textures.size());

	if(name.empty())
	{
		//TP_ERROR(Log::RendererVulkanShaderPrefix, "Textures with invalid set and/or binding provided!");
		return;
	}

	std::vector<TRAP::Graphics::TextureBase*> textureBases(textures.size());
	for(std::size_t i = 0; i < textureBases.size(); ++i)
		textureBases[i] = textures[i]->GetTexture().get();

	std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(1);
	params[0].Name = name.c_str();
	params[0].Resource = textureBases;
	params[0].Count = static_cast<uint32_t>(textures.size());
	if(set == static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::Static))
		GetDescriptorSets()[set]->Update(0, params);
	else
	{
		const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(window);
		GetDescriptorSets()[set]->Update(imageIndex, params);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::UseSampler(const uint32_t set, const uint32_t binding,
	                                               TRAP::Graphics::Sampler* const sampler, Window* window)
{
	TRAP_ASSERT(sampler, "Sampler is nullptr!");

	if(!window)
		window = TRAP::Application::GetWindow();

	//OPTIMIZE Use index into root signature instead of name
	const std::string name = RetrieveDescriptorName(set, binding, RendererAPI::DescriptorType::Sampler);

	if(name.empty())
	{
		//TP_ERROR(Log::RendererVulkanShaderPrefix, "Sampler with invalid set and/or binding provided!");
		return;
	}

	std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(1);
	params[0].Name = name.c_str();
	params[0].Resource = std::vector<TRAP::Graphics::Sampler*>{sampler};
	if(set == static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::Static))
		GetDescriptorSets()[set]->Update(0, params);
	else
	{
		const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(window);
		GetDescriptorSets()[set]->Update(imageIndex, params);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::UseSamplers(const uint32_t set, const uint32_t binding,
	                                                const std::vector<TRAP::Graphics::Sampler*>& samplers,
													Window* window)
{
	TRAP_ASSERT(!samplers.empty(), "Samplers are empty!");

	if(!window)
		window = TRAP::Application::GetWindow();

	//OPTIMIZE Use index into root signature instead of name
	const std::string name = RetrieveDescriptorName(set, binding, RendererAPI::DescriptorType::Sampler,
	                                                samplers.size());

	if(name.empty())
	{
		//TP_ERROR(Log::RendererVulkanShaderPrefix, "Samplers with invalid set and/or binding provided!");
		return;
	}

	std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(1);
	params[0].Name = name.c_str();
	params[0].Resource = samplers;
	params[0].Count = static_cast<uint32_t>(samplers.size());
	if(set == static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::Static))
		GetDescriptorSets()[set]->Update(0, params);
	else
	{
		const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(window);
		GetDescriptorSets()[set]->Update(imageIndex, params);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::UseUBO(const uint32_t set, const uint32_t binding,
                                               TRAP::Graphics::UniformBuffer* uniformBuffer,
											   const uint64_t size,  const uint64_t offset, Window* window)
{
	TRAP_ASSERT(uniformBuffer, "UniformBuffer is nullptr!");

	if(!window)
		window = TRAP::Application::GetWindow();

	const uint32_t UBOIndex = (uniformBuffer->GetUpdateFrequency() == RendererAPI::DescriptorUpdateFrequency::Static) ?
		                          0 : RendererAPI::GetCurrentImageIndex(window);

	UseBuffer(set, binding, uniformBuffer->GetUBOs()[UBOIndex].get(), size, offset, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::UseSSBO(const uint32_t set, const uint32_t binding,
                                                TRAP::Graphics::StorageBuffer* storageBuffer,
											    const uint64_t size, Window* window)
{
	TRAP_ASSERT(storageBuffer, "StorageBuffer is nullptr!");

	if(!window)
		window = TRAP::Application::GetWindow();

	const uint32_t SSBOIndex = (storageBuffer->GetUpdateFrequency() == RendererAPI::DescriptorUpdateFrequency::Static) ?
		                           0 : RendererAPI::GetCurrentImageIndex(window);

	UseBuffer(set, binding, storageBuffer->GetSSBOs()[SSBOIndex].get(), size, 0, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::SetShaderStageName(const std::string_view name, VkShaderModule stage) const
{
	VkSetObjectName(m_device->GetVkDevice(), reinterpret_cast<uint64_t>(stage), VK_OBJECT_TYPE_SHADER_MODULE, name);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::UseBuffer(const uint32_t set, const uint32_t binding,
												  TRAP::Graphics::Buffer* buffer, uint64_t size, uint64_t offset,
												  Window* window)
{
	//OPTIMIZE Use index into root signature instead of name
	std::string name = RetrieveDescriptorName(set, binding,
	                                          buffer->GetDescriptors(),
											  buffer->GetSize());
	bool isDynamic = false;
	if(name.empty()) //Try again as this might be a dynamic Buffer
	{
		//OPTIMIZE Use index into root signature instead of name
		name = RetrieveDescriptorName(set, binding, buffer->GetDescriptors(), 1);
		if(!name.empty())
			isDynamic = true;
		else //Try again as this might be a storage buffer
			name = RetrieveDescriptorName(set, binding, buffer->GetDescriptors(), 0);
	}

	if(name.empty()) //Unable to find Buffer @ with set, binding & size
	{
		//TP_ERROR(Log::RendererVulkanShaderPrefix, "UniformBuffer with invalid set and/or binding & size provided!");
		return;
	}

	//Bind Buffer
	std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(1);
	params[0].Name = name.c_str();
	if(isDynamic)
	{
		TRAP::Graphics::RendererAPI::DescriptorData::BufferOffset off{};
		off.Offsets.emplace_back(offset);
		off.Sizes.emplace_back(size);
		params[0].Offset = off;
	}
	else
		params[0].Offset = TRAP::Graphics::RendererAPI::DescriptorData::BufferOffset{};

	if(set == static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::Static))
	{
		params[0].Resource = std::vector<TRAP::Graphics::Buffer*>{buffer};
		GetDescriptorSets()[set]->Update(0, params);
	}
	else
	{
		const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(window);
		params[0].Resource = std::vector<TRAP::Graphics::Buffer*>{buffer};
		GetDescriptorSets()[set]->Update(imageIndex, params);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::VulkanShader::RetrieveDescriptorName(const uint32_t set, const uint32_t binding,
                                                                      const RendererAPI::DescriptorType type,
																	  const uint64_t size) const
{
	for(const auto& resource : m_reflection->ShaderResources)
	{
		if(resource.Type == type && resource.Set == set && resource.Reg == binding && resource.Size == size)
			return resource.Name;
	}

	return "";
}
