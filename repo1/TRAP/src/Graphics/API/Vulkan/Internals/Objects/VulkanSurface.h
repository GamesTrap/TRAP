#ifndef _TRAP_VULKANSURFACE_H_
#define _TRAP_VULKANSURFACE_H_

#include "Window/WindowingAPI.h"

namespace TRAP::Graphics::API::Vulkan
{
	struct QueueFamilyIndices;
	class PhysicalDevice;
	class Instance;

	class Surface
	{
	public:
		Surface(const Scope<Instance>& instance, PhysicalDevice& device, INTERNAL::WindowingAPI::InternalWindow* window);
		Surface(const Surface&) = default;
		Surface& operator=(const Surface&) = default;
		Surface(Surface&&) = default;
		Surface& operator=(Surface&&) = default;
		~Surface();

		VkSurfaceKHR GetSurface() const;
		VkSurfaceCapabilitiesKHR GetSurfaceCapabilities();
		const std::vector<VkSurfaceFormatKHR>& GetSurfaceFormats() const;
		const std::vector<VkPresentModeKHR>& GetPresentModes() const;
		VkSurfaceFormatKHR& GetOptimalSurfaceFormat() const;
		VkFormat* GetOptimalDepthFormat();
		VkPresentModeKHR& GetOptimalPresentMode();
		
	private:
        PhysicalDevice* m_device;
		VkSurfaceKHR m_surface;
		VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
		std::vector<VkPresentModeKHR> m_presentModes;
		Instance* m_instance;
		VkSurfaceFormatKHR* m_usedSurfaceFormat;
		VkPresentModeKHR m_usedPresentMode;
		VkFormat m_depthFormat;
		bool m_supportDepthStencil;
	};
}

#endif /*_TRAP_VULKANSURFACE_H_*/