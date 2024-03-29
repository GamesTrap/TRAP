#include "TRAPPCH.h"
#include "VulkanFence.h"

#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Objects/AftermathTracker.h"
#include "Utils/ErrorCodes/ErrorCodes.h"

TRAP::Graphics::API::VulkanFence::VulkanFence(const bool signalled)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanFence()(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanFencePrefix, "Creating Fence");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	VkFenceCreateInfo info = VulkanInits::FenceCreateInfo();
	if(signalled)
		info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	VkCall(vkCreateFence(m_device->GetVkDevice(), &info, nullptr, &m_fence));
	TRAP_ASSERT(m_fence, "VulkanFence(): Vulkan Fence is nullptr");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanFence::~VulkanFence()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanFencePrefix, "Destroying Fence");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroyFence(m_device->GetVkDevice(), m_fence, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::FenceStatus TRAP::Graphics::API::VulkanFence::GetStatus()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if(!m_submitted)
		return RendererAPI::FenceStatus::NotSubmitted;

	const VkResult res = vkGetFenceStatus(m_device->GetVkDevice(), m_fence);
	if(res == VK_SUCCESS)
	{
		VkCall(vkResetFences(m_device->GetVkDevice(), 1, &m_fence));
		m_submitted = false;
	}

	return res == VK_SUCCESS ? RendererAPI::FenceStatus::Complete : RendererAPI::FenceStatus::Incomplete;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanFence::Wait()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if(m_submitted)
	{
#ifdef ENABLE_NSIGHT_AFTERMATH
		const VkResult result = vkWaitForFences(m_device->GetVkDevice(), 1, &m_fence, VK_TRUE, std::numeric_limits<u32>::max());
		if(RendererAPI::s_aftermathSupport)
		{
			if(result == VK_ERROR_DEVICE_LOST)
			{
				//Device lost notification is async to the NVIDIA display driver's GPU crash handling.

				GFSDK_Aftermath_CrashDump_Status status = GFSDK_Aftermath_CrashDump_Status_Unknown;
				TRAP::Graphics::AftermathTracker::AftermathCall(TRAP::Graphics::AftermathTracker::GetGPUCrashDumpStatus(status));

				auto tStart = std::chrono::steady_clock::now();
				auto tElapsed = std::chrono::milliseconds::zero();

				//Loop while Aftermath crash dump data collection has not finished or
				//the application is still processing the crash dump data.
				while (status != GFSDK_Aftermath_CrashDump_Status_CollectingDataFailed &&
					   status != GFSDK_Aftermath_CrashDump_Status_Finished &&
					   tElapsed.count() < 5000)
				{
					// Sleep a couple of milliseconds and poll the status again.
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					TRAP::Graphics::AftermathTracker::AftermathCall(TRAP::Graphics::AftermathTracker::GetGPUCrashDumpStatus(status));

					tElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - tStart);
				}

				Utils::DisplayError(Utils::ErrorCode::VulkanDeviceLost);
			}
		}
#else
	VkCall(vkWaitForFences(m_device->GetVkDevice(), 1, &m_fence, VK_TRUE, std::numeric_limits<u32>::max()));
#endif
	}

	VkCall(vkResetFences(m_device->GetVkDevice(), 1, &m_fence));

	m_submitted = false;
}