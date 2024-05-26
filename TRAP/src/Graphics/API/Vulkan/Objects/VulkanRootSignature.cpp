#include "TRAPPCH.h"
#include "VulkanRootSignature.h"

#include "Utils/String/String.h"
#include "VulkanBuffer.h"
#include "VulkanSampler.h"
#include "VulkanDescriptorPool.h"
#include "VulkanShader.h"
#include "Graphics/API/ShaderReflection.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "VulkanTexture.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

namespace
{
	[[nodiscard]] TRAP::Utils::UnorderedStringMap<TRAP::Ref<TRAP::Graphics::API::VulkanSampler>> GetStaticSamplerMap(const TRAP::Graphics::RendererAPI::RootSignatureDesc& desc)
	{
		TRAP::Utils::UnorderedStringMap<TRAP::Ref<TRAP::Graphics::API::VulkanSampler>> staticSamplerMap{};

		for(usize i = 0; i < desc.StaticSamplers.size(); ++i)
		{
			TRAP_ASSERT(desc.StaticSamplers[i], "VulkanRootSignature(): Static Sampler is nullptr");
			staticSamplerMap.try_emplace(desc.StaticSamplerNames[i], std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanSampler>(desc.StaticSamplers[i]));
		}

		return staticSamplerMap;
	}

	[[nodiscard]] TRAP::Graphics::RendererAPI::PipelineType GetReflectionPipelineType(const TRAP::Graphics::Shader& shader)
	{
		using RendererAPI = TRAP::Graphics::RendererAPI;

		const TRAP::Graphics::API::VulkanShader* const vkShader = dynamic_cast<const TRAP::Graphics::API::VulkanShader* const>(&shader);
		if(vkShader == nullptr)
			return RendererAPI::PipelineType::Undefined;
		const auto reflection = vkShader->GetReflection();
		if(reflection == nullptr)
			return RendererAPI::PipelineType::Undefined;

		if ((reflection->ShaderStages & RendererAPI::ShaderStage::Compute) != RendererAPI::ShaderStage::None)
			return RendererAPI::PipelineType::Compute;
		if ((reflection->ShaderStages & RendererAPI::ShaderStage::RayTracing) != RendererAPI::ShaderStage::None)
			return RendererAPI::PipelineType::RayTracing;
		if ((reflection->ShaderStages & RendererAPI::ShaderStage::AllGraphics) != RendererAPI::ShaderStage::None)
			return RendererAPI::PipelineType::Graphics;

		return RendererAPI::PipelineType::Undefined;
	}

	[[nodiscard]] bool CollectUniqueShaderResourcesFromReflection(const TRAP::Graphics::API::ShaderReflection::PipelineReflection& reflection,
	                                                              std::vector<TRAP::Graphics::API::ShaderReflection::ShaderResource>& outShaderResources,
													              TRAP::Graphics::RendererAPI::DescriptorIndexMap& outIndexMap)
	{
		for(const auto& res : reflection.ShaderResources)
		{
			const auto resNameIt = outIndexMap.find(res.Name);
			if(resNameIt == outIndexMap.end())
			{
				const auto resIt = std::ranges::find_if(outShaderResources,
												[&res](const TRAP::Graphics::API::ShaderReflection::ShaderResource& a)
				{
					return (a.Type == res.Type) && (a.UsedStages == res.UsedStages) &&
							(((a.Reg ^ res.Reg) | (a.Set ^ res.Set)) == 0);
				});
				if(resIt == outShaderResources.end())
				{
					outIndexMap.try_emplace(res.Name, NumericCast<u32>(outShaderResources.size()));
					outShaderResources.push_back(res);
				}
				else
				{
					TRAP_ASSERT(res.Type == resIt->Type, "VulkanRootSignature(): Resource Type mismatch");
					if(res.Type != resIt->Type)
					{
						TP_ERROR(TRAP::Log::RendererVulkanRootSignaturePrefix, "Failed to create root signature");
						TP_ERROR(TRAP::Log::RendererVulkanRootSignaturePrefix, "Shared shader resources ", res.Name,
								" and ", resIt->Name, " have mismatching types (", std::to_underlying(res.Type),
								") and (", std::to_underlying(resIt->Type),
								"). All shader resources sharing the same",
								"register and space RootSignature must have the same type");
						return false;
					}

					outIndexMap.try_emplace(res.Name, outIndexMap[resIt->Name]);
					resIt->UsedStages |= res.UsedStages;
				}
			}
			else
			{
				if(outShaderResources[resNameIt->second].Reg != res.Reg || outShaderResources[resNameIt->second].Set != res.Set)
				{
					TP_ERROR(TRAP::Log::RendererVulkanRootSignaturePrefix, "Failed to create root signature");
					TP_ERROR(TRAP::Log::RendererVulkanRootSignaturePrefix, "Shared shader resources ", res.Name,
							" has mismatching binding or set. All shader resources shared by multiple shaders specified ",
							"in RootSignature must have the same binding and set.");
					return false;
				}

				const auto foundRes = std::ranges::find_if(outShaderResources, [resNameIt](const auto& r){return r.Name == resNameIt->first;});
				if(foundRes != outShaderResources.end())
					foundRes->UsedStages |= res.UsedStages;
			}
		}

		return true;
	}

