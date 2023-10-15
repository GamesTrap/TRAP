#include "TRAPPCH.h"
#include "VulkanShader.h"

#include "Graphics/API/ShaderReflection.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/VulkanShaderReflection.h"
#include "Graphics/API/Vulkan/Objects/VulkanDevice.h"
#include "Graphics/API/Vulkan/Objects/VulkanRootSignature.h"
#include "Graphics/API/Objects/RootSignature.h"
#include "Graphics/API/Objects/DescriptorSet.h"
#include "Graphics/API/Objects/DescriptorPool.h"
#include "Graphics/API/Objects/Buffer.h"
#include "Graphics/Textures/Texture.h"
#include "Graphics/Buffers/UniformBuffer.h"
#include "Graphics/Buffers/StorageBuffer.h"

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, std::filesystem::path filepath,
												const RendererAPI::BinaryShaderDesc& desc,
                                                const std::vector<Macro>* const userMacros, const bool valid)
	: Shader(name, valid, desc.Stages, userMacros, filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanShader(): Vulkan Device is nullptr");

	if(!m_valid)
		return;

	Init(desc);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, const RendererAPI::BinaryShaderDesc& desc,
                                                const std::vector<Macro>* const userMacros, const bool valid)
	: Shader(name, valid, desc.Stages, userMacros)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanShader(): Vulkan Device is nullptr");

	if(!m_valid)
		return;

	Init(desc);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, std::filesystem::path filepath,
                                                const std::vector<Macro>* const userMacros,
												const RendererAPI::ShaderStage stages)
	: Shader(name, false, stages, userMacros, filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanShader(): Vulkan Device is nullptr");

	//Intentionally not calling InitShader() as this is always an invalid shader
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::~VulkanShader()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::ShaderReflection::PipelineReflection> TRAP::Graphics::API::VulkanShader::GetReflection() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_reflection;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::Use(const Window* const window)
#else
void TRAP::Graphics::API::VulkanShader::Use()
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanShader::Use(): Window is nullptr");

	dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->BindShader(this, window);
#else
	dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->BindShader(this);
#endif /*TRAP_HEADLESS_MODE*/

	if(m_rootSignature) //Only do the following if the shader actually uses descriptors
	{
		//Following some descriptor set allocation and reusing logic

#ifndef TRAP_HEADLESS_MODE
		const uint32_t currImageIndex = RendererAPI::GetCurrentImageIndex(window);
#else
		const uint32_t currImageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

		if(m_lastImageIndex != std::numeric_limits<uint32_t>::max())
		{
			//Move last dirty descriptor sets to cleaned descriptor sets
			if(currImageIndex != m_lastImageIndex && !m_dirtyDescriptorSets[m_lastImageIndex].empty())
				m_cleanedDescriptorSets[m_lastImageIndex] = std::move(m_dirtyDescriptorSets[m_lastImageIndex]);

			//Set current descriptor sets as dirty
			for(auto & m_descriptorSet : m_descriptorSets)
				m_dirtyDescriptorSets[currImageIndex].push_back(std::move(m_descriptorSet));
		}

		//Get a clean descriptor set
		if(m_lastImageIndex == std::numeric_limits<uint32_t>::max() ||
		m_cleanedDescriptorSets[currImageIndex].empty()) //Slow path
		{
			//Descriptor sets are now dirty, so we need new ones
			const Ref<VulkanRootSignature> root = std::dynamic_pointer_cast<VulkanRootSignature>(m_rootSignature);
			RendererAPI::DescriptorSetDesc setDesc{};
			setDesc.RootSignature = m_rootSignature;
			for(std::size_t i = 0; i < m_descriptorSets.size(); ++i)
			{
				if(root->GetVkDescriptorSetLayouts()[i] != VK_NULL_HANDLE)
				{
					setDesc.MaxSets = (i == 0) ? 1 : RendererAPI::ImageCount;
					setDesc.Set = NumericCast<uint32_t>(i);
					m_descriptorSets[i] = RendererAPI::GetDescriptorPool()->RetrieveDescriptorSet(setDesc);
				}
			}
		}
		else //Fast path
		{
			for(auto& m_descriptorSet : std::ranges::reverse_view(m_descriptorSets))
			{
				m_descriptorSet = std::move(m_cleanedDescriptorSets[currImageIndex].back());
				m_cleanedDescriptorSets[currImageIndex].pop_back();
			}
		}

		m_lastImageIndex = currImageIndex;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::UseTexture(const uint32_t set, const uint32_t binding,
                                                   Ref<TRAP::Graphics::Texture> const texture, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanShader::UseTexture(const uint32_t set, const uint32_t binding,
                                                   Ref<TRAP::Graphics::Texture> const texture) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(texture, "VulkanShader::UseTexture(): Texture is nullptr!");
#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanShader::UseTexture(): Window is nullptr");
#endif /*TRAP_HEADLESS_MODE*/

	if(!m_valid)
		return;

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
	params[0].Name = name;
	params[0].Resource = std::vector<Ref<TRAP::Graphics::Texture>>{texture};

	if(shaderUAV && (texture->GetDescriptorTypes() & RendererAPI::DescriptorType::RWTexture) != RendererAPI::DescriptorType::Undefined)
		params[0].Offset = RendererAPI::DescriptorData::TextureSlice{};

	if(set == std::to_underlying(RendererAPI::DescriptorUpdateFrequency::Static))
		GetDescriptorSets()[set]->Update(0, params);
	else
	{
#ifndef TRAP_HEADLESS_MODE
		const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(window);
#else
		const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/
		GetDescriptorSets()[set]->Update(imageIndex, params);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::UseTextures(const uint32_t set, const uint32_t binding,
													const std::vector<Ref<TRAP::Graphics::Texture>>& textures,
													const Window* const window) const
#else
void TRAP::Graphics::API::VulkanShader::UseTextures(const uint32_t set, const uint32_t binding,
													const std::vector<Ref<TRAP::Graphics::Texture>>& textures) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(!textures.empty(), "VulkanShader::UseTextures(): Textures are empty!");
#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanShader::UseTextures(): Window is nullptr");
#endif /*TRAP_HEADLESS_MODE*/

	if(!m_valid)
		return;

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
	params[0].Name = name;
	params[0].Resource = textures;
	params[0].Count = NumericCast<uint32_t>(textures.size());

	if(shaderUAV && (textures[0]->GetDescriptorTypes() & RendererAPI::DescriptorType::RWTexture) != RendererAPI::DescriptorType::Undefined)
		params[0].Offset = RendererAPI::DescriptorData::TextureSlice{};

	if(set == std::to_underlying(RendererAPI::DescriptorUpdateFrequency::Static))
		GetDescriptorSets()[set]->Update(0, params);
	else
	{
#ifndef TRAP_HEADLESS_MODE
		const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(window);
#else
		const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/
		GetDescriptorSets()[set]->Update(imageIndex, params);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::UseSampler(const uint32_t set, const uint32_t binding,
	                                               TRAP::Graphics::Sampler* const sampler, const Window* const window) const
#else
void TRAP::Graphics::API::VulkanShader::UseSampler(const uint32_t set, const uint32_t binding,
	                                               TRAP::Graphics::Sampler* const sampler) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(sampler, "VulkanShader::UseSampler(): Sampler is nullptr!");
#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanShader::UseSampler(): Window is nullptr");
#endif /*TRAP_HEADLESS_MODE*/

	if(!m_valid)
		return;

	//OPTIMIZE Use index into root signature instead of name
	const std::string name = RetrieveDescriptorName(set, binding, RendererAPI::DescriptorType::Sampler);

	if(name.empty())
	{
		//TP_ERROR(Log::RendererVulkanShaderPrefix, "Sampler with invalid set and/or binding provided!");
		return;
	}

	std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(1);
	params[0].Name = name;
	params[0].Resource = std::vector<TRAP::Graphics::Sampler*>{sampler};
	if(set == std::to_underlying(RendererAPI::DescriptorUpdateFrequency::Static))
		GetDescriptorSets()[set]->Update(0, params);
	else
	{
#ifndef TRAP_HEADLESS_MODE
		const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(window);
#else
		const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/
		GetDescriptorSets()[set]->Update(imageIndex, params);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::UseSamplers(const uint32_t set, const uint32_t binding,
	                                                const std::vector<TRAP::Graphics::Sampler*>& samplers,
													const Window* const window) const
#else
void TRAP::Graphics::API::VulkanShader::UseSamplers(const uint32_t set, const uint32_t binding,
	                                                const std::vector<TRAP::Graphics::Sampler*>& samplers) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(!samplers.empty(), "VulkanShader::UseSamplers(): Samplers are empty!");
#ifndef TRAP_HEADLESS_MODE
	TRAP_ASSERT(window, "VulkanShader::UseSamplers(): Window is nullptr");
#endif /*TRAP_HEADLESS_MODE*/

	if(!m_valid)
		return;

	//OPTIMIZE Use index into root signature instead of name
	const std::string name = RetrieveDescriptorName(set, binding, RendererAPI::DescriptorType::Sampler,
	                                                nullptr, samplers.size());

	if(name.empty())
	{
		//TP_ERROR(Log::RendererVulkanShaderPrefix, "Samplers with invalid set and/or binding provided!");
		return;
	}

	std::vector<TRAP::Graphics::RendererAPI::DescriptorData> params(1);
	params[0].Name = name;
	params[0].Resource = samplers;
	params[0].Count = NumericCast<uint32_t>(samplers.size());
	if(set == std::to_underlying(RendererAPI::DescriptorUpdateFrequency::Static))
		GetDescriptorSets()[set]->Update(0, params);
	else
	{
#ifndef TRAP_HEADLESS_MODE
		const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(window);
#else
		const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/
		GetDescriptorSets()[set]->Update(imageIndex, params);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::UseUBO(const uint32_t set, const uint32_t binding,
                                               const TRAP::Graphics::UniformBuffer* const uniformBuffer,
											   const uint64_t size, const uint64_t offset, const Window* const window) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(uniformBuffer, "VulkanShader::UseUBO(): UniformBuffer is nullptr!");
	TRAP_ASSERT(window, "VulkanShader::UseUBO(): Window is nullptr");

	if(!m_valid)
		return;

	const uint32_t UBOIndex = (uniformBuffer->GetUpdateFrequency() == RendererAPI::DescriptorUpdateFrequency::Static) ?
		                          0 : RendererAPI::GetCurrentImageIndex(window);

	UseBuffer(set, binding, uniformBuffer->GetUBOs()[UBOIndex].get(), size != 0u ? size : uniformBuffer->GetSize(), offset, window);
}
#else
void TRAP::Graphics::API::VulkanShader::UseUBO(const uint32_t set, const uint32_t binding,
                                               const TRAP::Graphics::UniformBuffer* const uniformBuffer,
											   const uint64_t size, const uint64_t offset) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(uniformBuffer, "VulkanShader::UseUBO(): UniformBuffer is nullptr!");

	if(!m_valid)
		return;

	const uint32_t UBOIndex = (uniformBuffer->GetUpdateFrequency() == RendererAPI::DescriptorUpdateFrequency::Static) ?
		                          0 : RendererAPI::GetCurrentImageIndex();

	UseBuffer(set, binding, uniformBuffer->GetUBOs()[UBOIndex].get(), size != 0u ? size : uniformBuffer->GetSize(), offset);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::UseSSBO(const uint32_t set, const uint32_t binding,
                                                const TRAP::Graphics::StorageBuffer* const storageBuffer,
											    const uint64_t size, const Window* const window) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(storageBuffer, "VulkanShader::UseSSBO(): StorageBuffer is nullptr!");
	TRAP_ASSERT(window, "VulkanShader::UseSSBO(): Window is nullptr");

	if(!m_valid)
		return;

	const uint32_t SSBOIndex = (storageBuffer->GetUpdateFrequency() == RendererAPI::DescriptorUpdateFrequency::Static) ?
		                           0 : RendererAPI::GetCurrentImageIndex(window);

	UseBuffer(set, binding, storageBuffer->GetSSBOs()[SSBOIndex].get(), size != 0u ? size : storageBuffer->GetSize(), 0, window);
}
#else
void TRAP::Graphics::API::VulkanShader::UseSSBO(const uint32_t set, const uint32_t binding,
                                                const TRAP::Graphics::StorageBuffer* const storageBuffer,
											    const uint64_t size) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(storageBuffer, "VulkanShader::UseSSBO(): StorageBuffer is nullptr!");

	if(!m_valid)
		return;

	const uint32_t SSBOIndex = (storageBuffer->GetUpdateFrequency() == RendererAPI::DescriptorUpdateFrequency::Static) ?
		                           0 : RendererAPI::GetCurrentImageIndex();

	UseBuffer(set, binding, storageBuffer->GetSSBOs()[SSBOIndex].get(), size != 0u ? size : storageBuffer->GetSize(), 0);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Init(const RendererAPI::BinaryShaderDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanShaderPrefix, "Creating Shader: \"", m_name, "\"");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	std::vector<ShaderReflection::ShaderReflection> stageReflections{};

	for(std::size_t i = 0; i < std::to_underlying(RendererAPI::ShaderStage::SHADER_STAGE_COUNT); i++)
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
					stageReflections.push_back(VkCreateShaderReflection(desc.Vertex.ByteCode, stageMask));

					createInfo.codeSize = desc.Vertex.ByteCode.size() * sizeof(uint32_t);
					createInfo.pCode = desc.Vertex.ByteCode.data();
					stageDesc = &desc.Vertex;
					m_shaderModules.emplace_back(VK_NULL_HANDLE);
					VkCall(vkCreateShaderModule(m_device->GetVkDevice(), &createInfo, nullptr,
					                            &m_shaderModules.back()));
#ifdef ENABLE_GRAPHICS_DEBUG
					if (!m_name.empty())
						SetShaderStageName(m_name + "_Vertex", m_shaderModules.back());
#endif /*ENABLE_GRAPHICS_DEBUG*/
					break;
				}

				case RendererAPI::ShaderStage::TessellationControl:
				{
					stageReflections.push_back(VkCreateShaderReflection(desc.TessellationControl.ByteCode,
					                                                    stageMask));

					createInfo.codeSize = desc.TessellationControl.ByteCode.size() * sizeof(uint32_t);
					createInfo.pCode = desc.TessellationControl.ByteCode.data();
					stageDesc = &desc.TessellationControl;
					m_shaderModules.emplace_back(VK_NULL_HANDLE);
					VkCall(vkCreateShaderModule(m_device->GetVkDevice(), &createInfo, nullptr,
					                            &m_shaderModules.back()));
#ifdef ENABLE_GRAPHICS_DEBUG
					if (!m_name.empty())
						SetShaderStageName(m_name + "_TessellationControl", m_shaderModules.back());
#endif /*ENABLE_GRAPHICS_DEBUG*/
					break;
				}

				case RendererAPI::ShaderStage::TessellationEvaluation:
				{
					stageReflections.push_back(VkCreateShaderReflection(desc.TessellationEvaluation.ByteCode,
					                                                    stageMask));

					createInfo.codeSize = desc.TessellationEvaluation.ByteCode.size() * sizeof(uint32_t);
					createInfo.pCode = desc.TessellationEvaluation.ByteCode.data();
					stageDesc = &desc.TessellationEvaluation;
					m_shaderModules.emplace_back(VK_NULL_HANDLE);
					VkCall(vkCreateShaderModule(m_device->GetVkDevice(), &createInfo, nullptr,
					                            &m_shaderModules.back()));
#ifdef ENABLE_GRAPHICS_DEBUG
					if (!m_name.empty())
						SetShaderStageName(m_name + "_TessellationEvaluation", m_shaderModules.back());
#endif /*ENABLE_GRAPHICS_DEBUG*/
					break;
				}

				case RendererAPI::ShaderStage::Geometry:
				{
					stageReflections.push_back(VkCreateShaderReflection(desc.Geometry.ByteCode, stageMask));

					createInfo.codeSize = desc.Geometry.ByteCode.size() * sizeof(uint32_t);
					createInfo.pCode = desc.Geometry.ByteCode.data();
					stageDesc = &desc.Geometry;
					m_shaderModules.emplace_back(VK_NULL_HANDLE);
					VkCall(vkCreateShaderModule(m_device->GetVkDevice(), &createInfo, nullptr,
					                            &m_shaderModules.back()));
#ifdef ENABLE_GRAPHICS_DEBUG
					if (!m_name.empty())
						SetShaderStageName(m_name + "_Geometry", m_shaderModules.back());
#endif /*ENABLE_GRAPHICS_DEBUG*/
					break;
				}

				case RendererAPI::ShaderStage::Fragment:
				{
					stageReflections.push_back(VkCreateShaderReflection(desc.Fragment.ByteCode, stageMask));

					createInfo.codeSize = desc.Fragment.ByteCode.size() * sizeof(uint32_t);
					createInfo.pCode = desc.Fragment.ByteCode.data();
					stageDesc = &desc.Fragment;
					m_shaderModules.emplace_back(VK_NULL_HANDLE);
					VkCall(vkCreateShaderModule(m_device->GetVkDevice(), &createInfo, nullptr,
					                            &m_shaderModules.back()));
#ifdef ENABLE_GRAPHICS_DEBUG
					if (!m_name.empty())
						SetShaderStageName(m_name + "_Fragment", m_shaderModules.back());
#endif /*ENABLE_GRAPHICS_DEBUG*/
					break;
				}

				case RendererAPI::ShaderStage::Compute:
					[[fallthrough]];
				case RendererAPI::ShaderStage::RayTracing:
				{
					stageReflections.push_back(VkCreateShaderReflection(desc.Compute.ByteCode, stageMask));

					createInfo.codeSize = desc.Compute.ByteCode.size() * sizeof(uint32_t);
					createInfo.pCode = desc.Compute.ByteCode.data();
					stageDesc = &desc.Compute;
					m_shaderModules.emplace_back(VK_NULL_HANDLE);
					VkCall(vkCreateShaderModule(m_device->GetVkDevice(), &createInfo, nullptr,
					                            &m_shaderModules.back()));
#ifdef ENABLE_GRAPHICS_DEBUG
					if (!m_name.empty())
						SetShaderStageName(m_name + "_Compute/RayTracing", m_shaderModules.back());
#endif /*ENABLE_GRAPHICS_DEBUG*/
					break;
				}

				default:
					TRAP_ASSERT(false, "VulkanShader::Init(): Shader Stage not supported!");
					break;
			}
			m_entryNames.push_back(stageDesc->EntryPoint);
		}
	}

	m_reflection = CreatePipelineReflection(stageReflections);

	RendererAPI::RootSignatureDesc rootDesc{};
	rootDesc.Shaders.push_back(this);
	m_rootSignature = RootSignature::Create(rootDesc);

	//Set work group sizes if compute shader
	if(m_reflection->ShaderStages == RendererAPI::ShaderStage::Compute)
		m_numThreadsPerGroup = m_reflection->StageReflections[0].NumThreadsPerGroup;

	//Create DescriptorSets
	for(std::size_t i = 0; i < m_descriptorSets.size(); ++i)
	{
		if(std::dynamic_pointer_cast<VulkanRootSignature>(m_rootSignature)->GetVkDescriptorSetLayouts()[i] != VK_NULL_HANDLE)
		{
			RendererAPI::DescriptorSetDesc setDesc{};
			setDesc.MaxSets = (i == 0) ? 1 : RendererAPI::ImageCount;
			setDesc.RootSignature = m_rootSignature;
			setDesc.Set = NumericCast<uint32_t>(i);
			m_descriptorSets[i] = RendererAPI::GetDescriptorPool()->RetrieveDescriptorSet(setDesc);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanShaderPrefix, "Destroying Shader: \"", m_name, "\"");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if(!m_reflection || !m_valid)
	{
		m_shaderModules = {};
		m_shaderStages = RendererAPI::ShaderStage::None;
	}

	if ((m_shaderStages & RendererAPI::ShaderStage::Vertex) != RendererAPI::ShaderStage::None)
		vkDestroyShaderModule(m_device->GetVkDevice(), m_shaderModules[m_reflection->VertexStageIndex.value()], nullptr);

	if ((m_shaderStages & RendererAPI::ShaderStage::TessellationControl) != RendererAPI::ShaderStage::None)
		vkDestroyShaderModule(m_device->GetVkDevice(), m_shaderModules[m_reflection->TessellationControlStageIndex.value()],
		                      nullptr);

	if ((m_shaderStages & RendererAPI::ShaderStage::TessellationEvaluation) != RendererAPI::ShaderStage::None)
		vkDestroyShaderModule(m_device->GetVkDevice(),
		                      m_shaderModules[m_reflection->TessellationEvaluationStageIndex.value()], nullptr);

	if ((m_shaderStages & RendererAPI::ShaderStage::Geometry) != RendererAPI::ShaderStage::None)
		vkDestroyShaderModule(m_device->GetVkDevice(), m_shaderModules[m_reflection->GeometryStageIndex.value()], nullptr);

	if ((m_shaderStages & RendererAPI::ShaderStage::Fragment) != RendererAPI::ShaderStage::None)
		vkDestroyShaderModule(m_device->GetVkDevice(), m_shaderModules[m_reflection->FragmentStageIndex.value()], nullptr);

	if ((m_shaderStages & RendererAPI::ShaderStage::Compute) != RendererAPI::ShaderStage::None)
		vkDestroyShaderModule(m_device->GetVkDevice(), m_shaderModules[0], nullptr);

	if ((m_shaderStages & RendererAPI::ShaderStage::RayTracing) != RendererAPI::ShaderStage::None)
		vkDestroyShaderModule(m_device->GetVkDevice(), m_shaderModules[0], nullptr);

	m_shaderModules = {};
	m_shaderStages = RendererAPI::ShaderStage::None;
	m_reflection.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::SetShaderStageName(const std::string_view name, VkShaderModule stage) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if(!m_valid)
		return;

	if(!VulkanRenderer::s_debugMarkerSupport)
		return;

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	VkSetObjectName(m_device->GetVkDevice(), std::bit_cast<uint64_t>(stage), VK_OBJECT_TYPE_SHADER_MODULE, name);
#else
	VkSetObjectName(m_device->GetVkDevice(), std::bit_cast<uint64_t>(stage), VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT, name);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::UseBuffer(const uint32_t set, const uint32_t binding,
												  TRAP::Graphics::Buffer* const buffer, uint64_t size, uint64_t offset,
												  const Window* const window) const
#else
void TRAP::Graphics::API::VulkanShader::UseBuffer(const uint32_t set, const uint32_t binding,
												  TRAP::Graphics::Buffer* const buffer, uint64_t size, uint64_t offset) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	params[0].Name = name;
	if(isDynamic)
	{
		TRAP::Graphics::RendererAPI::DescriptorData::BufferOffset off{};
		off.Offsets.emplace_back(offset);
		off.Sizes.emplace_back(size);
		params[0].Offset = off;
	}
	else
		params[0].Offset = TRAP::Graphics::RendererAPI::DescriptorData::BufferOffset{};

	if(set == std::to_underlying(RendererAPI::DescriptorUpdateFrequency::Static))
	{
		params[0].Resource = std::vector<TRAP::Graphics::Buffer*>{buffer};
		GetDescriptorSets()[set]->Update(0, params);
	}
	else
	{
#ifndef TRAP_HEADLESS_MODE
		const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex(window);
#else
		const uint32_t imageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/
		params[0].Resource = std::vector<TRAP::Graphics::Buffer*>{buffer};
		GetDescriptorSets()[set]->Update(imageIndex, params);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Graphics::API::VulkanShader::RetrieveDescriptorName(const uint32_t set, const uint32_t binding,
                                                                                    const RendererAPI::DescriptorType type,
																	                bool* const outUAV,
																	                const uint64_t size) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	for(const auto& resource : m_reflection->ShaderResources)
	{
		if((resource.Type & type) != RendererAPI::DescriptorType::Undefined &&
		   resource.Set == set && resource.Reg == binding && resource.Size == size)
		{
			if(outUAV != nullptr)
				*outUAV = (resource.Type & (RendererAPI::DescriptorType::RWTexture | RendererAPI::DescriptorType::RWBuffer)) != RendererAPI::DescriptorType::Undefined;

			return resource.Name;
		}
	}

	return "";
}
