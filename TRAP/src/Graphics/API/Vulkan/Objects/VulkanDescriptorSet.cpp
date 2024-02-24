#include "TRAPPCH.h"
#include "VulkanDescriptorSet.h"

#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "VulkanSampler.h"
#include "VulkanRootSignature.h"
#include "VulkanTexture.h"
#include "Graphics/Textures/Texture.h"

TRAP::Graphics::API::VulkanDescriptorSet::VulkanDescriptorSet(TRAP::Ref<VulkanDevice> device,
	                                                          std::vector<VkDescriptorSet> vkDescriptorSetHandles,
	                                                          TRAP::Ref<VulkanRootSignature> rootSignature,
	                                                          std::vector<std::vector<VulkanRenderer::DescriptorUpdateData>> updateData,
	                                                          const u32 maxSets, const u8 dynamicOffsetCount,
	                                                          const u32 set)
		: m_vkDescriptorSetHandles(std::move(vkDescriptorSetHandles)),
		  m_rootSignature(std::move(rootSignature)),
		  m_updateData(std::move(updateData)),
		  m_dynamicSizeOffsets(NumericCast<usize>(dynamicOffsetCount) * maxSets),
		  m_maxSets(maxSets),
		  m_dynamicOffsetCount(dynamicOffsetCount),
		  m_set(set),
	      m_device(std::move(device))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanDescriptorSet(): Vulkan Device is nullptr");
	TRAP_ASSERT(m_rootSignature, "VulkanDescriptorSet(): Vulkan RootSignature is nullptr");
	TRAP_ASSERT(!m_vkDescriptorSetHandles.empty(), "VulkanDescriptorSet(): No Vulkan DescriptorSets available!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::VulkanRootSignature> TRAP::Graphics::API::VulkanDescriptorSet::GetRootSignature() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_rootSignature;
}

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
#ifdef ENABLE_GRAPHICS_DEBUG
	template<typename... Ts>
	[[nodiscard]] constexpr bool ValidateDescriptor(const bool condition, const Ts&... args)
	{
		if(!(condition))
		{
			TP_ERROR(TRAP::Log::RendererVulkanDescriptorSetPrefix, args...);
			TRAP_ASSERT(false, args...);

			return false;
		}

		return true;
	}
#else
	template<typename... Ts>
	[[nodiscard]] constexpr bool ValidateDescriptor([[maybe_unused]] const bool condition,
	                                                [[maybe_unused]] const Ts&... args)
	{
		return true;
	}
