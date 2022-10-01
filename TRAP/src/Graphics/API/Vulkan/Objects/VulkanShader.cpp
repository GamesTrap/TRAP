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
#include "Graphics/Textures/Texture.h"
#include "Application.h"

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, std::filesystem::path filepath,
												const RendererAPI::BinaryShaderDesc& desc,
                                                const std::vector<Macro>* const userMacros, const bool valid)
	: m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice()),
	  m_numThreadsPerGroup(),
	  m_shaderModules(static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)),
	  m_reflection(nullptr),
	  m_entryNames(static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)),
	  m_dirtyDescriptorSets(),
	  m_cleanedDescriptorSets(),
	  m_lastImageIndex(std::numeric_limits<uint32_t>::max())
{
	ZoneScoped;

	m_name = std::move(name);
	m_filepath = std::move(filepath);
	m_valid = valid;
	m_shaderStages = desc.Stages;

	if(userMacros)
		m_macros = *userMacros;

	TRAP_ASSERT(m_device, "device is nullptr");

	if(!m_valid)
		return;

	Init(desc);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, const RendererAPI::BinaryShaderDesc& desc,
                                                const std::vector<Macro>* const userMacros, const bool valid)
	: m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice()),
	  m_numThreadsPerGroup(),
	  m_shaderModules(static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)),
	  m_reflection(nullptr),
	  m_entryNames(static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)),
	  m_dirtyDescriptorSets(),
	  m_cleanedDescriptorSets(),
	  m_lastImageIndex(std::numeric_limits<uint32_t>::max())
{
	ZoneScoped;

	m_name = std::move(name);
	m_valid = valid;
	m_shaderStages = desc.Stages;

	if(userMacros)
		m_macros = *userMacros;

	TRAP_ASSERT(m_device, "device is nullptr");

	if(!m_valid)
		return;

	Init(desc);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, std::filesystem::path filepath,
                                                const std::vector<Macro>* const userMacros,
												const RendererAPI::ShaderStage stages)
	: m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice()),
	  m_numThreadsPerGroup(),
	  m_shaderModules(static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)),
	  m_reflection(nullptr),
	  m_entryNames(static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)),
	  m_dirtyDescriptorSets(),
	  m_cleanedDescriptorSets(),
	  m_lastImageIndex(std::numeric_limits<uint32_t>::max())
{
	ZoneScoped;

	m_name = std::move(name);
	m_filepath = std::move(filepath);
	m_valid = false;
	m_shaderStages = stages;

	if(userMacros)
		m_macros = *userMacros;

	TRAP_ASSERT(m_device, "device is nullptr");

	//Intentionally not calling InitShader() as this is always an invalid shader
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::~VulkanShader()
{
	ZoneScoped;

	Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkShaderModule>& TRAP::Graphics::API::VulkanShader::GetVkShaderModules() const
{
	ZoneScoped;

	return m_shaderModules;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::API::ShaderReflection::PipelineReflection> TRAP::Graphics::API::VulkanShader::GetReflection() const
{
	ZoneScoped;

	return m_reflection;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::string>& TRAP::Graphics::API::VulkanShader::GetEntryNames() const
{
	ZoneScoped;

	return m_entryNames;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Use(const Window* window)
{
	ZoneScoped;

	if(!window)
		window = Application::GetWindow();

	dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->BindShader(this, window);

	//Following some descriptor set allocation and reusing logic

	const uint32_t currImageIndex = RendererAPI::GetCurrentImageIndex(window);

	if(m_lastImageIndex != std::numeric_limits<uint32_t>::max())
	{
		//Move last dirty descriptor sets to cleaned descriptor sets
		if(currImageIndex != m_lastImageIndex && !m_dirtyDescriptorSets[m_lastImageIndex].empty())
			m_cleanedDescriptorSets[m_lastImageIndex] = std::move(m_dirtyDescriptorSets[m_lastImageIndex]);

		//Set current descriptor sets as dirty
		for(std::size_t i = 0; i < m_descriptorSets.size(); ++i)
			m_dirtyDescriptorSets[currImageIndex].push_back(std::move(m_descriptorSets[i]));
	}

	//Get a clean descriptor set
	if(m_lastImageIndex == std::numeric_limits<uint32_t>::max() ||
	   m_cleanedDescriptorSets[currImageIndex].empty()) //Slow path
	{
		//Descriptor sets are now dirty, so we need new ones
		const VulkanRootSignature* const root = dynamic_cast<VulkanRootSignature*>(m_rootSignature.get());
		RendererAPI::DescriptorSetDesc setDesc{};
		setDesc.RootSignature = m_rootSignature;
		for(std::size_t i = 0; i < m_descriptorSets.size(); ++i)
		{
			if(root->GetVkDescriptorSetLayouts()[i] != VK_NULL_HANDLE)
			{
				setDesc.MaxSets = (i == 0) ? 1 : RendererAPI::ImageCount;
				setDesc.Set = i;
				m_descriptorSets[i] = RendererAPI::GetDescriptorPool()->RetrieveDescriptorSet(setDesc);
			}
		}
	}
	else //Fast path
	{
		for(auto it = m_descriptorSets.rbegin(); it != m_descriptorSets.rend(); ++it)
		{
			*it = std::move(m_cleanedDescriptorSets[currImageIndex].back());
			m_cleanedDescriptorSets[currImageIndex].pop_back();
		}
	}

	m_lastImageIndex = currImageIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::UseTexture(const uint32_t set, const uint32_t binding,
                                                   Ref<TRAP::Graphics::Texture> const texture, const Window* window) const
{
	ZoneScoped;

	TRAP_ASSERT(texture, "Texture is nullptr!");

	if(!m_valid)
		return;

	if(!window)
		window = TRAP::Application::GetWindow();

	//OPTIMIZE Use index into root signature instead of name
	bool shaderUAV = false;
	const std::string name = RetrieveDescriptorName(set, binding, (RendererAPI::DescriptorType::Texture |
	                                                               RendererAPI::DescriptorType::TextureCube |
																   texture->GetDescriptorTypes()), &shaderUAV);

	if(name.empty())
	{
		//TP_ERROR(Log::RendererVulkanShaderPrefix, "Texture with invalid set and/or binding provided!");
		return;
	}

	std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(1);
	params[0].Name = name.c_str();
	params[0].Resource = std::vector<Ref<TRAP::Graphics::Texture>>{texture};

	if(shaderUAV && static_cast<bool>(texture->GetDescriptorTypes() & RendererAPI::DescriptorType::RWTexture))
		params[0].Offset = RendererAPI::DescriptorData::TextureSlice{};

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
													const std::vector<Ref<TRAP::Graphics::Texture>>& textures,
													const Window* window) const
{
	ZoneScoped;

	TRAP_ASSERT(!textures.empty(), "Textures are empty!");

	if(!m_valid)
		return;

	if(!window)
		window = TRAP::Application::GetWindow();

	//OPTIMIZE Use index into root signature instead of name
	bool shaderUAV = false;
	const std::string name = RetrieveDescriptorName(set, binding, (RendererAPI::DescriptorType::Texture |
	                                                               RendererAPI::DescriptorType::TextureCube |
																   textures[0]->GetDescriptorTypes()),
	                                                &shaderUAV, textures.size());

	if(name.empty())
	{
		//TP_ERROR(Log::RendererVulkanShaderPrefix, "Textures with invalid set and/or binding provided!");
		return;
	}

	std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(1);
	params[0].Name = name.c_str();
	params[0].Resource = textures;
	params[0].Count = static_cast<uint32_t>(textures.size());

	if(shaderUAV && static_cast<bool>(textures[0]->GetDescriptorTypes() & RendererAPI::DescriptorType::RWTexture))
		params[0].Offset = RendererAPI::DescriptorData::TextureSlice{};

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
	                                               TRAP::Graphics::Sampler* const sampler, const Window* window) const
{
	ZoneScoped;

	TRAP_ASSERT(sampler, "Sampler is nullptr!");

	if(!m_valid)
		return;

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
													const Window* window) const
{
	ZoneScoped;

	TRAP_ASSERT(!samplers.empty(), "Samplers are empty!");

	if(!m_valid)
		return;

	if(!window)
		window = TRAP::Application::GetWindow();

	//OPTIMIZE Use index into root signature instead of name
	const std::string name = RetrieveDescriptorName(set, binding, RendererAPI::DescriptorType::Sampler,
	                                                nullptr, samplers.size());

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
                                               const TRAP::Graphics::UniformBuffer* const uniformBuffer,
											   const uint64_t size,  const uint64_t offset, const Window* window) const
{
	ZoneScoped;

	TRAP_ASSERT(uniformBuffer, "UniformBuffer is nullptr!");

	if(!m_valid)
		return;

	if(!window)
		window = TRAP::Application::GetWindow();

	const uint32_t UBOIndex = (uniformBuffer->GetUpdateFrequency() == RendererAPI::DescriptorUpdateFrequency::Static) ?
		                          0 : RendererAPI::GetCurrentImageIndex(window);

	UseBuffer(set, binding, uniformBuffer->GetUBOs()[UBOIndex].get(), size ? size : uniformBuffer->GetSize(), offset, window);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::UseSSBO(const uint32_t set, const uint32_t binding,
                                                const TRAP::Graphics::StorageBuffer* const storageBuffer,
											    const uint64_t size, const Window* window) const
{
	ZoneScoped;

	TRAP_ASSERT(storageBuffer, "StorageBuffer is nullptr!");

	if(!m_valid)
		return;

	if(!window)
		window = TRAP::Application::GetWindow();

	const uint32_t SSBOIndex = (storageBuffer->GetUpdateFrequency() == RendererAPI::DescriptorUpdateFrequency::Static) ?
		                           0 : RendererAPI::GetCurrentImageIndex(window);

	UseBuffer(set, binding, storageBuffer->GetSSBOs()[SSBOIndex].get(), size ? size : storageBuffer->GetSize(), 0, window);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::array<uint32_t, 3>& TRAP::Graphics::API::VulkanShader::GetNumThreadsPerGroup() const
{
	ZoneScoped;

	return m_numThreadsPerGroup;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Init(const RendererAPI::BinaryShaderDesc& desc)
{
	ZoneScoped;

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanShaderPrefix, "Creating Shader: \"", m_name, "\"");
#endif

	uint32_t counter = 0;

	std::array<ShaderReflection::ShaderReflection,
	           static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> stageReflections{};

	for(std::size_t i = 0; i < stageReflections.size(); i++)
	{
		const RendererAPI::ShaderStage stageMask = static_cast<RendererAPI::ShaderStage>(BIT(i));
		if(stageMask == (m_shaderStages & stageMask))
		{
			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;

			const RendererAPI::BinaryShaderStageDesc* stageDesc = nullptr;

			const std::string stageName = m_name + "_";
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
					[[fallthrough]];
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

	//Set work group sizes if compute shader
	if(m_reflection->ShaderStages == RendererAPI::ShaderStage::Compute)
		m_numThreadsPerGroup = m_reflection->StageReflections[0].NumThreadsPerGroup;

	//Create DescriptorSets
	for(std::size_t i = 0; i < m_descriptorSets.size(); ++i)
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

void TRAP::Graphics::API::VulkanShader::Shutdown()
{
	ZoneScoped;

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanShaderPrefix, "Destroying Shader: \"", m_name, "\"");
#endif

	if(!m_reflection || !m_valid)
	{
		m_shaderModules = {};
		m_shaderStages = RendererAPI::ShaderStage::None;
	}

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

	m_shaderModules = {};
	m_shaderStages = RendererAPI::ShaderStage::None;
	m_reflection.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::SetShaderStageName(const std::string_view name, VkShaderModule stage) const
{
	ZoneScoped;

	if(!m_valid)
		return;

	if(!VulkanRenderer::s_debugMarkerSupport)
		return;

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	VkSetObjectName(m_device->GetVkDevice(), Utils::BitCast<VkShaderModule, uint64_t>(stage), VK_OBJECT_TYPE_SHADER_MODULE, name);
#else
	VkSetObjectName(m_device->GetVkDevice(), Utils::BitCast<VkShaderModule, uint64_t>(stage), VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT, name);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::UseBuffer(const uint32_t set, const uint32_t binding,
												  TRAP::Graphics::Buffer* const buffer, uint64_t size, uint64_t offset,
												  const Window* const window) const
{
	ZoneScoped;

	//OPTIMIZE Use index into root signature instead of name
	std::string name = RetrieveDescriptorName(set, binding,
	                                          buffer->GetDescriptors(),
											  nullptr, buffer->GetSize());
	bool isDynamic = false;
	if(name.empty()) //Try again as this might be a dynamic Buffer
	{
		//OPTIMIZE Use index into root signature instead of name
		name = RetrieveDescriptorName(set, binding, buffer->GetDescriptors(), nullptr, 1);
		if(!name.empty())
			isDynamic = true;
		else //Try again as this might be a storage buffer
			name = RetrieveDescriptorName(set, binding, buffer->GetDescriptors(), nullptr, 0);
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
																	  bool* const outUAV,
																	  const uint64_t size) const
{
	ZoneScoped;

	for(const auto& resource : m_reflection->ShaderResources)
	{
		if(static_cast<bool>(resource.Type & type) && resource.Set == set && resource.Reg == binding && resource.Size == size)
		{
			if(outUAV)
				*outUAV = static_cast<bool>(resource.Type & (RendererAPI::DescriptorType::RWTexture | RendererAPI::DescriptorType::RWBuffer));

			return resource.Name;
		}
	}

	return "";
}
