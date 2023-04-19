#include "TRAPPCH.h"
#include "VulkanDescriptorPool.h"

#include "VulkanRootSignature.h"
#include "VulkanDescriptorSet.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"

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

TRAP::Graphics::API::VulkanDescriptorPool::VulkanDescriptorPool(const uint32_t numDescriptorSets)
	: m_currentPool(VK_NULL_HANDLE),
	  m_descriptorPoolSizes(DescriptorTypeRangeSize),
	  m_usedDescriptorSetCount(0),
	  m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice())
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	m_numDescriptorSets = numDescriptorSets;
	TRAP_ASSERT(m_device, "VulkanDescriptorPool(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDescriptorPoolPrefix, "Creating DescriptorPool");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if (VulkanRenderer::s_rayTracingExtension)
		std::get<DESCRIPTOR_TYPE_RANGE_SIZE - 1>(s_descriptorPoolSizes) = { VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
		                                                                    1024 };

	for(uint32_t i = 0; i < DescriptorTypeRangeSize; i++)
		m_descriptorPoolSizes[i] = s_descriptorPoolSizes[i];

	const VkDescriptorPoolCreateInfo info = VulkanInits::DescriptorPoolCreateInfo(m_descriptorPoolSizes,
	                                                                              m_numDescriptorSets);
	VkCall(vkCreateDescriptorPool(m_device->GetVkDevice(), &info, nullptr, &m_currentPool));
	TRAP_ASSERT(m_currentPool, "VulkanDescriptorPool(): Failed to create DescriptorPool");

	if(m_currentPool != VK_NULL_HANDLE)
		m_descriptorPools.emplace_back(m_currentPool);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDescriptorPool::~VulkanDescriptorPool()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDescriptorPoolPrefix, "Destroying DescriptorPool");
#endif /*VERBOSE_GRAPHICS_DEBUG*/
	for(VkDescriptorPool& pool : m_descriptorPools)
		vkDestroyDescriptorPool(m_device->GetVkDevice(), pool, nullptr);
	m_descriptorPools.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDescriptorPool::Reset()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkCall(vkResetDescriptorPool(m_device->GetVkDevice(), m_currentPool, 0));
	m_usedDescriptorSetCount = 0;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDescriptorPool TRAP::Graphics::API::VulkanDescriptorPool::GetCurrentVkDescriptorPool() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_currentPool;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::vector<VkDescriptorPoolSize>& TRAP::Graphics::API::VulkanDescriptorPool::GetDescriptorPoolSizes() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_descriptorPoolSizes;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::API::VulkanDescriptorPool::GetUsedDescriptorSetsCount() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_usedDescriptorSetCount;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Scope<TRAP::Graphics::DescriptorSet> TRAP::Graphics::API::VulkanDescriptorPool::RetrieveDescriptorSet(const RendererAPI::DescriptorSetDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(desc.RootSignature, "VulkanDescriptorPool::RetrieveDescriptorSet(): RootSignature is nullptr");

	const TRAP::Ref<VulkanRootSignature> rootSignature = std::dynamic_pointer_cast<VulkanRootSignature>
		(
			desc.RootSignature
		);
	const uint32_t updateFreq = desc.Set;
	const uint8_t dynamicOffsetCount = rootSignature->GetVkDynamicDescriptorCounts()[updateFreq];
	const uint32_t maxSets = desc.MaxSets;
	std::vector<std::vector<VulkanRenderer::DescriptorUpdateData>> updateData{};
	std::vector<VkDescriptorSetLayout> layouts{};
	std::vector<VkDescriptorSet> handles{};

	if (rootSignature->GetVkDescriptorSetLayouts()[updateFreq] != VK_NULL_HANDLE)
	{
		updateData.resize(maxSets);
		layouts.resize(maxSets);
		handles.resize(maxSets);

		for(uint32_t i = 0; i < maxSets; ++i)
		{
			layouts[i] = rootSignature->GetVkDescriptorSetLayouts()[updateFreq];
			updateData[i] = rootSignature->GetUpdateTemplateData()[updateFreq];
		}

		for(std::size_t i = 0; i < handles.size(); ++i)
			handles[i] = RetrieveVkDescriptorSet(layouts[i]);
	}
	else
	{
		TP_ERROR(Log::RendererVulkanDescriptorSetPrefix, "nullptr Descriptor Set Layout for update frequency ",
		         updateFreq, ". Cannot allocate descriptor set");
		TRAP_ASSERT(false, "VulkanDescriptorPool::RetrieveDescriptorSet(): nullptr Descriptor Set Layout for update frequency. Cannot allocate descriptor set");
	}

	if(dynamicOffsetCount != 0u)
	{
		TRAP_ASSERT(dynamicOffsetCount == 1, "VulkanDescriptorPool::RetrieveDescriptorSet(): Only 1 dynamic descriptor per set is supported");
	}

	return TRAP::MakeScope<VulkanDescriptorSet>(m_device, handles, rootSignature, updateData, maxSets,
	                                            dynamicOffsetCount, updateFreq);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] VkDescriptorSet TRAP::Graphics::API::VulkanDescriptorPool::RetrieveVkDescriptorSet(VkDescriptorSetLayout layout)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	//Need a lock since vkAllocateDescriptorSets needs to be externally synchronized
	//This is fine since this will only happen during Init time
	std::lock_guard lockGuard(m_mutex);
	LockMark(m_mutex);

	VkDescriptorSetAllocateInfo info = VulkanInits::DescriptorSetAllocateInfo(m_currentPool,
	                                                                          layout);

	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
	VkResult res = vkAllocateDescriptorSets(m_device->GetVkDevice(), &info, &descriptorSet);
	if(res != VK_SUCCESS)
	{
		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

		VkDescriptorPoolCreateInfo poolInfo = VulkanInits::DescriptorPoolCreateInfo(m_descriptorPoolSizes,
																					m_numDescriptorSets);

		VkCall(vkCreateDescriptorPool(m_device->GetVkDevice(), &poolInfo, nullptr, &descriptorPool));

		m_descriptorPools.emplace_back(descriptorPool);

		m_currentPool = descriptorPool;
		m_usedDescriptorSetCount = 0;

		info.descriptorPool = m_currentPool;
		res = vkAllocateDescriptorSets(m_device->GetVkDevice(), &info, &descriptorSet);
	}

	TRAP_ASSERT(res == VK_SUCCESS, "VulkanDescriptorPool::RetrieveVkDescriptorSet(): Failed to allocate Descriptor Set");

	m_usedDescriptorSetCount++;

	return descriptorSet;
}