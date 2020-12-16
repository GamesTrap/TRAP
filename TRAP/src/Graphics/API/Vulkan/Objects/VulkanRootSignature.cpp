#include "TRAPPCH.h"
#include "VulkanRootSignature.h"

#include "Utils/String/String.h"
#include "VulkanBuffer.h"
#include "VulkanSampler.h"
#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorPool.h"
#include "VulkanShader.h"
#include "Graphics/API/ShaderReflection.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "VulkanTexture.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanRootSignature::VulkanRootSignature(TRAP::Ref<VulkanDevice> device, const TRAP::Ref<VulkanDescriptorPool>& descriptorPool, const RendererAPI::RootSignatureDesc& desc)
	: m_device(std::move(device)),
	  m_descriptorCount(),
	  m_pipelineType(),
	  m_vkDescriptorSetLayouts(),
	  m_vkCumulativeDescriptorsCounts(),
	  m_vkDescriptorCounts(),
	  m_vkDynamicDescriptorCounts(),
	  m_vkRayTracingDescriptorCounts(),
	  m_pipelineLayout(),
	  m_updateTemplates(),
	  m_vkEmptyDescriptorSets(),
	  m_vkPushConstantCount()
{
	TRAP_ASSERT(m_device, "device is nullptr");
	
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRootSignaturePrefix, "Creating RootSignature");
#endif

	static constexpr uint32_t maxLayoutCount = static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT);
	std::array<UpdateFrequencyLayoutInfo, maxLayoutCount> layouts{};
	std::array<VkPushConstantRange, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> pushConstants{};
	uint32_t pushConstantCount = 0;
	std::vector<ShaderReflection::ShaderResource> shaderResources{};
	std::unordered_map<std::string, TRAP::Ref<VulkanSampler>> staticSamplerMap;

	for(uint32_t i = 0; i < desc.StaticSamplers.size(); ++i)
	{
		TRAP_ASSERT(desc.StaticSamplers[i]);
		staticSamplerMap.insert({ {desc.StaticSamplerNames[i], desc.StaticSamplers[i]} });
	}

	RendererAPI::PipelineType pipelineType = RendererAPI::PipelineType::Undefined;
	VulkanRenderer::DescriptorIndexMap indexMap;

	//Collect all unique shader resources in the given shaders
	//Resources are parsed by name (two resources name "XYZ" in two shader will
	//be considered the same resource)
	for(uint32_t sh = 0; sh < desc.Shaders.size(); ++sh)
	{
		TRAP::Ref<ShaderReflection::PipelineReflection> reflection = desc.Shaders[sh]->GetReflection();

		if (static_cast<uint32_t>(reflection->ShaderStages & RendererAPI::ShaderStage::Compute))
			pipelineType = RendererAPI::PipelineType::Compute;
		else if (static_cast<uint32_t>(reflection->ShaderStages & RendererAPI::ShaderStage::RayTracing))
			pipelineType = RendererAPI::PipelineType::RayTracing;
		else
			pipelineType = RendererAPI::PipelineType::Graphics;

		for(uint32_t i = 0; i < reflection->ShaderResources.size(); ++i)
		{
			ShaderReflection::ShaderResource& res = reflection->ShaderResources[i];
			uint32_t setIndex = res.Set;

			if (res.Type == RendererAPI::DescriptorType::RootConstant)
				setIndex = 0;

			std::unordered_map<std::string, uint32_t>::iterator it =
				indexMap.Map.find(res.Name);
			if(it == indexMap.Map.end())
			{
				auto it = std::find_if(shaderResources.begin(), shaderResources.end(),
				                       [res](const ShaderReflection::ShaderResource& a)
				                       {
					                       return (a.Type == res.Type) && (a.UsedStages == res.UsedStages) && (((a.Reg ^ res.Reg) | (a.Set ^ res.Set)) == 0);
				                       });
				if(it == shaderResources.end())
				{
					indexMap.Map.insert({ res.Name, static_cast<uint32_t>(shaderResources.size()) });
					shaderResources.push_back(res);
				}
				else
				{
					TRAP_ASSERT(res.Type == it->Type);
					if(res.Type != it->Type)
					{
						TP_ERROR(Log::RendererVulkanRootSignaturePrefix, "Failed to create root signature");
						TP_ERROR(Log::RendererVulkanRootSignaturePrefix, "Shared shader resources ", res.Name, " and ", it->Name, " have mismatching types (", static_cast<uint32_t>(res.Type),
							") and (", static_cast<uint32_t>(it->Type), "). All shader resources sharing the same",
							"register and space addRootSignature must have the same type");
						return;
					}

					indexMap.Map.insert({ res.Name, indexMap.Map[it->Name] });
					it->UsedStages |= res.UsedStages;
				}
			}
			else
			{
				if(shaderResources[it->second].Reg != res.Reg)
				{
					TP_ERROR(Log::RendererVulkanRootSignaturePrefix, "Failed to create root signature");
					TP_ERROR(Log::RendererVulkanRootSignaturePrefix, "Shared shader resources ", res.Name, " has mismatching binding.", 
						" All shader resources shared by multiple shaders specified in addRootSignature ",
						"must have the same binding and set.");
					return;
				}
				if (shaderResources[it->second].Set != res.Set)
				{
					TP_ERROR(Log::RendererVulkanRootSignaturePrefix, "Failed to create root signature");
					TP_ERROR(Log::RendererVulkanRootSignaturePrefix, "Shared shader resources ", res.Name, " has mismatching set.",
						" All shader resources shared by multiple shaders specified in addRootSignature ",
						"must have the same binding and set.");
					return;
				}

				for(ShaderReflection::ShaderResource& r : shaderResources)
				{
					if(res.Name == it->first)
					{
						r.UsedStages |= res.UsedStages;
						break;
					}
				}
			}
		}
	}

	m_descriptorNameToIndexMap = TRAP::MakeScope<VulkanRenderer::DescriptorIndexMap>();

	if(!shaderResources.empty())
		m_descriptorCount = static_cast<uint32_t>(shaderResources.size());

	m_pipelineType = pipelineType;
	m_descriptorNameToIndexMap->Map = indexMap.Map;

	//Fill the descriptor array to be stored in the root signature
	for(uint32_t i = 0; i < static_cast<uint32_t>(shaderResources.size()); ++i)
	{
		RendererAPI::DescriptorInfo& descInfo = m_descriptors[i];
		ShaderReflection::ShaderResource& res = shaderResources[i];
		uint32_t setIndex = res.Set;
		RendererAPI::DescriptorUpdateFrequency updateFreq = static_cast<RendererAPI::DescriptorUpdateFrequency>(setIndex);

		//Copy the binding information generated from the shader reflection into the descriptor
		descInfo.Reg = res.Reg;
		descInfo.Size = res.Size;
		descInfo.Type = res.Type;
		descInfo.Name = res.Name;
		descInfo.Dimension = res.Dim;

		//If descriptor is not a root constant create a new layout binding for this descriptor and add it to the binding array
		if(descInfo.Type != RendererAPI::DescriptorType::RootConstant)
		{
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = res.Reg;
			binding.descriptorCount = descInfo.Size;
			binding.descriptorType = DescriptorTypeToVkDescriptorType(descInfo.Type);

			std::string name = res.Name;
			name = Utils::String::ToLower(name);

			//If a user specified a uniform buffer to be used as a dynamic uniform buffer change its type to
			//VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
			//Also log a message for debugging purpose
			if(name.find("rootcbv") != std::string::npos)
			{
				if(descInfo.Size == 1)
				{
					TP_INFO("Descriptor (", descInfo.Name, "): User specified VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC");
					binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				}
				else
				TP_WARN("Descriptor (", descInfo.Name, "): Cannot use VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC for arrays");
			}

			binding.stageFlags = ShaderStageToVkShaderStageFlags(res.UsedStages);

			//Store the Vulkan related info in the descriptor to avoid constantly calling mapping functions
			descInfo.VkType = binding.descriptorType;
			descInfo.VkStages = binding.stageFlags;
			descInfo.UpdateFrequency = updateFreq;

			if (binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
				layouts[setIndex].DynamicDescriptors.emplace_back(&descInfo);

			//Find if the given descriptor is a static sampler
			auto it = staticSamplerMap.find(descInfo.Name);
			if (it != staticSamplerMap.end())
			{
				TP_INFO("Descriptor (", descInfo.Name, "): User specified Static Sampler");

				//Set the index to an invalid value to we can use this later for error checking
				//if user tries to update a static sampler
				descInfo.IndexInParent = -1;
				binding.pImmutableSamplers = &it->second->GetVkSampler();
			}
			else
				layouts[setIndex].Descriptors.emplace_back(&descInfo);

			layouts[setIndex].Bindings.push_back(binding);
		}
		else //If descriptor array to be stored in the root signature
		{
			TP_INFO("Descriptor (", descInfo.Name, "): User specified Push Constant");

			descInfo.VkStages = ShaderStageToVkShaderStageFlags(res.UsedStages);
			setIndex = 0;
			descInfo.IndexInParent = pushConstantCount++;

			pushConstants[descInfo.IndexInParent] = {};
			pushConstants[descInfo.IndexInParent].offset = 0;
			pushConstants[descInfo.IndexInParent].size = descInfo.Size;
			pushConstants[descInfo.IndexInParent].stageFlags = descInfo.VkStages;
		}

		layouts[setIndex].DescriptorIndexMap[&descInfo] = i;
	}

	//Create descriptor layouts
	//Put most frequently changed params first
	for(uint32_t i = maxLayoutCount; i-- > 0U;)
	{
		UpdateFrequencyLayoutInfo& layout = layouts[i];

		if(!layouts[i].Bindings.empty())
		{
			//Sort table by type (CBV/SRV/UAV) by register
			std::sort(layout.Bindings.begin(), layout.Bindings.end(),
				[](const VkDescriptorSetLayoutBinding& lhs, const VkDescriptorSetLayoutBinding& rhs)
				{
					return lhs.binding > rhs.binding;
				});
			std::sort(layout.Bindings.begin(), layout.Bindings.end(),
				[](const VkDescriptorSetLayoutBinding& lhs, const VkDescriptorSetLayoutBinding& rhs)
				{
					return lhs.descriptorType > rhs.descriptorType;
				});
		}

		bool createLayout = !layout.Bindings.empty();
		//Check if we need to create an empty layout in case there is an empty set between two used sets
		//Example: set = 0 is used, set = 2 is used.
		//In this case, set = 1 needs to exist even if it is empty
		if (!createLayout && i < maxLayoutCount - 1)
			createLayout = m_vkDescriptorSetLayouts[i + 1] != VK_NULL_HANDLE;

		if(createLayout)
		{
			VkDescriptorSetLayoutCreateInfo layoutInfo = VulkanInits::DescriptorSetLayoutCreateInfo(layout.Bindings);

			VkCall(vkCreateDescriptorSetLayout(m_device->GetVkDevice(), &layoutInfo, nullptr, &m_vkDescriptorSetLayouts[i]));
		}

		if (layouts[i].Bindings.empty())
			continue;

		m_vkDescriptorCounts[i] = static_cast<uint16_t>(layout.Descriptors.size());

		//Loop through descriptor belonging to this update frequency and increment the cumulative descriptor count
		for(uint32_t descIndex = 0; descIndex < static_cast<uint32_t>(layout.Descriptors.size()); ++descIndex)
		{
			RendererAPI::DescriptorInfo* descInfo = layout.Descriptors[descIndex];
			descInfo->IndexInParent = descIndex;
			descInfo->HandleIndex = m_vkCumulativeDescriptorsCounts[i];
			m_vkCumulativeDescriptorsCounts[i] += descInfo->Size;
		}

		m_vkDynamicDescriptorCounts[i] = static_cast<uint8_t>(layout.DynamicDescriptors.size());
		for(uint32_t descIndex = 0; descIndex < m_vkDynamicDescriptorCounts[i]; ++descIndex)
		{
			RendererAPI::DescriptorInfo* descInfo = layout.DynamicDescriptors[descIndex];
			descInfo->RootDescriptorIndex = descIndex;
		}
	}

	//Pipeline layout
	std::array<VkDescriptorSetLayout, maxLayoutCount> descriptorSetLayouts{};
	uint32_t descriptorSetLayoutCount = 0;
	for(uint32_t i = 0; i < static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT); ++i)
	{
		if (m_vkDescriptorSetLayouts[i])
			descriptorSetLayouts[descriptorSetLayoutCount++] = m_vkDescriptorSetLayouts[i];
	}

	VkPipelineLayoutCreateInfo addInfo = VulkanInits::PipelineLayoutCreateInfo(descriptorSetLayoutCount, descriptorSetLayouts.data(), m_vkPushConstantCount, pushConstants.data());
	VkCall(vkCreatePipelineLayout(m_device->GetVkDevice(), &addInfo, nullptr, &m_pipelineLayout));

	//Update templates
	for(uint32_t setIndex = 0; setIndex < static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT); ++setIndex)
	{
		if(m_vkDescriptorCounts[setIndex])
		{
			const UpdateFrequencyLayoutInfo& layout = layouts[setIndex];
			std::vector<VkDescriptorUpdateTemplateEntry> entries(m_vkDescriptorCounts[setIndex]);
			uint32_t entryCount = 0;

			m_updateTemplateData[setIndex].resize(m_vkCumulativeDescriptorsCounts[setIndex]);

			//Fill the write descriptors with default values during initialization so the only thing we change
			//in CmdBindDescriptors are the VkBuffer / VKImageView objects
			for(uint32_t i = 0; i < static_cast<uint32_t>(layout.Descriptors.size()); ++i)
			{
				const RendererAPI::DescriptorInfo* descInfo = layout.Descriptors[i];
				const uint64_t offset = descInfo->HandleIndex * sizeof(VulkanRenderer::DescriptorUpdateData);

				//Raytracing descriptor dont support update template so we ignore them
				if(descInfo->Type == RendererAPI::DescriptorType::RayTracing)
				{
					m_vkRayTracingDescriptorCounts[setIndex] += descInfo->Size;
					continue;
				}

				entries[entryCount].descriptorCount = descInfo->Size;
				entries[entryCount].descriptorType = descInfo->VkType;
				entries[entryCount].dstArrayElement = 0;
				entries[entryCount].dstBinding = descInfo->Reg;
				entries[entryCount].offset = offset;
				entries[entryCount].stride = sizeof(VulkanRenderer::DescriptorUpdateData);

				std::vector<VulkanRenderer::DescriptorUpdateData>& updateData = m_updateTemplateData[setIndex];

				const RendererAPI::DescriptorType type = descInfo->Type;
				const uint32_t arrayCount = descInfo->Size;

				switch(type)
				{
				case RendererAPI::DescriptorType::Sampler:
					for (uint32_t arr = 0; arr < arrayCount; ++arr)
						updateData[descInfo->HandleIndex + arr].ImageInfo = {VulkanRenderer::s_NullDescriptors->DefaultSampler->GetVkSampler(), VK_NULL_HANDLE};
					break;

				case RendererAPI::DescriptorType::Texture:
					for (uint32_t arr = 0; arr < arrayCount; ++arr)
						updateData[descInfo->HandleIndex + arr].ImageInfo = { VK_NULL_HANDLE, VulkanRenderer::s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(descInfo->Dimension)]->GetSRVVkImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
					break;

				case RendererAPI::DescriptorType::RWTexture:
					for (uint32_t arr = 0; arr < arrayCount; ++arr)
						updateData[descInfo->HandleIndex + arr].ImageInfo = {VK_NULL_HANDLE, VulkanRenderer::s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(descInfo->Dimension)]->GetUAVVkImageViews()[0], VK_IMAGE_LAYOUT_GENERAL};
					break;

				case RendererAPI::DescriptorType::UniformBuffer:
				case RendererAPI::DescriptorType::Buffer:
				case RendererAPI::DescriptorType::BufferRaw:
					for(uint32_t arr = 0; arr < arrayCount; ++arr)
					{
						updateData[descInfo->HandleIndex + arr].BufferInfo =
						{
							VulkanRenderer::s_NullDescriptors->DefaultBufferSRV->GetVkBuffer(),
							VulkanRenderer::s_NullDescriptors->DefaultBufferSRV->GetOffset(),
							VK_WHOLE_SIZE
						};
					}
					break;

				case RendererAPI::DescriptorType::RWBuffer:
				case RendererAPI::DescriptorType::RWBufferRaw:
					for(uint32_t arr = 0; arr < arrayCount; ++arr)
					{
						updateData[descInfo->HandleIndex + arr].BufferInfo =
						{
							VulkanRenderer::s_NullDescriptors->DefaultBufferUAV->GetVkBuffer(),
							VulkanRenderer::s_NullDescriptors->DefaultBufferUAV->GetOffset(),
							VK_WHOLE_SIZE
						};
					}
					break;

				case RendererAPI::DescriptorType::TexelBuffer:
					for (uint32_t arr = 0; arr < arrayCount; ++arr)
						updateData[descInfo->HandleIndex + arr].BufferView = VulkanRenderer::s_NullDescriptors->DefaultBufferSRV->GetUniformTexelView();
					break;

				case RendererAPI::DescriptorType::RWTexelBuffer:
					for (uint32_t arr = 0; arr < arrayCount; ++arr)
						updateData[descInfo->HandleIndex + arr].BufferView = VulkanRenderer::s_NullDescriptors->DefaultBufferUAV->GetStorageTexelView();
					break;
					
				default:
					break;
				}
			}

			++entryCount;

			VkDescriptorUpdateTemplateCreateInfo createInfo =
					VulkanInits::DescriptorUpdateTemplateCreateInfo(m_vkDescriptorSetLayouts[setIndex],
					                                                entryCount,
					                                                entries.data(),
					                                                VkPipelineBindPointTranslator[static_cast<uint32_t>(m_pipelineType)],
					                                                m_pipelineLayout,
					                                                setIndex);
			VkCall(vkCreateDescriptorUpdateTemplate(m_device->GetVkDevice(), &createInfo, nullptr, &m_updateTemplates[setIndex]));
			
			entries.clear();
		}
		else if(m_vkDescriptorSetLayouts[setIndex] != VK_NULL_HANDLE)
		{
			//Consume empty descriptor sets from empty descriptor set pool
			m_vkEmptyDescriptorSets[setIndex] = descriptorPool->RetrieveVkDescriptorSet(m_vkDescriptorSetLayouts[setIndex]);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRootSignature::~VulkanRootSignature()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRootSignaturePrefix, "Destroying RootSignature");
