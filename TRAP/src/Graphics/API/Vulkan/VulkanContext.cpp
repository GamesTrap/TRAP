#include "TRAPPCH.h"
#include "VulkanContext.h"

#include "Window/Window.h"

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::VulkanContext::s_appName;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanContext::VulkanContext(Window* window)
{
	s_appName = window->GetTitle();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanContext::~VulkanContext()
{
	TP_DEBUG(Log::ContextVulkanPrefix, "Destroying Context");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanContext::SetVSyncIntervalInternal(const uint32_t interval)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanContext::UseInternal(Window* window)
{
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanContext::IsVulkanCapable()
{
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::API::VulkanContext::GetAppName()
{
	return s_appName;
}