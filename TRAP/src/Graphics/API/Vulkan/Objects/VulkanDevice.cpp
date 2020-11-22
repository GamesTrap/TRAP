#include "TRAPPCH.h"
#include "VulkanDevice.h"

#include "VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Graphics::API::VulkanDevice::VulkanDevice(TRAP::Scope<VulkanPhysicalDevice> physicalDevice,
                                                std::vector<std::string> deviceExtensions)
	: m_physicalDevice(std::move(physicalDevice)), m_deviceExtensions(std::move(deviceExtensions))
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Creating Device");
#endif

	std::vector<const char*> extensions(m_deviceExtensions.size());
	for (uint32_t i = 0; i < m_deviceExtensions.size(); i++)
	{
		if (m_physicalDevice->IsExtensionSupported(m_deviceExtensions[i]))
			extensions[i] = m_deviceExtensions[i].c_str();
		else
			m_deviceExtensions.erase(m_deviceExtensions.begin() + i);
	}

#ifdef VERBOSE_GRAPHICS_DEBUG
	if (!m_deviceExtensions.empty())
	{
		TP_DEBUG(Log::RendererVulkanDevicePrefix, "Loading Device Extension(s):");
		for (const std::string& str : m_deviceExtensions)
			TP_DEBUG(Log::RendererVulkanDevicePrefix, "    ", str);
	}
#endif

	if (VulkanRenderer::s_getPhysicalDeviceProperties2Extension &&
		std::find_if(extensions.begin(),
			extensions.end(),
			[](const char* ext)
			{
				return std::strcmp(ext, VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME) == 0;
			}) != extensions.end())
	{
		m_physicalDevice->RetrievePhysicalDeviceFragmentShaderInterlockFeatures();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDevice::~VulkanDevice()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Destroying Device");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::API::VulkanPhysicalDevice>& TRAP::Graphics::API::VulkanDevice::GetPhysicalDevice() const
{
	return m_physicalDevice;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::string>& TRAP::Graphics::API::VulkanDevice::GetUsedPhysicalDeviceExtensions() const
{
	return m_deviceExtensions;
}