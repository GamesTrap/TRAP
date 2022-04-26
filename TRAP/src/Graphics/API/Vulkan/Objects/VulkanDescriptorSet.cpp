#include "TRAPPCH.h"
#include "VulkanDescriptorSet.h"

#include "VulkanBuffer.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanSampler.h"
#include "VulkanRootSignature.h"
#include "VulkanTexture.h"
#include "Graphics/Textures/TextureBase.h"

TRAP::Graphics::API::VulkanDescriptorSet::~VulkanDescriptorSet()
{
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDescriptorSetPrefix, "Destroying DescriptorSet");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDescriptorSet::VulkanDescriptorSet(TRAP::Ref<VulkanDevice> device,
	                                                          std::vector<VkDescriptorSet> vkDescriptorSetHandles,
	                                                          TRAP::Ref<VulkanRootSignature> rootSignature,
	                                                          std::vector<std::vector<VulkanRenderer::DescriptorUpdateData>> updateData,
	                                                          const uint32_t maxSets, const uint8_t dynamicOffsetCount,
	                                                          const uint32_t set)
		: m_vkDescriptorSetHandles(std::move(vkDescriptorSetHandles)),
		  m_rootSignature(std::move(rootSignature)),
		  m_updateData(std::move(updateData)),
		  m_dynamicSizeOffsets(static_cast<std::size_t>(dynamicOffsetCount) * maxSets),
		  m_maxSets(maxSets),
		  m_dynamicOffsetCount(dynamicOffsetCount),
		  m_set(set),
	      m_device(std::move(device))
{
	TRAP_ASSERT(m_rootSignature, "rootSignature is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDescriptorSetPrefix, "Creating DescriptorSet");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkDescriptorSet>& TRAP::Graphics::API::VulkanDescriptorSet::GetVkDescriptorSets() const
{
	return m_vkDescriptorSetHandles;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::API::VulkanRootSignature> TRAP::Graphics::API::VulkanDescriptorSet::GetRootSignature() const
{
	return m_rootSignature;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::DescriptorUpdateFrequency TRAP::Graphics::API::VulkanDescriptorSet::GetUpdateFrequency() const
{
	return m_set > 0 ? RendererAPI::DescriptorUpdateFrequency::Dynamic :
	                   RendererAPI::DescriptorUpdateFrequency::Static;
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::Graphics::API::VulkanDescriptorSet::GetDynamicOffsetCount() const
{
	return m_dynamicOffsetCount;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Graphics::API::VulkanRenderer::SizeOffset> TRAP::Graphics::API::VulkanDescriptorSet::GetDynamicSizeOffsets() const
{
	return m_dynamicSizeOffsets;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanDescriptorSet::GetMaxSets() const
{
	return m_maxSets;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanDescriptorSet::GetSet() const
{
	return m_set;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDescriptorSet::Update(uint32_t index,
                                                      const std::vector<RendererAPI::DescriptorData>& params)
{
#ifdef ENABLE_GRAPHICS_DEBUG
#define VALIDATE_DESCRIPTOR(descriptor, ...)                                            \
	if(!(descriptor))                                                                   \
	{                                                                                   \
		std::string msg = __FUNCTION__ + std::string(" : ") + std::string(__VA_ARGS__); \
		TP_ERROR(Log::RendererVulkanDescriptorSetPrefix, msg);                          \
		TRAP_ASSERT(false, msg);                                                        \
		continue;                                                                       \
	}
#else
#define VALIDATE_DESCRIPTOR(descriptor, ...)
#endif

	TRAP_ASSERT(!m_vkDescriptorSetHandles.empty());
	TRAP_ASSERT(index < m_maxSets);

	const TRAP::Ref<VulkanRootSignature>& rootSignature = m_rootSignature;
	std::vector<VulkanRenderer::DescriptorUpdateData>& updateData = m_updateData[index];
	bool update = false;

	std::vector<VkWriteDescriptorSet> rayTracingWrites;
	std::vector<VkWriteDescriptorSetAccelerationStructureKHR> rayTracingWritesKHR;
	uint32_t rayTracingWriteCount = 0;

	if(rootSignature->GetVkRayTracingDescriptorCounts()[m_set])
	{
		rayTracingWrites.resize(rootSignature->GetVkRayTracingDescriptorCounts()[m_set]);
		rayTracingWritesKHR.resize(rootSignature->GetVkRayTracingDescriptorCounts()[m_set]);
	}

	for (const auto& param : params)
	{
		uint32_t paramIndex = param.Index;

		VALIDATE_DESCRIPTOR(param.Name || (paramIndex != std::numeric_limits<uint32_t>::max()),
		                    "DescriptorData has nullptr name and invalid index");

		const RendererAPI::DescriptorInfo* desc = (paramIndex != std::numeric_limits<uint32_t>::max()) ?
		                                          (&rootSignature->GetDescriptors()[paramIndex]) :
												  rootSignature->GetDescriptor(param.Name);
		if(paramIndex != std::numeric_limits<uint32_t>::max())
		{
			VALIDATE_DESCRIPTOR(desc, "Invalid descriptor with param index ", paramIndex);
		}
		else
		{
			VALIDATE_DESCRIPTOR(desc, "Invalid descriptor with param name ", param.Name);
		}

		const RendererAPI::DescriptorType type = desc->Type;
		const uint32_t arrayCount = TRAP::Math::Max(1U, param.Count);

		VALIDATE_DESCRIPTOR(desc->Set == m_set, std::string("Descriptor ") + desc->Name +
		                    " - Mismatching set index");

		switch(type)
		{
		case RendererAPI::DescriptorType::Sampler:
		{
			//Index is invalid when descriptor is a static sampler
			VALIDATE_DESCRIPTOR(desc->IndexInParent != std::numeric_limits<uint32_t>::max(),
				                std::string("Trying to update a static sampler (") + desc->Name +
								"). All static samplers must be set in RootSignature constructor and cannot be "
								"updated later");

			const std::vector<Sampler*>& samplers = std::get<std::vector<Sampler*>>(param.Resource);
			VALIDATE_DESCRIPTOR(!samplers.empty(), std::string("Empty Sampler (") + desc->Name + ")");

			for(uint32_t arr = 0; arr < arrayCount; ++arr)
			{
				VALIDATE_DESCRIPTOR(samplers[arr], std::string("nullptr Sampler (") + desc->Name +
				                    std::string(" [") + std::to_string(arr) + "])");

				updateData[desc->HandleIndex + static_cast<std::size_t>(arr)].ImageInfo =
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
			const std::vector<TRAP::Graphics::TextureBase*>& textures = std::get<std::vector<TRAP::Graphics::TextureBase*>>(param.Resource);
			VALIDATE_DESCRIPTOR(!textures.empty(), std::string("Empty Texture (") + desc->Name + ")");

			std::unordered_map<std::string, uint32_t>::const_iterator it = m_rootSignature->GetDescriptorNameToIndexMap().find(desc->Name);
			if(it == m_rootSignature->GetDescriptorNameToIndexMap().end())
			{
				TP_ERROR(Log::RendererVulkanDescriptorSetPrefix, "No Static Sampler called (", desc->Name, ")");
				TRAP_ASSERT(false);
			}

			for(uint32_t arr = 0; arr < arrayCount; ++arr)
			{
				VALIDATE_DESCRIPTOR(textures[arr], std::string("nullptr Texture (") + desc->Name +
				                    std::string(" [") + std::to_string(arr) + "])");

				updateData[desc->HandleIndex + static_cast<std::size_t>(arr)].ImageInfo =
				{
					nullptr, //Sampler
					dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(textures[arr])->GetSRVVkImageView(), //Image View
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL //Image Layout
				};

				update = true;
			}
			break;
		}

		case RendererAPI::DescriptorType::Texture:
		{
			const std::vector<TRAP::Graphics::TextureBase*>& textures = std::get<std::vector<TRAP::Graphics::TextureBase*>>(param.Resource);
			VALIDATE_DESCRIPTOR(!textures.empty(), std::string("Empty Texture (") + desc->Name + ")");

			if(!std::get<bool>(param.Offset))
			{
				for(uint32_t arr = 0; arr < arrayCount; ++arr)
				{
					VALIDATE_DESCRIPTOR(textures[arr], std::string("nullptr Texture (") + desc->Name +
					                    std::string(" [") + std::to_string(arr) + "])");

					updateData[desc->HandleIndex + static_cast<std::size_t>(arr)].ImageInfo =
					{
						VK_NULL_HANDLE, //Sampler
						dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(textures[arr])->GetSRVVkImageView(), //Image View
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL //Image Layout
					};

					update = true;
				}
			}
			else
			{
				for(uint32_t arr = 0; arr < arrayCount; ++arr)
				{
					VALIDATE_DESCRIPTOR(textures[arr], std::string("nullptr Texture (") + desc->Name +
					                    std::string(" [") + std::to_string(arr) + "])");

					updateData[desc->HandleIndex + static_cast<std::size_t>(arr)].ImageInfo =
					{
						VK_NULL_HANDLE, //Sampler
						dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(textures[arr])->GetSRVStencilVkImageView(), //Image View
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL //Image Layout
					};

					update = true;
				}
			}
			break;
		}

		case RendererAPI::DescriptorType::RWTexture:
		{
			const std::vector<TRAP::Graphics::TextureBase*>& textures = std::get<std::vector<TRAP::Graphics::TextureBase*>>(param.Resource);
			VALIDATE_DESCRIPTOR(!textures.empty(), std::string("Empty RW Texture (") + desc->Name + ")");

			if(std::get<TRAP::Graphics::RendererAPI::DescriptorData::TextureSlice>(param.Offset).BindMipChain)
			{
				VALIDATE_DESCRIPTOR(textures[0], std::string("nullptr RW Texture (") + desc->Name + ")");

				for (uint32_t arr = 0; arr < arrayCount; ++arr)
				{
					updateData[desc->HandleIndex + static_cast<std::size_t>(arr)].ImageInfo =
					{
						VK_NULL_HANDLE, //Sampler
						dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(textures[0])->GetUAVVkImageViews()[arr], //Image View
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
					VALIDATE_DESCRIPTOR(textures[arr], std::string("nullptr RW Texture (") +
					                    desc->Name + std::string(" [") + std::to_string(arr) + "])");
					VALIDATE_DESCRIPTOR(mipSlice < textures[arr]->GetMipLevels(), std::string("Descriptor: (") +
					                    desc->Name + std::string(" [") + std::to_string(arr) +
										std::string("]) Mip Slice (") + std::to_string(mipSlice) +
										std::string(") exceeds mip levels (") +
										std::to_string(textures[arr]->GetMipLevels()) + ")");

					updateData[desc->HandleIndex + static_cast<std::size_t>(arr)].ImageInfo =
					{
						VK_NULL_HANDLE, //Sampler
						dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(textures[arr])->GetUAVVkImageViews()[mipSlice], //Image View
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
				VALIDATE_DESCRIPTOR(!buffers.empty(), std::string("Empty Uniform Buffer (") + desc->Name + ")");
				VALIDATE_DESCRIPTOR(buffers[0], std::string("nullptr Uniform Buffer (") + desc->Name + " [0])");
				VALIDATE_DESCRIPTOR(arrayCount == 1, std::string("Descriptor (") + desc->Name +
				                    "): VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC does not support arrays");
				VALIDATE_DESCRIPTOR(!off.Sizes.empty(), std::string("Descriptor (") + desc->Name +
				                    "): Must provide Sizes for VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC");
				VALIDATE_DESCRIPTOR(off.Sizes[0] > 0, std::string("Descriptor (") + desc->Name +
				                    ") - Sizes[0] is zero");
				VALIDATE_DESCRIPTOR(off.Sizes[0] <= RendererAPI::GPUSettings.MaxUniformBufferRange,
					                std::string("Descriptor (") + desc->Name +
									std::string(") - Sizes[0] is " + std::to_string(off.Sizes[0]) +
									std::string(" which exceeds max size ") +
									std::to_string(RendererAPI::GPUSettings.MaxUniformBufferRange)));

				m_dynamicSizeOffsets[index].Offset = !off.Offsets.empty() ? static_cast<uint32_t>(off.Offsets[0]) : 0;
				VulkanBuffer* buf = dynamic_cast<VulkanBuffer*>(buffers[0]);
				updateData[desc->HandleIndex + static_cast<std::size_t>(0)].BufferInfo =
				{
					buf->GetVkBuffer(),
					buf->GetOffset(),
					off.Sizes[0]
				};

				//If this is a different size we have to update the VkDescriptorBufferInfo::range so a call to
				//vkUpdateDescriptorSet is necessary
				if (off.Sizes[0] != static_cast<uint64_t>(m_dynamicSizeOffsets[index].Size))
				{
					m_dynamicSizeOffsets[index].Size = static_cast<uint32_t>(off.Sizes[0]);
					update = true;
				}
				break;
			}

		case RendererAPI::DescriptorType::Buffer:
		case RendererAPI::DescriptorType::BufferRaw:
		case RendererAPI::DescriptorType::RWBuffer:
		case RendererAPI::DescriptorType::RWBufferRaw:
		{
			const std::vector<Buffer*>& buffers = std::get<std::vector<Buffer*>>(param.Resource);
			VALIDATE_DESCRIPTOR(!buffers.empty(), std::string("Empty Buffer (") + desc->Name);

			for (uint32_t arr = 0; arr < arrayCount; ++arr)
			{
				VALIDATE_DESCRIPTOR(buffers[arr], std::string("nullptr Buffer (") + desc->Name +
				                    std::string(" [") + std::to_string(arr) + "])");

				VulkanBuffer* buf = dynamic_cast<VulkanBuffer*>(buffers[arr]);
				updateData[desc->HandleIndex + static_cast<std::size_t>(arr)].BufferInfo =
				{
					buf->GetVkBuffer(),
					buf->GetOffset(),
					VK_WHOLE_SIZE
				};

				const RendererAPI::DescriptorData::BufferOffset& off = std::get<RendererAPI::DescriptorData::BufferOffset>(param.Offset);
				if (!off.Offsets.empty())
				{
					VALIDATE_DESCRIPTOR(!off.Sizes.empty(), std::string("Descriptor (") + desc->Name +
					                    ") - Sizes must be provided with Offsets");
					VALIDATE_DESCRIPTOR(off.Sizes[arr] > 0, std::string("Descriptor (") + desc->Name +
					                    std::string(") - Sizes[") + std::to_string(arr) + "] is zero");
					VALIDATE_DESCRIPTOR(off.Sizes[arr] <= RendererAPI::GPUSettings.MaxUniformBufferRange,
						                std::string("Descriptor (") + desc->Name +
										std::string(") - Sizes[") + std::to_string(arr) + std::string("] is ") +
						                std::to_string(off.Sizes[arr]) + std::string(" which exceeds max size ") +
						                std::to_string(RendererAPI::GPUSettings.MaxUniformBufferRange));

					updateData[desc->HandleIndex + static_cast<std::size_t>(arr)].BufferInfo.offset = off.Offsets[arr];
					updateData[desc->HandleIndex + static_cast<std::size_t>(arr)].BufferInfo.range = off.Sizes[arr];
				}

				update = true;
			}

		}
			break;
		}

		case RendererAPI::DescriptorType::TexelBuffer:
		{
			const std::vector<Buffer*>& buffers = std::get<std::vector<Buffer*>>(param.Resource);
			VALIDATE_DESCRIPTOR(!buffers.empty(), std::string("Empty Texel Buffer (") + desc->Name + ")");

			for(uint32_t arr = 0; arr < arrayCount; ++arr)
			{
				VALIDATE_DESCRIPTOR(buffers[arr], std::string("nullptr Texel Buffer (") + desc->Name +
				                    std::string(" [") + std::to_string(arr) + "])");
				updateData[desc->HandleIndex + static_cast<std::size_t>(arr)].BufferView = dynamic_cast<VulkanBuffer*>
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
			VALIDATE_DESCRIPTOR(!buffers.empty(), std::string("Empty RW Texel Buffer (") + desc->Name + ")");

			for (uint32_t arr = 0; arr < arrayCount; ++arr)
			{
				VALIDATE_DESCRIPTOR(buffers[arr], std::string("nullptr RW Texel Buffer (") +
				                    desc->Name + std::string(" [") + std::to_string(arr) + "])");
				updateData[desc->HandleIndex + static_cast<std::size_t>(arr)].BufferView = dynamic_cast<VulkanBuffer*>
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
	if (rayTracingWriteCount)
		vkUpdateDescriptorSets(m_device->GetVkDevice(), rayTracingWriteCount, rayTracingWrites.data(), 0, nullptr);
}