#include "TRAPPCH.h"
#include "VulkanDescriptorPool.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

std::array<VkDescriptorPoolSize, TRAP::Graphics::API::VulkanDescriptorPool::DESCRIPTOR_TYPE_RANGE_SIZE>
TRAP::Graphics::API::VulkanDescriptorPool::s_descriptorPoolSizes =
{
	{
		{VK_DESCRIPTOR_TYPE_SAMPLER, 1024},
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
		{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 8192},
		{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1024},
		{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1024},
		{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1024},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 8192},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1024},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1024},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1},
		{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1}
	}
};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDescriptorPool::VulkanDescriptorPool(TRAP::Ref<VulkanDevice> device, const uint32_t numDescriptorSets)
	: m_descriptorPool(VK_NULL_HANDLE),
	  m_descriptorPoolSizes(DescriptorTypeRangeSize),
	  m_numDescriptorSets(numDescriptorSets),
	  m_usedDescriptorSetCount(0),
	  m_device(std::move(device))
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDescriptorPoolPrefix, "Creating DescriptorPool");
#endif

	if (VulkanRenderer::s_raytracingExtension)
		s_descriptorPoolSizes[DESCRIPTOR_TYPE_RANGE_SIZE - 1] = { VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1024 };
	
	for(uint32_t i = 0; i < DescriptorTypeRangeSize; i++)
		m_descriptorPoolSizes[i] = s_descriptorPoolSizes[i];

	VkDescriptorPoolCreateInfo info = VulkanInits::DescriptorPoolCreateInfo(m_descriptorPoolSizes, m_numDescriptorSets);
	VkCall(vkCreateDescriptorPool(m_device->GetVkDevice(), &info, nullptr, &m_descriptorPool));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDescriptorPool::~VulkanDescriptorPool()
{
	if(m_descriptorPool)
	{
#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::RendererVulkanDescriptorPoolPrefix, "Destroying DescriptorPool");
#endif
		vkDestroyDescriptorPool(m_device->GetVkDevice(), m_descriptorPool, nullptr);
		m_descriptorPool = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDescriptorPool::Reset()
{
	VkCall(vkResetDescriptorPool(m_device->GetVkDevice(), m_descriptorPool, 0));
	m_usedDescriptorSetCount = 0;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDescriptorPool& TRAP::Graphics::API::VulkanDescriptorPool::GetVkDescriptorPool()
{
	return m_descriptorPool;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkDescriptorPoolSize>& TRAP::Graphics::API::VulkanDescriptorPool::GetDescriptorPoolSizes() const
{
	return m_descriptorPoolSizes;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanDescriptorPool::GetDescriptorSetsNum() const
{
	return m_numDescriptorSets;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanDescriptorPool::GetUsedDescriptorSetsCount() const
{
	return m_usedDescriptorSetCount;
}
