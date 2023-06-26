#include "TRAPPCH.h"
#include "VulkanDescriptorSet.h"

#include "VulkanBuffer.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanSampler.h"
#include "VulkanRootSignature.h"
#include "VulkanTexture.h"
#include "Graphics/Textures/Texture.h"

TRAP::Graphics::API::VulkanDescriptorSet::VulkanDescriptorSet(TRAP::Ref<VulkanDevice> device,
	                                                          std::vector<VkDescriptorSet> vkDescriptorSetHandles,
	                                                          TRAP::Ref<VulkanRootSignature> rootSignature,
	                                                          std::vector<std::vector<VulkanRenderer::DescriptorUpdateData>> updateData,
	                                                          const uint32_t maxSets, const uint8_t dynamicOffsetCount,
	                                                          const uint32_t set)
		: m_vkDescriptorSetHandles(std::move(vkDescriptorSetHandles)),
		  m_rootSignature(std::move(rootSignature)),
		  m_updateData(std::move(updateData)),
		  m_dynamicSizeOffsets(NumericCast<std::size_t>(dynamicOffsetCount) * maxSets),
		  m_maxSets(maxSets),
		  m_dynamicOffsetCount(dynamicOffsetCount),
		  m_set(set),
	      m_device(std::move(device))
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanDescriptorSet(): Vulkan Device is nullptr");
	TRAP_ASSERT(m_rootSignature, "VulkanDescriptorSet(): Vulkan RootSignature is nullptr");
	TRAP_ASSERT(!m_vkDescriptorSetHandles.empty(), "VulkanDescriptorSet(): No Vulkan DescriptorSets available!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::VulkanRootSignature> TRAP::Graphics::API::VulkanDescriptorSet::GetRootSignature() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_rootSignature;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency TRAP::Graphics::API::VulkanDescriptorSet::GetUpdateFrequency() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_set > 0 ? RendererAPI::DescriptorUpdateFrequency::Dynamic :
	                   RendererAPI::DescriptorUpdateFrequency::Static;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint8_t TRAP::Graphics::API::VulkanDescriptorSet::GetDynamicOffsetCount() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_dynamicOffsetCount;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::API::VulkanDescriptorSet::GetMaxSets() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_maxSets;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::API::VulkanDescriptorSet::GetSet() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_set;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDescriptorSet::Update(const uint32_t index,
                                                      const std::vector<RendererAPI::DescriptorData>& params)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef ENABLE_GRAPHICS_DEBUG
#define VALIDATE_DESCRIPTOR(descriptor, ...)                                                  \
	if(!(descriptor))                                                                         \
	{                                                                                         \
	    constexpr std::source_location descLoc = std::source_location::current();             \
		const std::string msg = fmt::format("{} : {}", descLoc.function_name(), __VA_ARGS__); \
		TP_ERROR(Log::RendererVulkanDescriptorSetPrefix, msg);                                \
		TRAP_ASSERT(false, msg);                                                              \
		continue;                                                                             \
	}
#else
#define VALIDATE_DESCRIPTOR(descriptor, ...)
#endif

	TRAP_ASSERT(index < m_maxSets, "VulkanDescriptorSet::Update(): Index out of range!");

	const TRAP::Ref<VulkanRootSignature>& rootSignature = m_rootSignature;
	std::vector<VulkanRenderer::DescriptorUpdateData>& updateData = m_updateData[index];
	bool update = false;

	std::vector<VkWriteDescriptorSet> rayTracingWrites;
	std::vector<VkWriteDescriptorSetAccelerationStructureKHR> rayTracingWritesKHR;
	const uint32_t rayTracingWriteCount = 0; //TODO Use

	if(rootSignature->GetVkRayTracingDescriptorCounts()[m_set] != 0u)
	{
		rayTracingWrites.resize(rootSignature->GetVkRayTracingDescriptorCounts()[m_set]);
		rayTracingWritesKHR.resize(rootSignature->GetVkRayTracingDescriptorCounts()[m_set]);
	}

	for (const auto& param : params)
	{
		const uint32_t paramIndex = param.Index;

		VALIDATE_DESCRIPTOR(!param.Name.empty() || (paramIndex != std::numeric_limits<uint32_t>::max()),
		                    "DescriptorData has nullptr name and invalid index");

		const RendererAPI::DescriptorInfo* const desc = (paramIndex != std::numeric_limits<uint32_t>::max()) ?
		                                                (&rootSignature->GetDescriptors()[paramIndex]) :
												        rootSignature->GetDescriptor(param.Name.c_str());
		if(paramIndex != std::numeric_limits<uint32_t>::max())
		{
			VALIDATE_DESCRIPTOR(desc, "Invalid descriptor with param index ", paramIndex);
		}
		else
		{
			VALIDATE_DESCRIPTOR(desc, "Invalid descriptor with param name ", param.Name.c_str());
		}

		const RendererAPI::DescriptorType type = desc->Type;
		const uint32_t arrayCount = TRAP::Math::Max(1U, param.Count);

		VALIDATE_DESCRIPTOR(desc->Set == m_set, fmt::format("Descriptor {} - Mismatching set index", desc->Name));

		switch(type)
		{
		case RendererAPI::DescriptorType::Sampler:
		{
			//Index is invalid when descriptor is a static sampler
			VALIDATE_DESCRIPTOR(desc->IndexInParent != std::numeric_limits<uint32_t>::max(),
			                    fmt::format("Trying to update a static sampler ({}). All static samplers must be "
								            "set in RootSignature constructor and cannot be updated later", desc->Name));

			const std::vector<Sampler*>& samplers = std::get<std::vector<Sampler*>>(param.Resource);
			VALIDATE_DESCRIPTOR(!samplers.empty(), fmt::format("Empty Sampler ({})", desc->Name));

			for(uint32_t arr = 0; arr < arrayCount; ++arr)
			{
				VALIDATE_DESCRIPTOR(samplers[arr], fmt::format("nullptr Sampler ({} [{}])", desc->Name, arr));

				updateData[NumericCast<std::size_t>(desc->HandleIndex) + arr].ImageInfo =
				{
					dynamic_cast<VulkanSampler*>(samplers[arr])->GetVkSampler(),
					VK_NULL_HANDLE,
					VK_IMAGE_LAYOUT_UNDEFINED
				};
				update = true;
			}
			break;
		}

		case RendererAPI::DescriptorType::CombinedImageSampler:
		{
			const std::vector<Ref<TRAP::Graphics::Texture>>& textures = std::get<std::vector<Ref<TRAP::Graphics::Texture>>>(param.Resource);
			VALIDATE_DESCRIPTOR(!textures.empty(), fmt::format("Empty Texture ({})", desc->Name));

			if(!m_rootSignature->GetDescriptorNameToIndexMap().contains(desc->Name))
			{
				TP_ERROR(Log::RendererVulkanDescriptorSetPrefix, "No Static Sampler called (", desc->Name, ")");
				TRAP_ASSERT(false, "VulkanDescriptorSet::Update(): No Static Sampler called (" + desc->Name + ")");
			}

			for(uint32_t arr = 0; arr < arrayCount; ++arr)
			{
				VALIDATE_DESCRIPTOR(textures[arr], fmt::format("nullptr Texture ({} [{}])", desc->Name, arr));

				updateData[NumericCast<std::size_t>(desc->HandleIndex) + arr].ImageInfo =
				{
					nullptr, //Sampler
					std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(textures[arr])->GetSRVVkImageView(), //Image View
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL //Image Layout
				};

				update = true;
			}
			break;
		}

		case RendererAPI::DescriptorType::Texture:
		{
			const std::vector<Ref<TRAP::Graphics::Texture>>& textures = std::get<std::vector<Ref<TRAP::Graphics::Texture>>>(param.Resource);
			VALIDATE_DESCRIPTOR(!textures.empty(), fmt::format("Empty Texture ({})", desc->Name));

			if(!std::get<bool>(param.Offset))
			{
				for(uint32_t arr = 0; arr < arrayCount; ++arr)
				{
					VALIDATE_DESCRIPTOR(textures[arr], fmt::format("nullptr Texture ({} [{}])", desc->Name, arr));

					updateData[NumericCast<std::size_t>(desc->HandleIndex) + arr].ImageInfo =
					{
						VK_NULL_HANDLE, //Sampler
						std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(textures[arr])->GetSRVVkImageView(), //Image View
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL //Image Layout
					};

					update = true;
				}
			}
			else
			{
				for(uint32_t arr = 0; arr < arrayCount; ++arr)
				{
					VALIDATE_DESCRIPTOR(textures[arr], fmt::format("nullptr Texture ({} [{}])", desc->Name, arr));

					updateData[NumericCast<std::size_t>(desc->HandleIndex) + arr].ImageInfo =
					{
						VK_NULL_HANDLE, //Sampler
						std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(textures[arr])->GetSRVStencilVkImageView(), //Image View
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL //Image Layout
					};

					update = true;
				}
			}
			break;
		}

		case RendererAPI::DescriptorType::RWTexture:
		{
			const std::vector<Ref<TRAP::Graphics::Texture>>& textures = std::get<std::vector<Ref<TRAP::Graphics::Texture>>>(param.Resource);
			VALIDATE_DESCRIPTOR(!textures.empty(), fmt::format("Empty RW Texture ({})", desc->Name));

			if(std::get<TRAP::Graphics::RendererAPI::DescriptorData::TextureSlice>(param.Offset).BindMipChain)
			{
				VALIDATE_DESCRIPTOR(textures[0], fmt::format("nullptr RW Texture ({})", desc->Name));

				for (uint32_t arr = 0; arr < arrayCount; ++arr)
				{
					updateData[NumericCast<std::size_t>(desc->HandleIndex) + arr].ImageInfo =
					{
						VK_NULL_HANDLE, //Sampler
						std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(textures[0])->GetUAVVkImageViews()[arr], //Image View
						VK_IMAGE_LAYOUT_GENERAL //Image Layout
					};

					update = true;
				}
			}
			else
			{
				const uint32_t mipSlice = std::get<TRAP::Graphics::RendererAPI::DescriptorData::TextureSlice>
					(
						param.Offset
					).UAVMipSlice;

				for(uint32_t arr = 0; arr < arrayCount; ++arr)
				{
					VALIDATE_DESCRIPTOR(textures[arr], fmt::format("nullptr RW Texture ({} [{}])", desc->Name, arr));
					VALIDATE_DESCRIPTOR(mipSlice < textures[arr]->GetMipLevels(),
					                    fmt::format("Descriptor: ({} [{}]) Mip Slice ({}) exceeds mip levels ({})",
										desc->Name, arr, mipSlice, textures[arr]->GetMipLevels()));

					updateData[NumericCast<std::size_t>(desc->HandleIndex) + arr].ImageInfo =
					{
						VK_NULL_HANDLE, //Sampler
						std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(textures[arr])->GetUAVVkImageViews()[mipSlice], //Image View
						VK_IMAGE_LAYOUT_GENERAL //Image Layout
					};

					update = true;
				}
			}
			break;
		}

		case RendererAPI::DescriptorType::UniformBuffer:
		{
			if(desc->VkType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
			{
				const std::vector<Buffer*>& buffers = std::get<std::vector<Buffer*>>(param.Resource);
				const RendererAPI::DescriptorData::BufferOffset& off = std::get<RendererAPI::DescriptorData::BufferOffset>(param.Offset);
				VALIDATE_DESCRIPTOR(!buffers.empty(), fmt::format("Empty Uniform Buffer ({})", desc->Name));
				VALIDATE_DESCRIPTOR(buffers[0], fmt::format("nullptr Uniform Buffer ({} [0])", desc->Name));
				VALIDATE_DESCRIPTOR(arrayCount == 1, fmt::format("Descriptor ({}): VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC does not support arrays", desc->Name));
				VALIDATE_DESCRIPTOR(!off.Sizes.empty(), fmt::format("Descriptor ({}): Must provide Sizes for VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC", desc->Name));
				VALIDATE_DESCRIPTOR(off.Sizes[0] > 0, fmt::format("Descriptor ({}) - Sizes[0] is zero", desc->Name));
				VALIDATE_DESCRIPTOR(off.Sizes[0] <= RendererAPI::GPUSettings.MaxUniformBufferRange,
					                fmt::format("Descriptor ({}) - Sizes[0] is {} which exceeds max size {}",
									            desc->Name, off.Sizes[0], RendererAPI::GPUSettings.MaxUniformBufferRange));

				m_dynamicSizeOffsets[index].Offset = !off.Offsets.empty() ? NumericCast<uint32_t>(off.Offsets[0]) : 0u;
				const VulkanBuffer* const buf = dynamic_cast<VulkanBuffer*>(buffers[0]);
				updateData[NumericCast<std::size_t>(desc->HandleIndex) + 0u].BufferInfo =
				{
					buf->GetVkBuffer(),
					buf->GetOffset(),
					off.Sizes[0]
				};

				//If this is a different size we have to update the VkDescriptorBufferInfo::range so a call to
				//vkUpdateDescriptorSet is necessary
				if (off.Sizes[0] != m_dynamicSizeOffsets[index].Size)
				{
					m_dynamicSizeOffsets[index].Size = NumericCast<uint32_t>(off.Sizes[0]);
					update = true;
				}
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
			const std::vector<Buffer*>& buffers = std::get<std::vector<Buffer*>>(param.Resource);
			VALIDATE_DESCRIPTOR(!buffers.empty(), fmt::format("Empty Buffer ({})", desc->Name));

			for (uint32_t arr = 0; arr < arrayCount; ++arr)
			{
				VALIDATE_DESCRIPTOR(buffers[arr], fmt::format("nullptr Buffer ({} [{}])", desc->Name, arr));

				const VulkanBuffer* const buf = dynamic_cast<VulkanBuffer*>(buffers[arr]);
				updateData[NumericCast<std::size_t>(desc->HandleIndex) + arr].BufferInfo =
				{
					buf->GetVkBuffer(),
					buf->GetOffset(),
					VK_WHOLE_SIZE
				};

				const RendererAPI::DescriptorData::BufferOffset& off = std::get<RendererAPI::DescriptorData::BufferOffset>(param.Offset);
				if (!off.Offsets.empty())
				{
					VALIDATE_DESCRIPTOR(!off.Sizes.empty(), fmt::format("Descriptor ({}) - Sizes must be provided with Offsets", desc->Name));
					VALIDATE_DESCRIPTOR(off.Sizes[arr] > 0, fmt::format("Descriptor ({}) - Sizes[{}] is zero", desc->Name, arr));
					VALIDATE_DESCRIPTOR(off.Sizes[arr] <= RendererAPI::GPUSettings.MaxStorageBufferRange,
						                fmt::format("Descriptor ({}) - Sizes[{}] is {} which exceeds max size {}",
										            desc->Name, arr, off.Sizes[arr], RendererAPI::GPUSettings.MaxStorageBufferRange));

					updateData[NumericCast<std::size_t>(desc->HandleIndex) + arr].BufferInfo.offset = off.Offsets[arr];
					updateData[NumericCast<std::size_t>(desc->HandleIndex) + arr].BufferInfo.range = off.Sizes[arr];
				}

				update = true;
			}

		}
			break;
		}

		case RendererAPI::DescriptorType::TexelBuffer:
		{
			const std::vector<Buffer*>& buffers = std::get<std::vector<Buffer*>>(param.Resource);
			VALIDATE_DESCRIPTOR(!buffers.empty(), fmt::format("Empty Texel Buffer ({})", desc->Name));

			for(uint32_t arr = 0; arr < arrayCount; ++arr)
			{
				VALIDATE_DESCRIPTOR(buffers[arr], fmt::format("nullptr Texel Buffer ({} [{}])", desc->Name, arr));
				updateData[NumericCast<std::size_t>(desc->HandleIndex) + arr].BufferView = dynamic_cast<VulkanBuffer*>
					(
						buffers[arr]
					)->GetUniformTexelView();
				update = true;
			}

			break;
		}

		case RendererAPI::DescriptorType::RWTexelBuffer:
		{
			const std::vector<Buffer*>& buffers = std::get<std::vector<Buffer*>>(param.Resource);
			VALIDATE_DESCRIPTOR(!buffers.empty(), fmt::format("Empty RW Texel Buffer ({})", desc->Name));

			for (uint32_t arr = 0; arr < arrayCount; ++arr)
			{
				VALIDATE_DESCRIPTOR(buffers[arr], fmt::format("nullptr RW Texel Buffer ({} [{}])", desc->Name, arr));
				updateData[NumericCast<std::size_t>(desc->HandleIndex) + arr].BufferView = dynamic_cast<VulkanBuffer*>
					(
						buffers[arr]
					)->GetStorageTexelView();
				update = true;
			}

			break;
		}

		case RendererAPI::DescriptorType::RayTracing:
		{
			//TODO RT
			break;
		}

		default:
			break;
		}
	}

	//If this was called to just update a dynamic offset skip the update
	if (update)
		vkUpdateDescriptorSetWithTemplate(m_device->GetVkDevice(), m_vkDescriptorSetHandles[index],
		                                  rootSignature->GetUpdateTemplates()[m_set],
										  updateData.data());

	//RayTracing Update Descriptor Set since it does not support update template
	if (rayTracingWriteCount != 0u)
		vkUpdateDescriptorSets(m_device->GetVkDevice(), rayTracingWriteCount, rayTracingWrites.data(), 0, nullptr);
}