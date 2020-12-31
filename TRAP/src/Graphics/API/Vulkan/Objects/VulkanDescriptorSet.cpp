#include "TRAPPCH.h"
#include "VulkanDescriptorSet.h"

#include "VulkanBuffer.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanTexture.h"
#include "VulkanSampler.h"
#include "VulkanRootSignature.h"

TRAP::Graphics::API::VulkanDescriptorSet::~VulkanDescriptorSet()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDescriptorSetPrefix, "Destroying DescriptorSet");
#endif	
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDescriptorSet::VulkanDescriptorSet(TRAP::Ref<VulkanDevice> device, 
	std::vector<VkDescriptorSet> vkDescriptorSetHandles,
	TRAP::Ref<VulkanRootSignature> rootSignature,
	std::vector<std::vector<VulkanRenderer::DescriptorUpdateData>> updateData,
	std::vector<VulkanRenderer::SizeOffset> dynamicSizeOffsets,
	const uint32_t maxSets,
	const uint8_t dynamicOffsetCount,
	const RendererAPI::DescriptorUpdateFrequency updateFrequency)
		: m_vkDescriptorSetHandles(std::move(vkDescriptorSetHandles)),
		  m_rootSignature(std::move(rootSignature)),
		  m_updateData(std::move(updateData)),
		  m_dynamicSizeOffsets(std::move(dynamicSizeOffsets)),
		  m_maxSets(maxSets),
		  m_dynamicOffsetCount(dynamicOffsetCount),
		  m_updateFrequency(updateFrequency),
	      m_device(std::move(device))
{
	TRAP_ASSERT(m_rootSignature, "rootSignature is nullptr");
	
#ifdef ENABLE_GRAPHICS_DEBUG
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
	return m_updateFrequency;
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

void TRAP::Graphics::API::VulkanDescriptorSet::Update(uint32_t index, uint32_t count, const std::vector<RendererAPI::DescriptorData>& params)
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
	RendererAPI::DescriptorUpdateFrequency updateFreq = m_updateFrequency;
	std::vector<VulkanRenderer::DescriptorUpdateData>& updateData = m_updateData[index];
	bool update = false;

	std::vector<VkWriteDescriptorSet> rayTracingWrites;
	std::vector<VkWriteDescriptorSetAccelerationStructureKHR> rayTracingWritesKHR;
	uint32_t rayTracingWriteCount = 0;

	if(rootSignature->GetVkRayTracingDescriptorCounts()[static_cast<uint32_t>(updateFreq)])
	{
		rayTracingWrites.resize(rootSignature->GetVkRayTracingDescriptorCounts()[static_cast<uint32_t>(updateFreq)]);
		rayTracingWritesKHR.resize(rootSignature->GetVkRayTracingDescriptorCounts()[static_cast<uint32_t>(updateFreq)]);
	}

	for(uint32_t i = 0; i < count; ++i)
	{
		const RendererAPI::DescriptorData& param = params[i];
		uint32_t paramIndex = param.Index;

		VALIDATE_DESCRIPTOR(param.Name || (paramIndex != -1), "DescriptorData has nullptr name and invalid index");

		const RendererAPI::DescriptorInfo* desc = (paramIndex != -1) ? (&rootSignature->GetDescriptors()[paramIndex]) : rootSignature->GetDescriptor(param.Name);
		if(paramIndex != -1)
		{
			VALIDATE_DESCRIPTOR(desc, "Invalid descriptor with param index ", paramIndex);
		}
		else
		{
			VALIDATE_DESCRIPTOR(desc, "Invalid descriptor with param name ", param.Name);
		}

		const RendererAPI::DescriptorType type = desc->Type;
		const uint32_t arrayCount = TRAP::Math::Max(1U, param.Count);

		VALIDATE_DESCRIPTOR(desc->UpdateFrequency == updateFreq, std::string("Descriptor ") + std::string(desc->Name) + std::string(" - Mismatching update frequency and set index"));

		switch(type)
		{
		case RendererAPI::DescriptorType::Sampler:
		{
			//Index is invalid when descriptor is a static sampler
			VALIDATE_DESCRIPTOR(desc->IndexInParent != -1,
				std::string("Trying to update a static sampler (") + std::string(desc->Name) + "). All static samplers must be set in RootSignature constructor and cannot be updated later");

			VALIDATE_DESCRIPTOR(!param.Samplers.empty(), std::string("Empty Sampler (" + std::string(desc->Name) + ")"));

			for(uint32_t arr = 0; arr < arrayCount; ++arr)
			{
				VALIDATE_DESCRIPTOR(param.Samplers[arr], std::string("nullptr Sampler (") + std::string(desc->Name) + std::string(" [") + std::to_string(arr) + std::string("])"));

				updateData[desc->HandleIndex + arr].ImageInfo = { param.Samplers[arr]->GetVkSampler(), VK_NULL_HANDLE };
				update = true;
			}
			break;
		}

		case RendererAPI::DescriptorType::Texture:
		{
			VALIDATE_DESCRIPTOR(!param.Textures.empty(), std::string("Empty Texture (") + std::string(desc->Name) + std::string(")"));

			if(!param.BindStencilResource)
			{
				for(uint32_t arr = 0; arr < arrayCount; ++arr)
				{
					VALIDATE_DESCRIPTOR(param.Textures[arr], std::string("nullptr Texture (") + std::string(desc->Name) + std::string(" [") + std::to_string(arr) + std::string("])"));

					updateData[desc->HandleIndex + arr].ImageInfo =
					{
						VK_NULL_HANDLE,                           //Sampler
						param.Textures[arr]->GetSRVVkImageView(), //Image View
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL  //Image Layout
					};

					update = true;
				}
			}
			else
			{
				for(uint32_t arr = 0; arr < arrayCount; ++arr)
				{
					VALIDATE_DESCRIPTOR(param.Textures[arr], std::string("nullptr Texture (") + std::string(desc->Name) + std::string(" [") + std::to_string(arr) + std::string("])"));

					updateData[desc->HandleIndex + arr].ImageInfo =
					{
						VK_NULL_HANDLE,                                  //Sampler
						param.Textures[arr]->GetSRVStencilVkImageView(), //Image View
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL         //Image Layout
					};

					update = true;
				}
			}
			break;
		}

		case RendererAPI::DescriptorType::RWTexture:
		{
			VALIDATE_DESCRIPTOR(!param.Textures.empty(), std::string("Empty RW Texture (") + std::string(desc->Name) + std::string(")"));
			const uint32_t mipSlice = param.UAVMipSlice;

			for(uint32_t arr = 0; arr < arrayCount; ++arr)
			{
				VALIDATE_DESCRIPTOR(param.Textures[arr], std::string("nullptr RW Texture (") + std::string(desc->Name) + std::string(" [") + std::to_string(arr) + std::string("])"));
				VALIDATE_DESCRIPTOR(mipSlice < param.Textures[arr]->GetMipLevels(),
				                    std::string("Descriptor: (") + std::string(desc->Name) + std::string(" [") + std::to_string(arr) +
									std::string("]) Mip Slice (") + std::to_string(mipSlice) + std::string(") exceeds mip levels (") +
									std::to_string(param.Textures[arr]->GetMipLevels()) + std::string(")"));

				updateData[desc->HandleIndex + arr].ImageInfo =
				{
					VK_NULL_HANDLE,                                      //Sampler
					param.Textures[arr]->GetUAVVkImageViews()[mipSlice], //Image View
					VK_IMAGE_LAYOUT_GENERAL                              //Image Layout
				};

				update = true;
			}
			break;
		}

		case RendererAPI::DescriptorType::UniformBuffer:
		{
			if(desc->VkType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
			{
				VALIDATE_DESCRIPTOR(!param.Buffers.empty(), std::string("Empty Uniform Buffer (") + std::string(desc->Name) + std::string(")"));
				VALIDATE_DESCRIPTOR(param.Buffers[0], std::string("nullptr Uniform Buffer (") + std::string(desc->Name) + std::string(" [0])"));
				VALIDATE_DESCRIPTOR(arrayCount == 1, std::string("Descriptor (") + std::string(desc->Name) + std::string("): VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC does not support arrays"));
				VALIDATE_DESCRIPTOR(!param.Sizes.empty(), std::string("Descriptor (") + std::string(desc->Name) + std::string("): Must provide Sizes for VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC"));
				VALIDATE_DESCRIPTOR(param.Sizes[0] > 0, std::string("Descriptor (") + std::string(desc->Name) + std::string(") - Sizes[0] is zero"));
				VALIDATE_DESCRIPTOR(param.Sizes[0] <= m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties().limits.maxUniformBufferRange,
					std::string("Descriptor (") + std::string(desc->Name) + std::string(") - Sizes[0] is " + std::to_string(param.Sizes[0]) +
					std::string(" which exceeds max size ") + std::to_string(m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties().limits.maxUniformBufferRange)));

				m_dynamicSizeOffsets[index].Offset = !param.Offsets.empty() ? static_cast<uint32_t>(param.Offsets[0]) : 0;
				updateData[desc->HandleIndex + 0].BufferInfo =
				{
					param.Buffers[0]->GetVkBuffer(),
					param.Buffers[0]->GetOffset(),
					param.Sizes[0]
				};

				//If this is a different size we have to update the VkDescriptorBufferInfo::range so a call to vkUpdateDescriptorSet is necessary
				if (param.Sizes[0] != static_cast<uint32_t>(m_dynamicSizeOffsets[index].Size))
				{
					m_dynamicSizeOffsets[index].Size = static_cast<uint32_t>(param.Sizes[0]);
					update = true;
				}

			}
			break;
		}

		case RendererAPI::DescriptorType::Buffer:
		case RendererAPI::DescriptorType::BufferRaw:
		case RendererAPI::DescriptorType::RWBuffer:
		case RendererAPI::DescriptorType::RWBufferRaw:
		{
			VALIDATE_DESCRIPTOR(!param.Buffers.empty(), std::string("Empty Buffer (") + std::string(desc->Name));

			for(uint32_t arr = 0; arr < arrayCount; ++arr)
			{
				VALIDATE_DESCRIPTOR(param.Buffers[arr], std::string("nullptr Buffer (") + std::string(desc->Name) + std::string(" [") + std::to_string(arr) + std::string("])"));

				updateData[desc->HandleIndex + arr].BufferInfo =
				{
					param.Buffers[arr]->GetVkBuffer(),
					param.Buffers[arr]->GetOffset(),
					VK_WHOLE_SIZE
				};

				if(!param.Offsets.empty())
				{
					VALIDATE_DESCRIPTOR(!param.Sizes.empty(), std::string("Descriptor (") + std::string(desc->Name) + std::string(") - Sizes must be provided with Offsets"));
					VALIDATE_DESCRIPTOR(param.Sizes[arr] > 0, std::string("Descriptor (") + std::string(desc->Name) + std::string(") - Sizes[") + std::to_string(arr) + std::string("] is zero"));
					VALIDATE_DESCRIPTOR(param.Sizes[arr] <= m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties().limits.maxUniformBufferRange,
						std::string("Descriptor (") + std::string(desc->Name) + std::string(") - Sizes[") + std::to_string(arr) + std::string("] is ") +
						std::to_string(param.Sizes[arr]) + std::string(" which exceeds max size ") +
						std::to_string(m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties().limits.maxUniformBufferRange));

					updateData[desc->HandleIndex + arr].BufferInfo.offset = param.Offsets[arr];
					updateData[desc->HandleIndex + arr].BufferInfo.range = param.Sizes[arr];
				}

				update = true;
			}
			
			break;
		}

		case RendererAPI::DescriptorType::TexelBuffer:
		{
			VALIDATE_DESCRIPTOR(!param.Buffers.empty(), std::string("Empty Texel Buffer (") + std::string(desc->Name) + std::string(")"));

			for(uint32_t arr = 0; arr < arrayCount; ++arr)
			{
				VALIDATE_DESCRIPTOR(param.Buffers[arr], std::string("nullptr Texel Buffer (") + std::string(desc->Name) + std::string(" [") + std::to_string(arr) + std::string("])"));
				updateData[desc->HandleIndex + arr].BufferView = param.Buffers[arr]->GetUniformTexelView();
				update = true;
			}
			
			break;
		}

		case RendererAPI::DescriptorType::RWTexelBuffer:
		{
			VALIDATE_DESCRIPTOR(!param.Buffers.empty(), std::string("Empty RW Texel Buffer (") + std::string(desc->Name) + std::string(")"));

			for (uint32_t arr = 0; arr < arrayCount; ++arr)
			{
				VALIDATE_DESCRIPTOR(param.Buffers[arr], std::string("nullptr RW Texel Buffer (") + std::string(desc->Name) + std::string(" [") + std::to_string(arr) + std::string("])"));
				updateData[desc->HandleIndex + arr].BufferView = param.Buffers[arr]->GetStorageTexelView();
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
		vkUpdateDescriptorSetWithTemplate(m_device->GetVkDevice(), m_vkDescriptorSetHandles[index], rootSignature->GetUpdateTemplates()[static_cast<uint32_t>(updateFreq)], updateData.data());

	//RayTracing Update Descriptor Set since it does not support update template
	if (rayTracingWriteCount)
		vkUpdateDescriptorSets(m_device->GetVkDevice(), rayTracingWriteCount, rayTracingWrites.data(), 0, nullptr);
}