	[[nodiscard]] std::pair<std::vector<TRAP::Graphics::API::ShaderReflection::ShaderResource>, TRAP::Graphics::RendererAPI::DescriptorIndexMap>
		CollectUniqueShaderResources(const TRAP::Graphics::RendererAPI::RootSignatureDesc& desc)
	{
		std::pair<std::vector<TRAP::Graphics::API::ShaderReflection::ShaderResource>, TRAP::Graphics::RendererAPI::DescriptorIndexMap> uniqueResources{};
		auto& [shaderResources, indexMap] = uniqueResources;

		//Collect all unique shader resources in the given shaders
		//Resources are parsed by name (two resources name "XYZ" in two shaders will
		//be considered the same resource)
		for(const TRAP::Graphics::Shader* const sh : desc.Shaders)
		{
			const TRAP::Graphics::API::VulkanShader* const vkShader = dynamic_cast<const TRAP::Graphics::API::VulkanShader* const>(sh);
			if(vkShader == nullptr)
				continue;

			const TRAP::Ref<TRAP::Graphics::API::ShaderReflection::PipelineReflection> reflection = vkShader->GetReflection();
			if(reflection == nullptr)
				continue;

			if(!CollectUniqueShaderResourcesFromReflection(*reflection, shaderResources, indexMap))
				return {};
		}

		return uniqueResources;
	}

	[[nodiscard]] VkDescriptorSetLayoutBinding BuildVkDescriptorSetLayoutBinding(const TRAP::Graphics::API::ShaderReflection::ShaderResource& shaderResource,
	                                                                             const TRAP::Graphics::RendererAPI::DescriptorInfo& descriptorInfo,
																				 const TRAP::Utils::UnorderedStringMap<TRAP::Ref<TRAP::Graphics::API::VulkanSampler>>& staticSamplerMap)
	{
		VkDescriptorSetLayoutBinding binding{};
		binding.binding = shaderResource.Reg;
		binding.descriptorCount = descriptorInfo.Size;
		binding.descriptorType = TRAP::Graphics::API::DescriptorTypeToVkDescriptorType(descriptorInfo.Type);

		//If a user specified a uniform buffer to be used as a dynamic uniform buffer change its type to
		//VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
		const std::string name = TRAP::Utils::String::ToLower(shaderResource.Name);
		if(TRAP::Utils::String::Contains(name, "rootcbv") || TRAP::Utils::String::Contains(name, "dynamic"))
		{
			if(descriptorInfo.Size == 1)
				binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			else
			{
				TRAP_ASSERT(false, "VulkanRootSignature(): Descriptor (" + descriptorInfo.Name +
							"): Cannot use VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC for arrays");
				TP_WARN("Descriptor (", descriptorInfo.Name,
						"): Cannot use VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC for arrays");
			}
		}

		binding.stageFlags = TRAP::Graphics::API::ShaderStageToVkShaderStageFlags(shaderResource.UsedStages);

		//Find if the given descriptor is a static sampler
		const auto it = staticSamplerMap.find(descriptorInfo.Name);
		if (it != staticSamplerMap.end())
		{
			const auto& [samplerName, sampler] = *it;
			VkSampler vkSampler = sampler->GetVkSampler();
			binding.pImmutableSamplers = &vkSampler;
		}

		return binding;
	}

