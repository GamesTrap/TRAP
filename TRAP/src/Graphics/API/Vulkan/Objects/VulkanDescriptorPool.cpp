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
	: m_currentPool(VK_NULL_HANDLE),
	  m_descriptorPoolSizes(DescriptorTypeRangeSize),
	  m_numDescriptorSets(numDescriptorSets),
	  m_usedDescriptorSetCount(0),
	  m_device(std::move(device))
{
	TRAP_ASSERT(m_device, "device is nullptr");
	
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDescriptorPoolPrefix, "Creating DescriptorPool");
#endif

	if (VulkanRenderer::s_raytracingExtension)
		s_descriptorPoolSizes[DESCRIPTOR_TYPE_RANGE_SIZE - 1] = { VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1024 };
	
	for(uint32_t i = 0; i < DescriptorTypeRangeSize; i++)
		m_descriptorPoolSizes[i] = s_descriptorPoolSizes[i];

	VkDescriptorPoolCreateInfo info = VulkanInits::DescriptorPoolCreateInfo(m_descriptorPoolSizes, m_numDescriptorSets);
	VkCall(vkCreateDescriptorPool(m_device->GetVkDevice(), &info, nullptr, &m_currentPool));
	
	m_descriptorPools.emplace_back(m_currentPool);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDescriptorPool::~VulkanDescriptorPool()
{
	if(!m_descriptorPools.empty())
	{
#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::RendererVulkanDescriptorPoolPrefix, "Destroying DescriptorPool");
#endif
		for(VkDescriptorPool& pool : m_descriptorPools)
			vkDestroyDescriptorPool(m_device->GetVkDevice(), pool, nullptr);
		m_descriptorPools.clear();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDescriptorPool::Reset()
{
	VkCall(vkResetDescriptorPool(m_device->GetVkDevice(), m_currentPool, 0));
	m_usedDescriptorSetCount = 0;
}

//-------------------------------------------------------------------------------------------------------------------//

VkDescriptorPool& TRAP::Graphics::API::VulkanDescriptorPool::GetCurrentVkDescriptorPool()
{
	return m_currentPool;
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

//-------------------------------------------------------------------------------------------------------------------//

VkDescriptorSet TRAP::Graphics::API::VulkanDescriptorPool::AllocateVkDescriptorSet(const VkDescriptorSetLayout layout)
{
	//Need a lock since vkAllocateDescriptorSets needs to be externally synchronized
	//This is fine since this will only happen during Init time
	{
		std::lock_guard<std::mutex> lockGuard(m_mutex);

		VkDescriptorSetAllocateInfo info = VulkanInits::DescriptorSetAllocateInfo(m_currentPool, layout);

		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
		VkResult res = vkAllocateDescriptorSets(m_device->GetVkDevice(), &info, &descriptorSet);
		if(res != VK_SUCCESS)
		{
			VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

			VkDescriptorPoolCreateInfo poolInfo = VulkanInits::DescriptorPoolCreateInfo(m_descriptorPoolSizes, m_numDescriptorSets);

			VkCall(vkCreateDescriptorPool(m_device->GetVkDevice(), &poolInfo, nullptr, &descriptorPool));

			m_descriptorPools.emplace_back(descriptorPool);

			m_currentPool = descriptorPool;
			m_usedDescriptorSetCount = 0;

			info.descriptorPool = m_currentPool;
			res = vkAllocateDescriptorSets(m_device->GetVkDevice(), &info, &descriptorSet);
		}

		TRAP_ASSERT(res == VK_SUCCESS);

		m_usedDescriptorSetCount++;

		return descriptorSet;
	}
}