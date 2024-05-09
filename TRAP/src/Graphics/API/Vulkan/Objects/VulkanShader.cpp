#include "TRAPPCH.h"
#include "VulkanShader.h"

#include "Graphics/API/RendererAPI.h"
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

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
	[[nodiscard]] constexpr bool IsDescriptorUAV(const TRAP::Graphics::API::ShaderReflection::ShaderResource& res)
	{
		using namespace TRAP::Graphics;

		return (res.Type & (RendererAPI::DescriptorType::RWTexture | RendererAPI::DescriptorType::RWBuffer)) != RendererAPI::DescriptorType::Undefined;
	}

	//-------------------------------------------------------------------------------------------------------------------//

#ifdef ENABLE_GRAPHICS_DEBUG
	void SetShaderStageName(const VkDevice& device, const std::string_view name, VkShaderModule stage)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

		if(name.empty())
			return;

		TRAP::Graphics::API::VkSetObjectName(device, std::bit_cast<u64>(stage), VK_OBJECT_TYPE_SHADER_MODULE, name);
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr const TRAP::Graphics::RendererAPI::BinaryShaderStageDesc* GetShaderStageDesc(const TRAP::Graphics::RendererAPI::ShaderStage stage,
	                                                                                                     const TRAP::Graphics::RendererAPI::BinaryShaderDesc& desc)
	{
		switch(stage)
		{
		case TRAP::Graphics::RendererAPI::ShaderStage::Vertex:
			return &desc.Vertex;
		case TRAP::Graphics::RendererAPI::ShaderStage::TessellationControl:
			return &desc.TessellationControl;
		case TRAP::Graphics::RendererAPI::ShaderStage::TessellationEvaluation:
			return &desc.TessellationEvaluation;
		case TRAP::Graphics::RendererAPI::ShaderStage::Geometry:
			return &desc.Geometry;
		case TRAP::Graphics::RendererAPI::ShaderStage::Fragment:
			return &desc.Fragment;
		case TRAP::Graphics::RendererAPI::ShaderStage::Compute:
			return &desc.Compute;

		default:
			TRAP_ASSERT(false, "VulkanShader::GetShaderStageDesc(): Unknown shader stage!");
			return nullptr;
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
	void UpdateDescriptorSet(const std::span<const TRAP::Scope<TRAP::Graphics::DescriptorSet>> descriptorSets, const u32 set, const TRAP::Graphics::RendererAPI::DescriptorData& descriptorData, const TRAP::Window& window)
#else
	void UpdateDescriptorSet(const std::span<const TRAP::Scope<TRAP::Graphics::DescriptorSet>> descriptorSets, const u32 set, const TRAP::Graphics::RendererAPI::DescriptorData& descriptorData)
#endif /*TRAP_HEADLESS_MODE*/
	{
		const TRAP::Graphics::RendererAPI::DescriptorData* const descDataPtr = &descriptorData;

		if(set == std::to_underlying(TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency::Static))
			descriptorSets[set]->Update(0, std::span<const TRAP::Graphics::RendererAPI::DescriptorData, 1>(descDataPtr, 1));
		else
		{
		#ifndef TRAP_HEADLESS_MODE
			const u32 imageIndex = TRAP::Graphics::RendererAPI::GetCurrentImageIndex(window);
		#else
			const u32 imageIndex = TRAP::Graphics::RendererAPI::GetCurrentImageIndex();
		#endif /*TRAP_HEADLESS_MODE*/
			descriptorSets[set]->Update(imageIndex, std::span<const TRAP::Graphics::RendererAPI::DescriptorData, 1>(descDataPtr, 1));
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr const TRAP::Graphics::API::ShaderReflection::ShaderResource* RetrieveDescriptor(const std::span<const TRAP::Graphics::API::ShaderReflection::ShaderResource> shaderResources,
		                                                                                                    const u32 set,
	                                                                                                        const u32 binding,
                                                                                                            const TRAP::Graphics::RendererAPI::DescriptorType type,
																	                                        const u64 size)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

		for(const auto& resource : shaderResources)
		{
			if((resource.Type & type) != TRAP::Graphics::RendererAPI::DescriptorType::Undefined &&
			resource.Set == set && resource.Reg == binding && resource.Size == size)
			{
				return &resource;
			}
		}

		return nullptr;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr std::pair<const TRAP::Graphics::API::ShaderReflection::ShaderResource*, bool> RetrieveBufferDescriptor(const std::span<const TRAP::Graphics::API::ShaderReflection::ShaderResource> shaderResources,
		                                                                                                                           const u32 set,
	                                                                                                                               const u32 binding,
                                                                                                                                   const TRAP::Graphics::RendererAPI::DescriptorType type,
																	                                                               const u64 size)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

		if(const auto* const exactMatch = RetrieveDescriptor(shaderResources, set, binding, type, size))
			return std::make_pair(exactMatch, false);
		if(const auto* const dynamicBufferMatch = RetrieveDescriptor(shaderResources, set, binding, type, 1))
			return std::make_pair(dynamicBufferMatch, true);

		const auto* const storageBufferMatch = RetrieveDescriptor(shaderResources, set, binding, type, 0);
		return std::make_pair(storageBufferMatch, false);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] std::pair<VkShaderModule, TRAP::Graphics::API::ShaderReflection::ShaderReflection> InitShaderStage(const TRAP::Graphics::API::VulkanDevice& device,
	                                                                                                                 [[maybe_unused]] const std::string_view name,
		                                                                                                             const TRAP::Graphics::RendererAPI::ShaderStage stage,
																						                             const TRAP::Graphics::RendererAPI::BinaryShaderStageDesc& stageDesc)
	{
		const TRAP::Graphics::API::ShaderReflection::ShaderReflection reflection = TRAP::Graphics::API::VkCreateShaderReflection(stageDesc.ByteCode, stage);

		const VkShaderModuleCreateInfo createInfo
		{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.codeSize = stageDesc.ByteCode.size() * sizeof(u32),
			.pCode = stageDesc.ByteCode.data()
		};
		VkShaderModule shaderModule = VK_NULL_HANDLE;
		VkCall(vkCreateShaderModule(device.GetVkDevice(), &createInfo, nullptr, &shaderModule));

	#ifdef ENABLE_GRAPHICS_DEBUG
		SetShaderStageName(device.GetVkDevice(), fmt::format("{}_{}", name, stage), shaderModule);
	#endif /*ENABLE_GRAPHICS_DEBUG*/

		return std::make_pair(shaderModule, reflection);
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, const std::filesystem::path& filepath,
												const RendererAPI::BinaryShaderDesc& desc,
                                                const std::vector<Macro>& userMacros, const bool valid)
	: Shader(std::move(name), valid, desc.Stages, userMacros, filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanShader(): Vulkan Device is nullptr");

	if(!m_valid)
		return;

	Init(desc);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, const RendererAPI::BinaryShaderDesc& desc,
                                                const std::vector<Macro>& userMacros, const bool valid)
	: Shader(std::move(name), valid, desc.Stages, userMacros)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanShader(): Vulkan Device is nullptr");

	if(!m_valid)
		return;

	Init(desc);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, const std::filesystem::path& filepath,
                                                const std::vector<Macro>& userMacros,
												const RendererAPI::ShaderStage stages)
	: Shader(std::move(name), false, stages, userMacros, filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanShader(): Vulkan Device is nullptr");

	//Intentionally not calling Init() as this is always an invalid shader
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::~VulkanShader()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::ShaderReflection::PipelineReflection> TRAP::Graphics::API::VulkanShader::GetReflection() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_reflection;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::Use(const Window& window)
#else
void TRAP::Graphics::API::VulkanShader::Use()
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifndef TRAP_HEADLESS_MODE
	dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->BindShader(*this, window);
#else
	dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->BindShader(*this);
#endif /*TRAP_HEADLESS_MODE*/

	if(!m_rootSignature)
		return;

	//Only do the following if the shader actually uses descriptors

	//Following some descriptor set allocation and reusing logic

#ifndef TRAP_HEADLESS_MODE
	const u32 currImageIndex = RendererAPI::GetCurrentImageIndex(window);
#else
	const u32 currImageIndex = RendererAPI::GetCurrentImageIndex();
#endif /*TRAP_HEADLESS_MODE*/

	if(m_lastImageIndex)
	{
		//Move last dirty descriptor sets to cleaned descriptor sets
		if(currImageIndex != *m_lastImageIndex && !m_dirtyDescriptorSets[*m_lastImageIndex].empty())
			m_cleanedDescriptorSets[*m_lastImageIndex] = std::move(m_dirtyDescriptorSets[*m_lastImageIndex]);

		//Set current descriptor sets as dirty
		for(auto& m_descriptorSet : m_descriptorSets)
			m_dirtyDescriptorSets[currImageIndex].emplace_back(std::move(m_descriptorSet));
	}

	//Get a clean descriptor set
	if(!m_lastImageIndex || m_cleanedDescriptorSets[currImageIndex].empty()) //Slow path
	{
		//Descriptor sets are now dirty, so we need new ones
		const Ref<VulkanRootSignature> root = std::dynamic_pointer_cast<VulkanRootSignature>(m_rootSignature);
		RendererAPI::DescriptorSetDesc setDesc{};
		setDesc.RootSignature = m_rootSignature;
		for(usize i = 0; i < m_descriptorSets.size(); ++i)
		{
			if(root->GetVkDescriptorSetLayouts()[i] == VK_NULL_HANDLE)
				continue;

			setDesc.MaxSets = (i == 0) ? 1 : RendererAPI::ImageCount;
			setDesc.Set = NumericCast<u32>(i);
			m_descriptorSets[i] = RendererAPI::GetDescriptorPool()->RetrieveDescriptorSet(setDesc);
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

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::UseTexture(const u32 set, const u32 binding,
                                                   const TRAP::Graphics::Texture& texture, const Window& window) const
#else
void TRAP::Graphics::API::VulkanShader::UseTexture(const u32 set, const u32 binding,
                                                   const TRAP::Graphics::Texture& texture) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifndef TRAP_HEADLESS_MODE
	UseTextures(set, binding, {&texture}, window);
#else
	UseTextures(set, binding, {&texture});
#endif /*TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::UseTextures(const u32 set, const u32 binding,
													const std::vector<const TRAP::Graphics::Texture*>& textures,
													const Window& window) const
#else
void TRAP::Graphics::API::VulkanShader::UseTextures(const u32 set, const u32 binding,
													const std::vector<const TRAP::Graphics::Texture*>& textures) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(!textures.empty(), "VulkanShader::UseTextures(): Textures are empty!");

	if(!m_valid)
		return;

	//OPTIMIZE Use index into root signature instead of name
	const auto* const descRes = RetrieveDescriptor(m_reflection->ShaderResources, set, binding, textures[0]->GetDescriptorTypes(), textures.size());
	if(descRes == nullptr)
	{
		//TP_ERROR(Log::RendererVulkanShaderPrefix, "Textures with invalid set and/or binding provided!");
		return;
	}

	TRAP::Graphics::RendererAPI::DescriptorData descData{};
	descData.Name = descRes->Name;
	descData.Resource = textures;
	descData.Count = NumericCast<u32>(textures.size());
	if(IsDescriptorUAV(*descRes) && (textures[0]->GetDescriptorTypes() & RendererAPI::DescriptorType::RWTexture) != RendererAPI::DescriptorType::Undefined)
		descData.Offset = RendererAPI::DescriptorData::TextureSlice{};

#ifndef TRAP_HEADLESS_MODE
	UpdateDescriptorSet(m_descriptorSets, set, descData, window);
#else
	UpdateDescriptorSet(m_descriptorSets, set, descData);
#endif /*TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::UseSampler(const u32 set, const u32 binding,
	                                               const TRAP::Graphics::Sampler& sampler, const Window& window) const
#else
void TRAP::Graphics::API::VulkanShader::UseSampler(const u32 set, const u32 binding,
	                                               const TRAP::Graphics::Sampler& sampler) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifndef TRAP_HEADLESS_MODE
	UseSamplers(set, binding, {&sampler}, window);
#else
	UseSamplers(set, binding, {&sampler});
#endif /*TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::UseSamplers(const u32 set, const u32 binding,
	                                                const std::vector<const TRAP::Graphics::Sampler*>& samplers,
													const Window& window) const
#else
void TRAP::Graphics::API::VulkanShader::UseSamplers(const u32 set, const u32 binding,
	                                                const std::vector<const TRAP::Graphics::Sampler*>& samplers) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(!samplers.empty(), "VulkanShader::UseSamplers(): Samplers are empty!");

	if(!m_valid)
		return;

	//OPTIMIZE Use index into root signature instead of name
	const auto* const descRes = RetrieveDescriptor(m_reflection->ShaderResources, set, binding, RendererAPI::DescriptorType::Sampler, samplers.size());
	if(descRes == nullptr)
	{
		//TP_ERROR(Log::RendererVulkanShaderPrefix, "Samplers with invalid set and/or binding provided!");
		return;
	}

	TRAP::Graphics::RendererAPI::DescriptorData descData{};
	descData.Name = descRes->Name;
	descData.Resource = samplers;
	descData.Count = NumericCast<u32>(samplers.size());

#ifndef TRAP_HEADLESS_MODE
	UpdateDescriptorSet(m_descriptorSets, set, descData, window);
#else
	UpdateDescriptorSet(m_descriptorSets, set, descData);
#endif /*TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::UseUBO(const u32 set, const u32 binding,
                                               const TRAP::Graphics::UniformBuffer& uniformBuffer,
											   const u64 size, const u64 offset, const Window& window) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(!m_valid)
		return;

	const u32 UBOIndex = (uniformBuffer.GetUpdateFrequency() == RendererAPI::DescriptorUpdateFrequency::Static) ?
		                          0 : RendererAPI::GetCurrentImageIndex(window);

	UseBuffer(set, binding, *uniformBuffer.GetUBOs()[UBOIndex], size != 0u ? size : uniformBuffer.GetSize(), offset, window);
}
#else
void TRAP::Graphics::API::VulkanShader::UseUBO(const u32 set, const u32 binding,
                                               const TRAP::Graphics::UniformBuffer& uniformBuffer,
											   const u64 size, const u64 offset) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(!m_valid)
		return;

	const u32 UBOIndex = (uniformBuffer.GetUpdateFrequency() == RendererAPI::DescriptorUpdateFrequency::Static) ?
		                          0 : RendererAPI::GetCurrentImageIndex();

	UseBuffer(set, binding, *uniformBuffer.GetUBOs()[UBOIndex], size != 0u ? size : uniformBuffer.GetSize(), offset);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::UseSSBO(const u32 set, const u32 binding,
                                                const TRAP::Graphics::StorageBuffer& storageBuffer,
											    const u64 size, const Window& window) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(!m_valid)
		return;

	const u32 SSBOIndex = (storageBuffer.GetUpdateFrequency() == RendererAPI::DescriptorUpdateFrequency::Static) ?
		                           0 : RendererAPI::GetCurrentImageIndex(window);

	UseBuffer(set, binding, *storageBuffer.GetSSBOs()[SSBOIndex], size != 0u ? size : storageBuffer.GetSize(), 0, window);
}
#else
void TRAP::Graphics::API::VulkanShader::UseSSBO(const u32 set, const u32 binding,
                                                const TRAP::Graphics::StorageBuffer& storageBuffer,
											    const u64 size) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(!m_valid)
		return;

	const u32 SSBOIndex = (storageBuffer.GetUpdateFrequency() == RendererAPI::DescriptorUpdateFrequency::Static) ?
		                           0 : RendererAPI::GetCurrentImageIndex();

	UseBuffer(set, binding, *storageBuffer.GetSSBOs()[SSBOIndex], size != 0u ? size : storageBuffer.GetSize(), 0);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Init(const RendererAPI::BinaryShaderDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanShaderPrefix, "Creating Shader: \"", m_name, "\"");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	std::vector<ShaderReflection::ShaderReflection> stageReflections{};

	for(usize i = 0; i < std::to_underlying(RendererAPI::ShaderStage::SHADER_STAGE_COUNT); i++)
	{
		const RendererAPI::ShaderStage stageMask = static_cast<RendererAPI::ShaderStage>(BIT(i));

		if(stageMask != (m_shaderStages & stageMask))
			continue;

		if(const auto* const stageDesc = GetShaderStageDesc(stageMask, desc))
		{
			auto [shaderModule, reflection] = InitShaderStage(*m_device, m_name, stageMask, *stageDesc);
			m_shaderModules.push_back(shaderModule);
			stageReflections.push_back(std::move(reflection));
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
	for(usize i = 0; i < m_descriptorSets.size(); ++i)
	{
		if(std::dynamic_pointer_cast<VulkanRootSignature>(m_rootSignature)->GetVkDescriptorSetLayouts()[i] == VK_NULL_HANDLE)
			continue;

		RendererAPI::DescriptorSetDesc setDesc{};
		setDesc.MaxSets = (i == 0) ? 1 : RendererAPI::ImageCount;
		setDesc.RootSignature = m_rootSignature;
		setDesc.Set = NumericCast<u32>(i);
		m_descriptorSets[i] = RendererAPI::GetDescriptorPool()->RetrieveDescriptorSet(setDesc);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanShaderPrefix, "Destroying Shader: \"", m_name, "\"");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if(!m_reflection || !m_valid)
	{
		m_shaderModules.clear();
		m_shaderStages = RendererAPI::ShaderStage::None;
		return;
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

	m_shaderModules.clear();
	m_shaderStages = RendererAPI::ShaderStage::None;
	m_reflection.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanShader::UseBuffer(const u32 set, const u32 binding,
												  const TRAP::Graphics::Buffer& buffer, u64 size, u64 offset,
												  const Window& window) const
#else
void TRAP::Graphics::API::VulkanShader::UseBuffer(const u32 set, const u32 binding,
												  const TRAP::Graphics::Buffer& buffer, u64 size, u64 offset) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	//OPTIMIZE Use index into root signature instead of name
	const auto [descRes, isDynamic] = RetrieveBufferDescriptor(m_reflection->ShaderResources, set, binding, buffer.GetDescriptors(), buffer.GetSize());

	if(descRes == nullptr) //Unable to find Buffer @ with set, binding & size
	{
		//TP_ERROR(Log::RendererVulkanShaderPrefix, "UniformBuffer with invalid set and/or binding & size provided!");
		return;
	}

	//Bind Buffer
	TRAP::Graphics::RendererAPI::DescriptorData descData{};
	descData.Name = descRes->Name;
	descData.Resource = std::vector<const TRAP::Graphics::Buffer*>{&buffer};
	if(isDynamic)
	{
		TRAP::Graphics::RendererAPI::DescriptorData::BufferOffset off{};
		off.Offsets.emplace_back(offset);
		off.Sizes.emplace_back(size);
		descData.Offset = std::move(off);
	}
	else
		descData.Offset = TRAP::Graphics::RendererAPI::DescriptorData::BufferOffset{};

#ifndef TRAP_HEADLESS_MODE
	UpdateDescriptorSet(m_descriptorSets, set, descData, window);
#else
	UpdateDescriptorSet(m_descriptorSets, set, descData);
#endif /*TRAP_HEADLESS_MODE*/
}