#endif

	[[nodiscard]] constexpr bool UpdateSampler(const TRAP::Graphics::RendererAPI::DescriptorData& descData,
	                                           const TRAP::Graphics::RendererAPI::DescriptorInfo& descInfo,
											   const u32 arrayCount,
											   const std::span<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData> updateData)
	{
		bool needsUpdate = false;

		//Index is invalid when descriptor is a static sampler
		if(!ValidateDescriptor(descInfo.IndexInParent != std::numeric_limits<u32>::max(),
		                       "VulkanDescriptorSet::UpdateSampler(): Trying to update a static sampler (\"",
							   descInfo.Name,
							   "\"). All static samplers must be set in RootSignature constructor and cannot be updated later!"))
		{
			return needsUpdate;
		}

		const std::span<const TRAP::Graphics::Sampler* const> samplers = std::get<std::vector<TRAP::Graphics::Sampler*>>(descData.Resource);
		if(!ValidateDescriptor(!samplers.empty(), "VulkanDescriptorSet::UpdateSampler(): Empty sampler(s) (\"", descInfo.Name, "\")!"))
			return needsUpdate;

		for(u32 arr = 0; arr < arrayCount; ++arr)
		{
			if(!ValidateDescriptor(samplers[arr] != nullptr, fmt::format("VulkanDescriptor::UpdateSampler(): Sampler (\"{}\" [{}]) is nullptr!", descInfo.Name, arr)))
			{
				return needsUpdate;
			}

			updateData[NumericCast<usize>(descInfo.HandleIndex) + arr].ImageInfo =
			{
				dynamic_cast<const TRAP::Graphics::API::VulkanSampler* const>(samplers[arr])->GetVkSampler(),
				VK_NULL_HANDLE,
				VK_IMAGE_LAYOUT_UNDEFINED
			};
			needsUpdate |= true;
		}

		return needsUpdate;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] bool UpdateCombinedImageSampler(const TRAP::Graphics::RendererAPI::DescriptorData& descData,
	                                              const TRAP::Graphics::RendererAPI::DescriptorInfo& descInfo,
											      const u32 arrayCount,
												  const TRAP::Graphics::API::VulkanRootSignature& rootSignature,
											      const std::span<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData> updateData)
	{
		bool needsUpdate = false;

		const std::span<const TRAP::Ref<TRAP::Graphics::Texture>> textures = std::get<std::vector<TRAP::Ref<TRAP::Graphics::Texture>>>(descData.Resource);
		if(!ValidateDescriptor(!textures.empty(), "VulkanDescriptorSet::UpdateCombinedImageSampler(): Empty texture(s) (\"", descInfo.Name, "\")"))
		{
			return needsUpdate;
		}

		if(!rootSignature.GetDescriptorNameToIndexMap().contains(descInfo.Name))
		{
			const std::string msg = fmt::format("VulkanDescriptorSet::UpdateCombinedImageSampler(): No Static Sampler with name \"{}\" found!", descInfo.Name);
			TP_ERROR(TRAP::Log::RendererVulkanDescriptorSetPrefix, msg);
			TRAP_ASSERT(false, msg);
			return needsUpdate;
		}

		for(u32 arr = 0; arr < arrayCount; ++arr)
		{
			if(!ValidateDescriptor(textures[arr] != nullptr, fmt::format("VulkanDescriptorSet::UpdateCombinedImageSampler(): Texture (\"{}\" [{}]) is nullptr!", descInfo.Name, arr)))
			{
				return needsUpdate;
			}

			updateData[NumericCast<usize>(descInfo.HandleIndex) + arr].ImageInfo =
			{
				VK_NULL_HANDLE, //Sampler
				std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(textures[arr])->GetSRVVkImageView(), //Image View
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL //Image Layout
			};

			needsUpdate |= true;
		}

		return needsUpdate;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr bool UpdateTexture(const TRAP::Graphics::RendererAPI::DescriptorData& descData,
	                                           const TRAP::Graphics::RendererAPI::DescriptorInfo& descInfo,
											   const u32 arrayCount,
											   const std::span<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData> updateData)
	{
		bool needsUpdate = false;

		const std::span<const TRAP::Ref<TRAP::Graphics::Texture>> textures = std::get<std::vector<TRAP::Ref<TRAP::Graphics::Texture>>>(descData.Resource);
		if(!ValidateDescriptor(!textures.empty(), "VulkanDescriptorSet::UpdateTexture(): Empty texture(s) (\"", descInfo.Name, "\")"))
			return needsUpdate;

		if(!std::get<bool>(descData.Offset))
		{
			for(u32 arr = 0; arr < arrayCount; ++arr)
			{
				if(!ValidateDescriptor(textures[arr] != nullptr, fmt::format("VulkanDescriptorSet::UpdateTexture(): Texture (\"{}\" [{}]) is nullptr!", descInfo.Name, arr)))
				{
					return needsUpdate;
				}

				updateData[NumericCast<usize>(descInfo.HandleIndex) + arr].ImageInfo =
				{
					VK_NULL_HANDLE, //Sampler
					std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(textures[arr])->GetSRVVkImageView(), //Image View
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL //Image Layout
				};

				needsUpdate |= true;
			}
		}
		else
		{
			for(u32 arr = 0; arr < arrayCount; ++arr)
			{
				if(!ValidateDescriptor(textures[arr] != nullptr, fmt::format("VulkanDescriptorSet::UpdateTexture(): Texture (\"{}\" [{}]) is nullptr!", descInfo.Name, arr)))
				{
					return needsUpdate;
				}

				updateData[NumericCast<usize>(descInfo.HandleIndex) + arr].ImageInfo =
				{
					VK_NULL_HANDLE, //Sampler
					std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(textures[arr])->GetSRVStencilVkImageView(), //Image View
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL //Image Layout
				};

				needsUpdate |= true;
			}
		}

		return needsUpdate;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr bool UpdateRWTexture(const TRAP::Graphics::RendererAPI::DescriptorData& descData,
	                                             const TRAP::Graphics::RendererAPI::DescriptorInfo& descInfo,
											     const u32 arrayCount,
											     const std::span<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData> updateData)
	{
		bool needsUpdate = false;

		const std::span<const TRAP::Ref<TRAP::Graphics::Texture>> textures = std::get<std::vector<TRAP::Ref<TRAP::Graphics::Texture>>>(descData.Resource);
		if(!ValidateDescriptor(!textures.empty(), "VulkanDescriptorSet::UpdateRWTexture(): Empty RW texture(s) (\"", descInfo.Name, "\")"))
		{
			return needsUpdate;
		}

		if(std::get<TRAP::Graphics::RendererAPI::DescriptorData::TextureSlice>(descData.Offset).BindMipChain)
		{
			if(!ValidateDescriptor(textures[0] != nullptr, "VulkanDescriptorSet::UpdateRWTexture(): RW Texture (\"", descInfo.Name, "\") is nullptr!"))
				return needsUpdate;

			for (u32 arr = 0; arr < arrayCount; ++arr)
			{
				updateData[NumericCast<usize>(descInfo.HandleIndex) + arr].ImageInfo =
				{
					VK_NULL_HANDLE, //Sampler
					std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(textures[0])->GetUAVVkImageViews()[arr], //Image View
					VK_IMAGE_LAYOUT_GENERAL //Image Layout
				};

				needsUpdate |= true;
			}
		}
		else
		{
			const u32 mipSlice = std::get<TRAP::Graphics::RendererAPI::DescriptorData::TextureSlice>
				(
					descData.Offset
				).UAVMipSlice;

			for(u32 arr = 0; arr < arrayCount; ++arr)
			{
				if(!ValidateDescriptor(textures[arr] != nullptr, "VulkanDescriptorSet::UpdateRWTexture(): RW Texture (\"", descInfo.Name, "\") is nullptr!"))
					return needsUpdate;
				if(!ValidateDescriptor(mipSlice < textures[arr]->GetMipLevels(), fmt::format("RW Texture (\"{}\" [{}]) Mip Slice ({}) exceeds mip levels ({})",
									   descInfo.Name, arr, mipSlice, textures[arr]->GetMipLevels())))
				{
					return needsUpdate;
				}

				updateData[NumericCast<usize>(descInfo.HandleIndex) + arr].ImageInfo =
				{
					VK_NULL_HANDLE, //Sampler
					std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(textures[arr])->GetUAVVkImageViews()[mipSlice], //Image View
					VK_IMAGE_LAYOUT_GENERAL //Image Layout
				};

				needsUpdate |= true;
			}
		}

		return needsUpdate;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr bool UpdateDynamicUniformBuffer(const TRAP::Graphics::RendererAPI::DescriptorData& descData,
	                                                        const TRAP::Graphics::RendererAPI::DescriptorInfo& descInfo,
											                const u32 arrayCount,
															const u32 index,
															const std::span<TRAP::Graphics::API::VulkanRenderer::SizeOffset> dynamicSizeOffsets,
											                const std::span<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData> updateData)
	{
		bool needsUpdate = false;

		if(descInfo.VkType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
			return false;

		const std::span<const TRAP::Graphics::Buffer* const> buffers = std::get<std::vector<TRAP::Graphics::Buffer*>>(descData.Resource);
		const TRAP::Graphics::RendererAPI::DescriptorData::BufferOffset& off = std::get<TRAP::Graphics::RendererAPI::DescriptorData::BufferOffset>(descData.Offset);
		if(!ValidateDescriptor(!buffers.empty(), "VulkanDescriptorSet::UpdateDynamicUniformBuffer(): Empty uniform buffer(s) (\"", descInfo.Name, "\")"))
			return needsUpdate;
		if(!ValidateDescriptor(buffers[0] != nullptr, "VulkanDescriptorSet::UpdateDynamicUniformBuffer(): Uniform buffer (\"", descInfo.Name, "\" [0]) is nullptr!"))
			return needsUpdate;
		if(!ValidateDescriptor(arrayCount == 1, "VulkanDescriptorSet::UpdateDynamicUniformBuffer(): Dynamic uniform buffer does not support arrays!"))
			return needsUpdate;
		if(!ValidateDescriptor(!off.Sizes.empty(), "VulkanDescriptorSet::UpdateDynamicUniformBuffer(): No sizes provided for dynamic uniform buffer (\"", descInfo.Name, "\")!"))
			return needsUpdate;
		if(!ValidateDescriptor(off.Sizes[0] > 0, "VulkanDescriptorSet::UpdateDynamicUniformBuffer(): Sizes[0] is 0 for dynamic uniform buffer (\"", descInfo.Name, "\")!"))
			return needsUpdate;
		if(!ValidateDescriptor(off.Sizes[0] <= TRAP::Graphics::RendererAPI::GPUSettings.MaxUniformBufferRange,
		                       fmt::format("VulkanDescriptorSet::UpdateDynamicUniformBuffer(): Sizes[0] ({}) exceeds maximum supported uniform buffer range {} for dynamic uniform buffer (\"{}\")!",
							   off.Sizes[0], TRAP::Graphics::RendererAPI::GPUSettings.MaxUniformBufferRange, descInfo.Name)))
		{
			return needsUpdate;
		}

		dynamicSizeOffsets[index].Offset = !off.Offsets.empty() ? NumericCast<u32>(off.Offsets[0]) : 0u;
		const TRAP::Graphics::API::VulkanBuffer* const buf = dynamic_cast<const TRAP::Graphics::API::VulkanBuffer* const>(buffers[0]);
		updateData[NumericCast<usize>(descInfo.HandleIndex) + 0u].BufferInfo =
		{
			buf->GetVkBuffer(),
			buf->GetOffset(),
			off.Sizes[0]
		};

		//If this is a different size we have to update the VkDescriptorBufferInfo::range so a call to
		//vkUpdateDescriptorSet is necessary
		if (off.Sizes[0] != dynamicSizeOffsets[index].Size)
		{
			dynamicSizeOffsets[index].Size = NumericCast<u32>(off.Sizes[0]);
			needsUpdate |= true;
		}

		return needsUpdate;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr bool UpdateBuffer(const TRAP::Graphics::RendererAPI::DescriptorData& descData,
											  const TRAP::Graphics::RendererAPI::DescriptorInfo& descInfo,
											  const u32 arrayCount,
											  const std::span<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData> updateData)
	{
		bool needsUpdate = false;

		const std::span<const TRAP::Graphics::Buffer* const> buffers = std::get<std::vector<TRAP::Graphics::Buffer*>>(descData.Resource);
		if(!ValidateDescriptor(!buffers.empty(), "VulkanDescriptorSet::UpdateBuffer(): Empty buffer(s) (\"", descInfo.Name, "\")"))
			return false;

		for (u32 arr = 0; arr < arrayCount; ++arr)
		{
			if(!ValidateDescriptor(buffers[arr] != nullptr, fmt::format("VulkanDescriptorSet::UpdateBuffer(): Buffer ({}, [{}]) is nullptr!", descInfo.Name, arr)))
				return needsUpdate;

			const TRAP::Graphics::API::VulkanBuffer* const buf = dynamic_cast<const TRAP::Graphics::API::VulkanBuffer* const>(buffers[arr]);
			updateData[NumericCast<usize>(descInfo.HandleIndex) + arr].BufferInfo =
			{
				buf->GetVkBuffer(),
				buf->GetOffset(),
				VK_WHOLE_SIZE
			};

			const TRAP::Graphics::RendererAPI::DescriptorData::BufferOffset& off = std::get<TRAP::Graphics::RendererAPI::DescriptorData::BufferOffset>(descData.Offset);
			if (!off.Offsets.empty())
			{
				if(!ValidateDescriptor(!off.Sizes.empty(), "VulkanDescriptorSet::UpdateBuffer(): Buffer offsets provided but no sizes for buffer (\"", descInfo.Name, "\")!"))
					return needsUpdate;
				if(!ValidateDescriptor(off.Sizes[arr] > 0, "VulkanDescriptorSet::UpdateBuffer(): Sizes[0] is 0 for buffer (\"", descInfo.Name, "\")!"))
					return needsUpdate;
				if(!ValidateDescriptor(off.Sizes[arr] <= TRAP::Graphics::RendererAPI::GPUSettings.MaxStorageBufferRange,
				                       fmt::format("VulkanDescriptorSet::UpdateBuffer(): Sizes[{}] ({}) exceeds maximum supported storage buffer range {} for buffer (\"{}\")!",
									   arr, off.Sizes[arr], TRAP::Graphics::RendererAPI::GPUSettings.MaxStorageBufferRange, descInfo.Name)))
				{
					return needsUpdate;
				}

				updateData[NumericCast<usize>(descInfo.HandleIndex) + arr].BufferInfo.offset = off.Offsets[arr];
				updateData[NumericCast<usize>(descInfo.HandleIndex) + arr].BufferInfo.range = off.Sizes[arr];
			}

			needsUpdate |= true;
		}

		return needsUpdate;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr bool UpdateTexelBuffer(const TRAP::Graphics::RendererAPI::DescriptorData& descData,
											       const TRAP::Graphics::RendererAPI::DescriptorInfo& descInfo,
											       const u32 arrayCount,
											       const std::span<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData> updateData)
	{
		bool needsUpdate = false;

		const std::span<const TRAP::Graphics::Buffer* const> buffers = std::get<std::vector<TRAP::Graphics::Buffer*>>(descData.Resource);
		if(!ValidateDescriptor(!buffers.empty(), "VulkanDescriptorSet::UpdateTexelBuffer(): Empty texel buffer(s) (\"", descInfo.Name, "\")"))
			return needsUpdate;

		for(u32 arr = 0; arr < arrayCount; ++arr)
		{
			if(!ValidateDescriptor(buffers[arr] != nullptr, fmt::format("VulkanDescriptorSet::UpdateTexelBuffer(): Texel buffer (\"{}\" [{}]) is nullptr!", descInfo.Name, arr)))
				return needsUpdate;

			updateData[NumericCast<usize>(descInfo.HandleIndex) + arr].BufferView = dynamic_cast<const TRAP::Graphics::API::VulkanBuffer* const>
				(
					buffers[arr]
				)->GetUniformTexelView();
			needsUpdate |= true;
		}

		return needsUpdate;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr bool UpdateRWTexelBuffer(const TRAP::Graphics::RendererAPI::DescriptorData& descData,
											         const TRAP::Graphics::RendererAPI::DescriptorInfo& descInfo,
											         const u32 arrayCount,
											         const std::span<TRAP::Graphics::API::VulkanRenderer::DescriptorUpdateData> updateData)
	{
		bool needsUpdate = false;

		const std::span<const TRAP::Graphics::Buffer* const> buffers = std::get<std::vector<TRAP::Graphics::Buffer*>>(descData.Resource);
		if(!ValidateDescriptor(!buffers.empty(), "VulkanDescriptorSet::UpdateRWTexelBuffer(): Empty RW Texel buffer(s) (\"", descInfo.Name, "\")"))
			return needsUpdate;

		for (u32 arr = 0; arr < arrayCount; ++arr)
		{
			if(!ValidateDescriptor(buffers[arr] != nullptr, fmt::format("VulkanDescriptorSet::UpdateRWTexelBuffer(): RW Texelbuffer (\"{}\" [{}]) is nullptr!", descInfo.Name, arr)))
			{
				return needsUpdate;
			}

			updateData[NumericCast<usize>(descInfo.HandleIndex) + arr].BufferView = dynamic_cast<const TRAP::Graphics::API::VulkanBuffer* const>
				(
					buffers[arr]
				)->GetStorageTexelView();
			needsUpdate |= true;
		}

		return needsUpdate;
	}
}

void TRAP::Graphics::API::VulkanDescriptorSet::Update(const u32 index,
                                                      const std::vector<RendererAPI::DescriptorData>& params)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(index < m_maxSets, "VulkanDescriptorSet::Update(): Index out of range!");

	std::span<VulkanRenderer::DescriptorUpdateData> updateData = m_updateData[index];
	bool needsUpdate = false;

	for (const auto& param : params)
	{
		const u32 paramIndex = param.Index;

		if(!ValidateDescriptor(!param.Name.empty() || (paramIndex != std::numeric_limits<u32>::max()), "VulkanDescriptorSet::Update(): DescriptorData name is empty and param index is invalid!"))
			continue;

		const RendererAPI::DescriptorInfo* const desc = (paramIndex != std::numeric_limits<u32>::max()) ?
		                                                (&m_rootSignature->GetDescriptors()[paramIndex]) :
												        m_rootSignature->GetDescriptor(param.Name);

		if(paramIndex != std::numeric_limits<u32>::max())
		{
			if(!ValidateDescriptor(desc != nullptr, "VulkanDescriptorSet::Update(): Invalid descriptor info with param index ", paramIndex))
				continue;
		}
		else
		{
			if(!ValidateDescriptor(desc != nullptr, "VulkanDescriptorSet::Update(): Invalid descriptor info with param name \"", param.Name, "\""))
				continue;
		}

		const RendererAPI::DescriptorType type = desc->Type;
		const u32 arrayCount = TRAP::Math::Max(1U, param.Count);

		if(!ValidateDescriptor(desc->Set == m_set, "VulkanDescriptorSet::Update(): Descriptor \"", desc->Name, "\" has mismatching set index!"))
			continue;

		switch(type)
		{
		case RendererAPI::DescriptorType::Sampler:
		{
			needsUpdate |= UpdateSampler(param, *desc, arrayCount, updateData);
			break;
		}

		case RendererAPI::DescriptorType::CombinedImageSampler:
		{
			needsUpdate |= UpdateCombinedImageSampler(param, *desc, arrayCount, *m_rootSignature, updateData);
			break;
		}

		case RendererAPI::DescriptorType::Texture:
		{
			needsUpdate |= UpdateTexture(param, *desc, arrayCount, updateData);
			break;
		}

		case RendererAPI::DescriptorType::RWTexture:
		{
			needsUpdate |= UpdateRWTexture(param, *desc, arrayCount, updateData);
			break;
		}

		case RendererAPI::DescriptorType::UniformBuffer:
		{
			if(desc->VkType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
				needsUpdate |= UpdateDynamicUniformBuffer(param, *desc, arrayCount, index, m_dynamicSizeOffsets, updateData);
			else
				needsUpdate |= UpdateBuffer(param, *desc, arrayCount, updateData);
			break;
		}

		case RendererAPI::DescriptorType::Buffer:
			[[fallthrough]];
		case RendererAPI::DescriptorType::BufferRaw:
			[[fallthrough]];
		case RendererAPI::DescriptorType::RWBuffer:
			[[fallthrough]];
		case RendererAPI::DescriptorType::RWBufferRaw:
		{
			needsUpdate |= UpdateBuffer(param, *desc, arrayCount, updateData);
			break;
		}

		case RendererAPI::DescriptorType::TexelBuffer:
		{
			needsUpdate |= UpdateTexelBuffer(param, *desc, arrayCount, updateData);
			break;
		}

		case RendererAPI::DescriptorType::RWTexelBuffer:
		{
			needsUpdate |= UpdateRWTexelBuffer(param, *desc, arrayCount, updateData);
			break;
		}

		default:
			break;
		}
	}

	//If this was called to just update a dynamic offset skip the update
	if (needsUpdate)
	{
		vkUpdateDescriptorSetWithTemplate(m_device->GetVkDevice(), m_vkDescriptorSetHandles[index],
		                                  m_rootSignature->GetUpdateTemplates()[m_set],
										  updateData.data());
	}
}
