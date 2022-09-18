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

TRAP::Graphics::API::VulkanRootSignature::VulkanRootSignature(const RendererAPI::RootSignatureDesc& desc)
	: m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice()),
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

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRootSignaturePrefix, "Creating RootSignature");
#endif

	static constexpr uint32_t maxLayoutCount = RendererAPI::MaxDescriptorSets;
	std::array<VulkanRenderer::UpdateFrequencyLayoutInfo, maxLayoutCount> layouts{};
	std::array<VkPushConstantRange,
	           static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> pushConstants{};
	uint32_t pushConstantCount = 0;
	std::vector<ShaderReflection::ShaderResource> shaderResources{};
	std::unordered_map<std::string, TRAP::Ref<VulkanSampler>> staticSamplerMap;

	for(std::size_t i = 0; i < desc.StaticSamplers.size(); ++i)
	{
		TRAP_ASSERT(desc.StaticSamplers[i]);
		staticSamplerMap.insert({ {desc.StaticSamplerNames[i],
		                           std::dynamic_pointer_cast<VulkanSampler>(desc.StaticSamplers[i])} });
	}

	RendererAPI::PipelineType pipelineType = RendererAPI::PipelineType::Undefined;
	RendererAPI::DescriptorIndexMap indexMap;

	//Collect all unique shader resources in the given shaders
	//Resources are parsed by name (two resources name "XYZ" in two shader will
	//be considered the same resource)
	for(auto sh : desc.Shaders)
	{
		const TRAP::Ref<ShaderReflection::PipelineReflection> reflection = dynamic_cast<VulkanShader*>
		(
			sh
		)->GetReflection();

		if (static_cast<uint32_t>(reflection->ShaderStages & RendererAPI::ShaderStage::Compute))
			pipelineType = RendererAPI::PipelineType::Compute;
		else if (static_cast<uint32_t>(reflection->ShaderStages & RendererAPI::ShaderStage::RayTracing))
			pipelineType = RendererAPI::PipelineType::RayTracing;
		else
			pipelineType = RendererAPI::PipelineType::Graphics;

		for(auto& res : reflection->ShaderResources)
		{
			const auto resNameIt = indexMap.find(res.Name);
			if(resNameIt == indexMap.end())
			{
				const auto resIt = std::find_if(shaderResources.begin(), shaderResources.end(),
				                                [res](const ShaderReflection::ShaderResource& a)
				{
					return (a.Type == res.Type) && (a.UsedStages == res.UsedStages) &&
					       (((a.Reg ^ res.Reg) | (a.Set ^ res.Set)) == 0);
				});
				if(resIt == shaderResources.end())
				{
					indexMap.insert({ res.Name, static_cast<uint32_t>(shaderResources.size()) });
					shaderResources.push_back(res);
				}
				else
				{
					TRAP_ASSERT(res.Type == resIt->Type);
					if(res.Type != resIt->Type)
					{
						TP_ERROR(Log::RendererVulkanRootSignaturePrefix, "Failed to create root signature");
						TP_ERROR(Log::RendererVulkanRootSignaturePrefix, "Shared shader resources ", res.Name,
						         " and ", resIt->Name, " have mismatching types (", static_cast<uint32_t>(res.Type),
							     ") and (", static_cast<uint32_t>(resIt->Type),
								 "). All shader resources sharing the same",
								 "register and space RootSignature must have the same type");
						return;
					}

					indexMap.insert({ res.Name, indexMap[resIt->Name] });
					resIt->UsedStages |= res.UsedStages;
				}
			}
			else
			{
				if(shaderResources[resNameIt->second].Reg != res.Reg)
				{
					TP_ERROR(Log::RendererVulkanRootSignaturePrefix, "Failed to create root signature");
					TP_ERROR(Log::RendererVulkanRootSignaturePrefix, "Shared shader resources ", res.Name,
					         " has mismatching binding. All shader resources shared by multiple shaders specified ",
							 "in RootSignature must have the same binding and set.");
					return;
				}
				if (shaderResources[resNameIt->second].Set != res.Set)
				{
					TP_ERROR(Log::RendererVulkanRootSignaturePrefix, "Failed to create root signature");
					TP_ERROR(Log::RendererVulkanRootSignaturePrefix, "Shared shader resources ", res.Name,
					         " has mismatching set. All shader resources shared by multiple shaders specified "
							 "in RootSignature must have the same binding and set.");
					return;
				}

				for(ShaderReflection::ShaderResource& r : shaderResources)
				{
					if(res.Name == resNameIt->first)
					{
						r.UsedStages |= res.UsedStages;
						break;
					}
				}
			}
		}
	}

	if(!shaderResources.empty())
		m_descriptors.resize(shaderResources.size());

	m_pipelineType = pipelineType;
	m_descriptorNameToIndexMap = indexMap;

	//Fill the descriptor array to be stored in the root signature
	for(std::size_t i = 0; i < shaderResources.size(); ++i)
	{
		RendererAPI::DescriptorInfo& descInfo = m_descriptors[i];
		const ShaderReflection::ShaderResource& res = shaderResources[i];
		uint32_t setIndex = res.Set;

		//Copy the binding information generated from the shader reflection into the descriptor
		descInfo.Reg = res.Reg;
		descInfo.Size = static_cast<uint32_t>(res.Size);
		descInfo.Type = res.Type;
		descInfo.Name = res.Name;
		descInfo.Dimension = res.Dim;

		//If descriptor is not a root constant create a new layout binding for this descriptor and add it to
		//the binding array
		if(descInfo.Type != RendererAPI::DescriptorType::RootConstant)
		{
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = res.Reg;
			binding.descriptorCount = descInfo.Size;
			binding.descriptorType = DescriptorTypeToVkDescriptorType(descInfo.Type);

			const std::string name = Utils::String::ToLower(res.Name);

			//If a user specified a uniform buffer to be used as a dynamic uniform buffer change its type to
			//VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
			//Also log a message for debugging purpose
			if(name.find("rootcbv") != std::string::npos || name.find("dynamic") != std::string::npos)
			{
				if(descInfo.Size == 1)
				{
					// TP_INFO("Descriptor (", descInfo.Name,
					//         "): User specified VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC");
					binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				}
				else
				{
					TRAP_ASSERT(false, "Descriptor (" + descInfo.Name +
					            "): Cannot use VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC for arrays");
					TP_WARN("Descriptor (", descInfo.Name,
					        "): Cannot use VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC for arrays");
				}
			}

			binding.stageFlags = ShaderStageToVkShaderStageFlags(res.UsedStages);

			//Store the Vulkan related info in the descriptor to avoid constantly calling mapping functions
			descInfo.VkType = binding.descriptorType;
			descInfo.VkStages = binding.stageFlags;
			descInfo.Set = setIndex;

			if (binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
				layouts[setIndex].DynamicDescriptors.emplace_back(&descInfo);

			//Find if the given descriptor is a static sampler
			const auto it = staticSamplerMap.find(descInfo.Name);
			const bool hasStaticSampler = it != staticSamplerMap.end();
			if (hasStaticSampler)
			{
				// TP_INFO("Descriptor (", descInfo.Name, "): User specified Static Sampler");
				const VkSampler sampler = it->second->GetVkSampler();
				binding.pImmutableSamplers = &sampler;
			}

			//Set the index to an invalid value so we can use this later for error checking
			//if user tries to update a static sampler
			//In case of Combined Image Samplers, skip invalidating the index
			//because we do not introduce new ways to update the descriptor in the interface
			if(hasStaticSampler && descInfo.Type != RendererAPI::DescriptorType::CombinedImageSampler)
				descInfo.IndexInParent = std::numeric_limits<uint32_t>::max();
			else
				layouts[setIndex].Descriptors.emplace_back(&descInfo);

			layouts[setIndex].Bindings.push_back(binding);
		}
		else //If descriptor is a push/root constant, add it to the push constant array
		{
			// TP_INFO("Descriptor (", descInfo.Name, "): User specified Push Constant");

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

	m_vkPushConstantCount = pushConstantCount;

	//Create descriptor layouts
	//Put most frequently changed params first
	for(uint32_t i = maxLayoutCount; i-- > 0U;)
	{
		VulkanRenderer::UpdateFrequencyLayoutInfo& layout = layouts[i];

		if(!layout.Bindings.empty())
		{
			//Sort table by type (CBV/SRV/UAV) by register
			std::sort(layout.Bindings.begin(), layout.Bindings.end(), [](const VkDescriptorSetLayoutBinding& lhs,
			                                                             const VkDescriptorSetLayoutBinding& rhs)
			{
				return lhs.binding > rhs.binding;
			});
			std::sort(layout.Bindings.begin(), layout.Bindings.end(), [](const VkDescriptorSetLayoutBinding& lhs,
			                                                             const VkDescriptorSetLayoutBinding& rhs)
			{
				return lhs.descriptorType > rhs.descriptorType;
			});
		}

		//TODO Validation Checks for
		//maxDescriptorSetSamplers
		//maxDescriptorSetUniformBuffers
		//maxDescriptorSetUniformBuffersDynamic
		//maxDescriptorSetStorageBuffers
		//maxDescriptorSetStorageBuffersDynamic
		//maxDescriptorSetSampledImages
		//maxDescriptorSetStorageImages
		//maxDescriptorSetInputAttachments

		bool createLayout = !layout.Bindings.empty();
		//Check if we need to create an empty layout in case there is an empty set between two used sets
		//Example: set = 0 is used, set = 2 is used.
		//In this case, set = 1 needs to exist even if it is empty
		if (!createLayout && i < maxLayoutCount - 1)
			createLayout = m_vkDescriptorSetLayouts[i + static_cast<uint64_t>(1)] != VK_NULL_HANDLE;

		if(createLayout)
		{
			const VkDescriptorSetLayoutCreateInfo layoutInfo = VulkanInits::DescriptorSetLayoutCreateInfo(layout.Bindings);

			VkCall(vkCreateDescriptorSetLayout(m_device->GetVkDevice(), &layoutInfo, nullptr,
			                                   &m_vkDescriptorSetLayouts[i]));
		}

		if (layout.Bindings.empty())
			continue;

		m_vkDescriptorCounts[i] = static_cast<uint16_t>(layout.Descriptors.size());

		//Loop through descriptor belonging to this update frequency and increment the cumulative descriptor count
		for(std::size_t descIndex = 0; descIndex < layout.Descriptors.size(); ++descIndex)
		{
			RendererAPI::DescriptorInfo* const descInfo = layout.Descriptors[descIndex];
			descInfo->IndexInParent = static_cast<uint32_t>(descIndex);
			descInfo->HandleIndex = m_vkCumulativeDescriptorsCounts[i];
			m_vkCumulativeDescriptorsCounts[i] += descInfo->Size;
		}

		m_vkDynamicDescriptorCounts[i] = static_cast<uint8_t>(layout.DynamicDescriptors.size());
		for(std::size_t descIndex = 0; descIndex < m_vkDynamicDescriptorCounts[i]; ++descIndex)
		{
			RendererAPI::DescriptorInfo* const descInfo = layout.DynamicDescriptors[descIndex];
			descInfo->RootDescriptorIndex = static_cast<uint32_t>(descIndex);
		}
	}

	//Pipeline layout
	std::array<VkDescriptorSetLayout, maxLayoutCount> descriptorSetLayouts{};
	uint32_t descriptorSetLayoutCount = 0;
	for(uint32_t i = 0; i < RendererAPI::MaxDescriptorSets; ++i)
	{
		if (m_vkDescriptorSetLayouts[i])
			descriptorSetLayouts[descriptorSetLayoutCount++] = m_vkDescriptorSetLayouts[i];
	}

	const VkPipelineLayoutCreateInfo addInfo = VulkanInits::PipelineLayoutCreateInfo(descriptorSetLayoutCount,
	                                                                                 descriptorSetLayouts.data(),
																			         m_vkPushConstantCount,
																			         pushConstants.data());
	VkCall(vkCreatePipelineLayout(m_device->GetVkDevice(), &addInfo, nullptr, &m_pipelineLayout));

	//Update templates
	for(uint32_t setIndex = 0; setIndex < RendererAPI::MaxDescriptorSets; ++setIndex)
	{
		if(m_vkDescriptorCounts[setIndex])
		{
			const VulkanRenderer::UpdateFrequencyLayoutInfo& layout = layouts[setIndex];
			std::vector<VkDescriptorUpdateTemplateEntry> entries(m_vkDescriptorCounts[setIndex]);
			uint32_t entryCount = 0;

			m_updateTemplateData[setIndex].resize(m_vkCumulativeDescriptorsCounts[setIndex]);

			//Fill the write descriptors with default values during initialization so the only thing we change
			//in CmdBindDescriptors are the VkBuffer / VKImageView objects
			for (const auto* const descInfo : layout.Descriptors)
			{
				const uint64_t offset = descInfo->HandleIndex * sizeof(VulkanRenderer::DescriptorUpdateData);

				//Raytracing descriptor dont support update template so we ignore them
				if(descInfo->Type == RendererAPI::DescriptorType::RayTracing)
				{
					m_vkRayTracingDescriptorCounts[setIndex] += static_cast<uint8_t>(descInfo->Size);
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
					{
						updateData[descInfo->HandleIndex + static_cast<std::size_t>(arr)].ImageInfo =
						{
							VulkanRenderer::s_NullDescriptors->DefaultSampler->GetVkSampler(),
						    VK_NULL_HANDLE,
							VK_IMAGE_LAYOUT_UNDEFINED
						};
					}
					break;

				case RendererAPI::DescriptorType::Texture:
					for (uint32_t arr = 0; arr < arrayCount; ++arr)
					{
						updateData[descInfo->HandleIndex + static_cast<std::size_t>(arr)].ImageInfo =
						{
							VK_NULL_HANDLE,
							dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(VulkanRenderer::s_NullDescriptors->DefaultTextureSRV[static_cast<uint32_t>(descInfo->Dimension)].get())->GetSRVVkImageView(),
							VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
						};
					}
					break;

				case RendererAPI::DescriptorType::RWTexture:
					for (uint32_t arr = 0; arr < arrayCount; ++arr)
					{
						updateData[descInfo->HandleIndex + static_cast<std::size_t>(arr)].ImageInfo =
						{
							VK_NULL_HANDLE,
							dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(VulkanRenderer::s_NullDescriptors->DefaultTextureUAV[static_cast<uint32_t>(descInfo->Dimension)].get())->GetUAVVkImageViews()[0],
							VK_IMAGE_LAYOUT_GENERAL
						};
					}
					break;

				case RendererAPI::DescriptorType::UniformBuffer:
				case RendererAPI::DescriptorType::Buffer:
				case RendererAPI::DescriptorType::BufferRaw:
					for(uint32_t arr = 0; arr < arrayCount; ++arr)
					{
						updateData[descInfo->HandleIndex + static_cast<std::size_t>(arr)].BufferInfo =
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
						updateData[descInfo->HandleIndex + static_cast<std::size_t>(arr)].BufferInfo =
						{
							VulkanRenderer::s_NullDescriptors->DefaultBufferUAV->GetVkBuffer(),
							VulkanRenderer::s_NullDescriptors->DefaultBufferUAV->GetOffset(),
							VK_WHOLE_SIZE
						};
					}
					break;

				case RendererAPI::DescriptorType::TexelBuffer:
					for (uint32_t arr = 0; arr < arrayCount; ++arr)
						updateData[descInfo->HandleIndex + static_cast<std::size_t>(arr)].BufferView = VulkanRenderer::s_NullDescriptors->DefaultBufferSRV->GetUniformTexelView();
					break;

				case RendererAPI::DescriptorType::RWTexelBuffer:
					for (uint32_t arr = 0; arr < arrayCount; ++arr)
						updateData[descInfo->HandleIndex + static_cast<std::size_t>(arr)].BufferView = VulkanRenderer::s_NullDescriptors->DefaultBufferUAV->GetStorageTexelView();
					break;

				default:
					break;
				}

				++entryCount;
			}

			const VkDescriptorUpdateTemplateCreateInfo createInfo = VulkanInits::DescriptorUpdateTemplateCreateInfo
			(
				m_vkDescriptorSetLayouts[setIndex], entryCount, entries.data(),
				VkPipelineBindPointTranslator[static_cast<uint32_t>(m_pipelineType)],
				m_pipelineLayout, setIndex
			);
			VkCall(vkCreateDescriptorUpdateTemplate(m_device->GetVkDevice(), &createInfo, nullptr,
			                                        &m_updateTemplates[setIndex]));

			entries.clear();
		}
		else if(m_vkDescriptorSetLayouts[setIndex] != VK_NULL_HANDLE)
		{
			//Consume empty descriptor sets from empty descriptor set pool
			m_vkEmptyDescriptorSets[setIndex] = dynamic_cast<TRAP::Graphics::API::VulkanDescriptorPool*>
			(
				RendererAPI::GetDescriptorPool().get()
			)->RetrieveVkDescriptorSet(m_vkDescriptorSetLayouts[setIndex]);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRootSignature::~VulkanRootSignature()
{
	TRAP_ASSERT(m_pipelineLayout);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRootSignaturePrefix, "Destroying RootSignature");
#endif

	for(uint32_t i = 0; i < RendererAPI::MaxDescriptorSets; ++i)
	{
		vkDestroyDescriptorSetLayout(m_device->GetVkDevice(), m_vkDescriptorSetLayouts[i], nullptr);
		if (m_updateTemplates[i] != VK_NULL_HANDLE)
			vkDestroyDescriptorUpdateTemplate(m_device->GetVkDevice(), m_updateTemplates[i], nullptr);

		m_updateTemplateData = {};
	}

	m_descriptorNameToIndexMap.clear();

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
const std::array<VkDescriptorSetLayout, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_vkDescriptorSetLayouts;
}

//-------------------------------------------------------------------------------------------------------------------//

auto TRAP::Graphics::API::VulkanRootSignature::GetVkCumulativeDescriptorCounts() const ->
	const std::array<uint32_t, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_vkCumulativeDescriptorsCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

auto TRAP::Graphics::API::VulkanRootSignature::GetVkDescriptorCounts() const ->
	const std::array<uint16_t, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_vkDescriptorCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

auto TRAP::Graphics::API::VulkanRootSignature::GetVkDynamicDescriptorCounts() const ->
	const std::array<uint8_t, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_vkDynamicDescriptorCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

auto TRAP::Graphics::API::VulkanRootSignature::GetVkRayTracingDescriptorCounts() const ->
	const std::array<uint8_t, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_vkRayTracingDescriptorCounts;
}

//-------------------------------------------------------------------------------------------------------------------//

auto TRAP::Graphics::API::VulkanRootSignature::GetUpdateTemplates() const ->
	const std::array<VkDescriptorUpdateTemplate, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_updateTemplates;
}

//-------------------------------------------------------------------------------------------------------------------//

auto TRAP::Graphics::API::VulkanRootSignature::GetVkEmptyDescriptorSets() const ->
	const std::array<VkDescriptorSet, TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_vkEmptyDescriptorSets;
}

//-------------------------------------------------------------------------------------------------------------------//

auto TRAP::Graphics::API::VulkanRootSignature::GetUpdateTemplateData() const ->
	const std::array<std::vector<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData>,
	                 TRAP::Graphics::RendererAPI::MaxDescriptorSets>&
{
	return m_updateTemplateData;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::DescriptorInfo* TRAP::Graphics::API::VulkanRootSignature::GetDescriptor(const char* const resName)
{
	const auto it = m_descriptorNameToIndexMap.find(resName);
	if (it != m_descriptorNameToIndexMap.end())
		return &m_descriptors[it->second];

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::RendererAPI::DescriptorInfo* TRAP::Graphics::API::VulkanRootSignature::GetDescriptor(const char* const resName) const
{
	const auto it = m_descriptorNameToIndexMap.find(resName);
	if (it != m_descriptorNameToIndexMap.end())
		return &m_descriptors[it->second];

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanRootSignature::GetDescriptorCount() const
{
	return static_cast<uint32_t>(m_descriptors.size());
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Graphics::RendererAPI::DescriptorInfo>& TRAP::Graphics::API::VulkanRootSignature::GetDescriptors() const
{
	return m_descriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::API::VulkanRenderer::DescriptorIndexMap& TRAP::Graphics::API::VulkanRootSignature::GetDescriptorNameToIndexMap() const
{
	return m_descriptorNameToIndexMap;
}