	void FillDescriptors(std::vector<TRAP::Graphics::API::ShaderReflection::ShaderResource> shaderResources,
	                     const TRAP::Graphics::RendererAPI::RootSignatureDesc& desc,
						 std::vector<VkPushConstantRange>& outPushConstants,
						 const std::span<TRAP::Graphics::API::VulkanRenderer::UpdateFrequencyLayoutInfo> outLayouts,
	                     std::vector<TRAP::Graphics::RendererAPI::DescriptorInfo>& outDescriptors)
	{
		const TRAP::Utils::UnorderedStringMap<TRAP::Ref<TRAP::Graphics::API::VulkanSampler>> staticSamplerMap = GetStaticSamplerMap(desc);

		//Fill the descriptor array to be stored in the root signature
		outDescriptors.reserve(shaderResources.size());
		outPushConstants.reserve(std::to_underlying(TRAP::Graphics::RendererAPI::ShaderStage::SHADER_STAGE_COUNT));

		for(usize i = 0; i < shaderResources.size(); ++i)
		{
			const TRAP::Graphics::API::ShaderReflection::ShaderResource& res = shaderResources[i];
			u32 setIndex = res.Set;
			if(setIndex >= TRAP::Graphics::RendererAPI::MaxDescriptorSets &&
			   res.Type != TRAP::Graphics::RendererAPI::DescriptorType::RootConstant)
			{
				TP_ERROR(TRAP::Log::RendererVulkanRootSignaturePrefix, "Trying to use descriptor set ", setIndex, " which is not supported");
				continue;
			}

			//Copy the binding information generated from the shader reflection into the descriptor
			TRAP::Graphics::RendererAPI::DescriptorInfo& descInfo = outDescriptors.emplace_back();
			descInfo.Reg = res.Reg;
			descInfo.Size = NumericCast<u32>(res.Size);
			descInfo.Type = res.Type;
			descInfo.Name = res.Name;
			descInfo.Dimension = res.Dim;

			//If descriptor is not a root constant create a new layout binding for this descriptor and add it to
			//the binding array
			if(descInfo.Type != TRAP::Graphics::RendererAPI::DescriptorType::RootConstant)
			{
				const VkDescriptorSetLayoutBinding binding = BuildVkDescriptorSetLayoutBinding(res, descInfo, staticSamplerMap);

				//Store the Vulkan related info in the descriptor to avoid constantly calling mapping functions
				descInfo.VkType = binding.descriptorType;
				descInfo.VkStages = binding.stageFlags;
				descInfo.Set = setIndex;

				if (binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
					outLayouts[setIndex].DynamicDescriptors.emplace_back(&descInfo);

				//Set the index to an invalid value so we can use this later for error checking
				//if user tries to update a static sampler
				//In case of Combined Image Samplers, skip invalidating the index
				//because we do not introduce new ways to update the descriptor in the interface
				if(staticSamplerMap.contains(descInfo.Name) && descInfo.Type != TRAP::Graphics::RendererAPI::DescriptorType::CombinedImageSampler)
					descInfo.IndexInParent = std::numeric_limits<u32>::max();
				else
					outLayouts[setIndex].Descriptors.emplace_back(&descInfo);

				outLayouts[setIndex].Bindings.push_back(binding);
			}
			else //If descriptor is a push/root constant, add it to the push constant array
			{
				// TP_INFO("Descriptor (", descInfo.Name, "): User specified Push Constant");

				descInfo.VkStages = TRAP::Graphics::API::ShaderStageToVkShaderStageFlags(res.UsedStages);
				setIndex = 0;
				descInfo.IndexInParent = NumericCast<u32>(outPushConstants.size());

				outPushConstants.emplace_back(descInfo.VkStages, 0, descInfo.Size);
			}

			outLayouts[setIndex].DescriptorIndexMap[&descInfo] = i;
		}
	}

	void CreateVkDescriptorSetLayouts(const TRAP::Graphics::API::VulkanDevice& device,
		                              const std::span<TRAP::Graphics::API::VulkanRenderer::UpdateFrequencyLayoutInfo> outLayouts,
	                                  const std::span<VkDescriptorSetLayout> outVkDescriptorSetLayouts,
									  const std::span<u16> outVkDescriptorCounts,
									  const std::span<u32> outVkCumulativeDescriptorCounts,
									  const std::span<u8> outVkDynamicDescriptorCounts)
	{
		//Create descriptor layouts
		//Put most frequently changed params first
		for(u32 i = TRAP::Graphics::RendererAPI::MaxDescriptorSets; i-- > 0U;)
		{
			TRAP::Graphics::API::VulkanRenderer::UpdateFrequencyLayoutInfo& layout = outLayouts[i];

			if(!layout.Bindings.empty())
			{
				//Sort table by type (CBV/SRV/UAV) by register
				std::ranges::sort(layout.Bindings, [](const VkDescriptorSetLayoutBinding& lhs,
													const VkDescriptorSetLayoutBinding& rhs)
				{
					return lhs.binding > rhs.binding;
				});
				std::ranges::sort(layout.Bindings, [](const VkDescriptorSetLayoutBinding& lhs,
													const VkDescriptorSetLayoutBinding& rhs)
				{
					return lhs.descriptorType > rhs.descriptorType;
				});
			}

			bool createLayout = !layout.Bindings.empty();
			//Check if we need to create an empty layout in case there is an empty set between two used sets
			//Example: set = 0 is used, set = 2 is used.
			//In this case, set = 1 needs to exist even if it is empty
			if (!createLayout && i < (TRAP::Graphics::RendererAPI::MaxDescriptorSets - 1))
				createLayout = outVkDescriptorSetLayouts[i + 1u] != VK_NULL_HANDLE;

			if(createLayout)
			{
				const VkDescriptorSetLayoutCreateInfo layoutInfo = TRAP::Graphics::API::VulkanInits::DescriptorSetLayoutCreateInfo(layout.Bindings);

				VkCall(vkCreateDescriptorSetLayout(device.GetVkDevice(), &layoutInfo, nullptr,
												   &outVkDescriptorSetLayouts[i]));
			}

			if (layout.Bindings.empty())
				continue;

			outVkDescriptorCounts[i] = NumericCast<u16>(layout.Descriptors.size());

			//Loop through descriptor belonging to this update frequency and increment the cumulative descriptor count
			for(usize descIndex = 0; descIndex < layout.Descriptors.size(); ++descIndex)
			{
				TRAP::Graphics::RendererAPI::DescriptorInfo* const descInfo = layout.Descriptors[descIndex];
				descInfo->IndexInParent = NumericCast<u32>(descIndex);
				descInfo->HandleIndex = outVkCumulativeDescriptorCounts[i];
				outVkCumulativeDescriptorCounts[i] += descInfo->Size;
			}

			outVkDynamicDescriptorCounts[i] = NumericCast<u8>(layout.DynamicDescriptors.size());
			for(usize descIndex = 0; descIndex < outVkDynamicDescriptorCounts[i]; ++descIndex)
			{
				TRAP::Graphics::RendererAPI::DescriptorInfo* const descInfo = layout.DynamicDescriptors[descIndex];
				descInfo->RootDescriptorIndex = NumericCast<u32>(descIndex);
			}
		}
	}

	[[nodiscard]] VkPipelineLayout CreateVkPipelineLayout(const TRAP::Graphics::API::VulkanDevice& device,
	                                                      const std::span<const VkDescriptorSetLayout> vkDescriptorSetLayouts,
														  const std::span<const VkPushConstantRange> vkPushConstants)
	{
		//Pipeline layout
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
		descriptorSetLayouts.reserve(vkDescriptorSetLayouts.size());
		for(const auto& vkDescriptorSetLayout : vkDescriptorSetLayouts)
		{
			if(vkDescriptorSetLayout != nullptr)
				descriptorSetLayouts.push_back(vkDescriptorSetLayout);
		}

		const VkPipelineLayoutCreateInfo addInfo = TRAP::Graphics::API::VulkanInits::PipelineLayoutCreateInfo(descriptorSetLayouts,
																						                      vkPushConstants);
		VkCall(vkCreatePipelineLayout(device.GetVkDevice(), &addInfo, nullptr, &pipelineLayout));
		TRAP_ASSERT(pipelineLayout, "VulkanRootSignature::CreateVkPipelineLayout(): Vulkan PipelineLayout is nullptr!");

		return pipelineLayout;
	}

	void FillDescriptorWithDefaultValue(const TRAP::Graphics::RendererAPI::DescriptorInfo& descInfo,
	                                    const std::span<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData> outUpdateTemplateData)
	{
		switch(descInfo.Type)
		{
		case TRAP::Graphics::RendererAPI::DescriptorType::Sampler:
			for (u32 arr = 0; arr < descInfo.Size; ++arr)
			{
				outUpdateTemplateData[descInfo.HandleIndex + NumericCast<usize>(arr)].ImageInfo =
				{
					TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors->DefaultSampler->GetVkSampler(),
					VK_NULL_HANDLE,
					VK_IMAGE_LAYOUT_UNDEFINED
				};
			}
			break;

		case TRAP::Graphics::RendererAPI::DescriptorType::Texture:
			for (u32 arr = 0; arr < descInfo.Size; ++arr)
			{
				outUpdateTemplateData[descInfo.HandleIndex + NumericCast<usize>(arr)].ImageInfo =
				{
					VK_NULL_HANDLE,
					std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors->DefaultTextureSRV[std::to_underlying(descInfo.Dimension)])->GetSRVVkImageView(),
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				};
			}
			break;

		case TRAP::Graphics::RendererAPI::DescriptorType::RWTexture:
			for (u32 arr = 0; arr < descInfo.Size; ++arr)
			{
				outUpdateTemplateData[descInfo.HandleIndex + NumericCast<usize>(arr)].ImageInfo =
				{
					VK_NULL_HANDLE,
					std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors->DefaultTextureUAV[std::to_underlying(descInfo.Dimension)])->GetUAVVkImageViews()[0],
					VK_IMAGE_LAYOUT_GENERAL
				};
			}
			break;

		case TRAP::Graphics::RendererAPI::DescriptorType::UniformBuffer:
			[[fallthrough]];
		case TRAP::Graphics::RendererAPI::DescriptorType::Buffer:
			[[fallthrough]];
		case TRAP::Graphics::RendererAPI::DescriptorType::BufferRaw:
			for(u32 arr = 0; arr < descInfo.Size; ++arr)
			{
				outUpdateTemplateData[descInfo.HandleIndex + NumericCast<usize>(arr)].BufferInfo =
				{
					TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors->DefaultBufferSRV->GetVkBuffer(),
					TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors->DefaultBufferSRV->GetOffset(),
					VK_WHOLE_SIZE
				};
			}
			break;

		case TRAP::Graphics::RendererAPI::DescriptorType::RWBuffer:
			[[fallthrough]];
		case TRAP::Graphics::RendererAPI::DescriptorType::RWBufferRaw:
			for(u32 arr = 0; arr < descInfo.Size; ++arr)
			{
				outUpdateTemplateData[descInfo.HandleIndex + NumericCast<usize>(arr)].BufferInfo =
				{
					TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors->DefaultBufferUAV->GetVkBuffer(),
					TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors->DefaultBufferUAV->GetOffset(),
					VK_WHOLE_SIZE
				};
			}
			break;

		case TRAP::Graphics::RendererAPI::DescriptorType::TexelBuffer:
			for (u32 arr = 0; arr < descInfo.Size; ++arr)
				outUpdateTemplateData[descInfo.HandleIndex + NumericCast<usize>(arr)].BufferView = TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors->DefaultBufferSRV->GetUniformTexelView();
			break;

		case TRAP::Graphics::RendererAPI::DescriptorType::RWTexelBuffer:
			for (u32 arr = 0; arr < descInfo.Size; ++arr)
				outUpdateTemplateData[descInfo.HandleIndex + NumericCast<usize>(arr)].BufferView = TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors->DefaultBufferUAV->GetStorageTexelView();
			break;

		default:
			break;
		}
	}

	[[nodiscard]] std::vector<VkDescriptorUpdateTemplateEntry> FillDescriptorsWithDefaultValues(const TRAP::Graphics::API::VulkanRenderer::UpdateFrequencyLayoutInfo& layout,
	                                                                                            const u16 vkDescriptorCount,
	                                                                                            const std::span<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData> outUpdateTemplateData,
		                                                                                        u8& outVkRayTracingDescriptorCount)
	{
		std::vector<VkDescriptorUpdateTemplateEntry> entries{};
		entries.reserve(vkDescriptorCount);

		//Fill the write descriptors with default values during initialization so the only thing we change
		//in CmdBindDescriptors are the VkBuffer / VKImageView objects
		for (const auto* const descInfo : layout.Descriptors)
		{
			//Raytracing descriptor dont support update template so we ignore them
			if(descInfo->Type == TRAP::Graphics::RendererAPI::DescriptorType::RayTracing)
			{
				outVkRayTracingDescriptorCount += NumericCast<u8>(descInfo->Size);
				continue;
			}

			const u64 offset = descInfo->HandleIndex * sizeof(TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData);

			entries.emplace_back(descInfo->Reg, 0, descInfo->Size, descInfo->VkType, offset,
								 sizeof(TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData));

			FillDescriptorWithDefaultValue(*descInfo, outUpdateTemplateData);
		}

		return entries;
	}

	void UpdateDescriptorSetTemplate(const TRAP::Graphics::API::VulkanDevice& device,
	                                 const TRAP::Graphics::RendererAPI::PipelineType pipelineType,
	                                 VkPipelineLayout pipelineLayout,
		                             const u32 setIndex,
	                                 const std::span<const TRAP::Graphics::API::VulkanRenderer::UpdateFrequencyLayoutInfo> layouts,
	                                 const std::span<const u16> vkDescriptorCounts,
									 const std::span<const u32> vkCumulativeDescriptorCounts,
									 const std::span<const VkDescriptorSetLayout> vkDescriptorSetLayouts,
									 const std::span<std::vector<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData>> outUpdateTemplateData,
									 const std::span<VkDescriptorUpdateTemplate> outUpdateTemplates,
									 const std::span<VkDescriptorSet> outVkEmptyDescriptorSets,
									 const std::span<u8> outVkRayTracingDescriptorCounts)
	{
		if(vkDescriptorCounts[setIndex] != 0u)
		{
			outUpdateTemplateData[setIndex].resize(vkCumulativeDescriptorCounts[setIndex]);

			const std::vector<VkDescriptorUpdateTemplateEntry> entries = FillDescriptorsWithDefaultValues(layouts[setIndex],
			                                                                                              vkDescriptorCounts[setIndex],
																										  outUpdateTemplateData[setIndex],
																										  outVkRayTracingDescriptorCounts[setIndex]);

			const VkDescriptorUpdateTemplateCreateInfo createInfo = TRAP::Graphics::API::VulkanInits::DescriptorUpdateTemplateCreateInfo
			(
				vkDescriptorSetLayouts[setIndex], entries,
				TRAP::Graphics::API::VkPipelineBindPointTranslator[std::to_underlying(pipelineType)],
				pipelineLayout, setIndex
			);
			VkCall(vkCreateDescriptorUpdateTemplate(device.GetVkDevice(), &createInfo, nullptr,
													&outUpdateTemplates[setIndex]));
		}
		else if(vkDescriptorSetLayouts[setIndex] != VK_NULL_HANDLE)
		{
			//Consume empty descriptor sets from empty descriptor set pool
			outVkEmptyDescriptorSets[setIndex] = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanDescriptorPool>
			(
				TRAP::Graphics::RendererAPI::GetDescriptorPool()
			)->RetrieveVkDescriptorSet(vkDescriptorSetLayouts[setIndex]);
		}
	}

	void UpdateDescriptorTemplates(const TRAP::Graphics::API::VulkanDevice& device,
	                               const TRAP::Graphics::RendererAPI::PipelineType pipelineType,
	                               VkPipelineLayout pipelineLayout,
	                               const std::span<const TRAP::Graphics::API::VulkanRenderer::UpdateFrequencyLayoutInfo> layouts,
	                               const std::span<const u16> vkDescriptorCounts,
								   const std::span<const u32> vkCumulativeDescriptorCounts,
								   const std::span<const VkDescriptorSetLayout> vkDescriptorSetLayouts,
								   const std::span<std::vector<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData>> outUpdateTemplateData,
								   const std::span<VkDescriptorUpdateTemplate> outUpdateTemplates,
								   const std::span<VkDescriptorSet> outVkEmptyDescriptorSets,
								   const std::span<u8> outVkRayTracingDescriptorCounts)
	{
		//Update templates
		for(u32 setIndex = 0; setIndex < TRAP::Graphics::RendererAPI::MaxDescriptorSets; ++setIndex)
		{
			UpdateDescriptorSetTemplate(device, pipelineType, pipelineLayout, setIndex, layouts,
		                                vkDescriptorCounts, vkCumulativeDescriptorCounts, vkDescriptorSetLayouts,
										outUpdateTemplateData, outUpdateTemplates, outVkEmptyDescriptorSets,
										outVkRayTracingDescriptorCounts);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRootSignature::VulkanRootSignature(const RendererAPI::RootSignatureDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanRootSignature(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRootSignaturePrefix, "Creating RootSignature");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	//This determines the pipeline type from the first shader in the reflection
	if(!desc.Shaders.empty() && desc.Shaders.front() != nullptr)
		m_pipelineType = GetReflectionPipelineType(*desc.Shaders.front());
	TRAP_ASSERT(m_pipelineType != RendererAPI::PipelineType::Undefined, "VulkanRootSignature::VulkanRootSignature(): Pipeline type is undefined!");

	auto [shaderResources, indexMap] = CollectUniqueShaderResources(desc);
	m_descriptorNameToIndexMap = indexMap;

	std::array<VulkanRenderer::UpdateFrequencyLayoutInfo, RendererAPI::MaxDescriptorSets> layouts{};
	std::vector<VkPushConstantRange> pushConstants{};
	FillDescriptors(shaderResources, desc, pushConstants, layouts, m_descriptors);
	m_vkPushConstantCount = NumericCast<u32>(pushConstants.size());

	CreateVkDescriptorSetLayouts(*m_device, layouts, m_vkDescriptorSetLayouts, m_vkDescriptorCounts,
	                             m_vkCumulativeDescriptorsCounts, m_vkDynamicDescriptorCounts);

	m_pipelineLayout = CreateVkPipelineLayout(*m_device, m_vkDescriptorSetLayouts, pushConstants);

	UpdateDescriptorTemplates(*m_device, m_pipelineType, m_pipelineLayout, layouts, m_vkDescriptorCounts,
	                          m_vkCumulativeDescriptorsCounts, m_vkDescriptorSetLayouts, m_updateTemplateData,
							  m_updateTemplates, m_vkEmptyDescriptorSets, m_vkRayTracingDescriptorCounts);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRootSignature::~VulkanRootSignature()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRootSignaturePrefix, "Destroying RootSignature");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	for(u32 i = 0; i < RendererAPI::MaxDescriptorSets; ++i)
	{
		vkDestroyDescriptorSetLayout(m_device->GetVkDevice(), m_vkDescriptorSetLayouts[i], nullptr);
		if (m_updateTemplates[i] != VK_NULL_HANDLE)
			vkDestroyDescriptorUpdateTemplate(m_device->GetVkDevice(), m_updateTemplates[i], nullptr);
	}

	vkDestroyPipelineLayout(m_device->GetVkDevice(), m_pipelineLayout, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Graphics::RendererAPI::DescriptorInfo* TRAP::Graphics::API::VulkanRootSignature::GetDescriptor(const std::string_view resName) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	const auto it = std::ranges::find_if(m_descriptorNameToIndexMap, [resName](const auto& pair){return pair.first == resName;});
	if (it != m_descriptorNameToIndexMap.end())
		return &m_descriptors[it->second];

	return nullptr;
}