#endif
	
	for(uint32_t i = 0; i < static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT); ++i)
	{
		vkDestroyDescriptorSetLayout(m_device->GetVkDevice(), m_vkDescriptorSetLayouts[i], nullptr);
		if (m_updateTemplates[i] != VK_NULL_HANDLE)
			vkDestroyDescriptorUpdateTemplate(m_device->GetVkDevice(), m_updateTemplates[i], nullptr);

		m_updateTemplateData = {};
	}

	m_descriptorNameToIndexMap->Map.clear();

	if (m_pipelineLayout)
		vkDestroyPipelineLayout(m_device->GetVkDevice(), m_pipelineLayout, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::PipelineType TRAP::Graphics::API::VulkanRootSignature::GetPipelineType() const
{
	return m_pipelineType;
}

//-------------------------------------------------------------------------------------------------------------------//

VkPipelineLayout TRAP::Graphics::API::VulkanRootSignature::GetVkPipelineLayout() const
{
	return m_pipelineLayout;
}

//-------------------------------------------------------------------------------------------------------------------//

auto TRAP::Graphics::API::VulkanRootSignature::GetVkDescriptorSetLayouts() const ->
const std::array<VkDescriptorSetLayout, static_cast<uint32_t>(TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>&
{
	return m_vkDescriptorSetLayouts;
}

//-------------------------------------------------------------------------------------------------------------------//

auto TRAP::Graphics::API::VulkanRootSignature::GetVkCumulativeDescriptorCounts() const ->
	const std::array<uint32_t, static_cast<uint32_t>(TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>&
{
	return m_vkCumulativeDescriptorsCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

auto TRAP::Graphics::API::VulkanRootSignature::GetVkDescriptorCounts() const ->
	const std::array<uint16_t, static_cast<uint32_t>(TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>&
{
	return m_vkDescriptorCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

auto TRAP::Graphics::API::VulkanRootSignature::GetVkDynamicDescriptorCounts() const ->
	const std::array<uint8_t, static_cast<uint32_t>(TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>&
{
	return m_vkDynamicDescriptorCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

auto TRAP::Graphics::API::VulkanRootSignature::GetVkRayTracingDescriptorCounts() const ->
	const std::array<uint8_t, static_cast<uint32_t>(TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>&
{
	return m_vkRayTracingDescriptorCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

auto TRAP::Graphics::API::VulkanRootSignature::GetUpdateTemplates() const ->
	const std::array<VkDescriptorUpdateTemplate, static_cast<uint32_t>(TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>&
{
	return m_updateTemplates;
}

//-------------------------------------------------------------------------------------------------------------------//

auto TRAP::Graphics::API::VulkanRootSignature::GetVkEmptyDescriptorSets() const ->
	const std::array<VkDescriptorSet, static_cast<uint32_t>(TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>&
{
	return m_vkEmptyDescriptorSets;
}

//-------------------------------------------------------------------------------------------------------------------//

auto TRAP::Graphics::API::VulkanRootSignature::GetUpdateTemplateData() const ->
	const std::array<std::vector<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData>, static_cast<uint32_t>(TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT)>&
{
	return m_updateTemplateData;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::DescriptorInfo* TRAP::Graphics::API::VulkanRootSignature::GetDescriptor(const char* resName)
{
	const std::unordered_map<std::string, uint32_t>::const_iterator it = m_descriptorNameToIndexMap->Map.find(resName);
	if (it != m_descriptorNameToIndexMap->Map.end())
		return &m_descriptors[it->second];

	TP_ERROR(Log::RendererVulkanRootSignaturePrefix, "Invalid descriptor param ", resName);
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanRootSignature::GetDescriptorCount() const
{
	return m_descriptorCount;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Graphics::RendererAPI::DescriptorInfo>& TRAP::Graphics::API::VulkanRootSignature::GetDescriptors() const
{
	return m_descriptors;
}