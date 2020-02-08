#include "TRAPPCH.h"
#include "Monitor.h"

#include "WindowingAPI.h"

TRAP::Monitor::Monitor(INTERNAL::WindowingAPI::InternalMonitor* monitor)
	: m_handle(monitor)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Monitor::Monitor(const uint32_t monitor)
{
	std::vector<INTERNAL::WindowingAPI::InternalMonitor*> monitors = INTERNAL::WindowingAPI::GetMonitors();
	m_handle = monitors[monitor];
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Monitor::GetName() const
{
	return m_handle->Name;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Monitor::VideoMode> TRAP::Monitor::GetVideoModes() const
{
	std::vector<VideoMode> modes{};
	for(const INTERNAL::WindowingAPI::InternalVideoMode mode : m_handle->Modes)
		modes.push_back({ mode.Width, mode.Height, mode.RefreshRate });
	
	return modes;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Monitor::VideoMode TRAP::Monitor::GetCurrentVideoMode() const
{	
	return VideoMode{ m_handle->CurrentMode.Width, m_handle->CurrentMode.Height, m_handle->CurrentMode.RefreshRate };
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Monitor::IsInUse() const
{
	return m_handle->Window ? true : false;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Monitor::GetID() const
{
	std::vector<INTERNAL::WindowingAPI::InternalMonitor*> monitors = INTERNAL::WindowingAPI::GetMonitors();
	for(uint32_t i = 0; i < monitors.size(); i++)
		if (monitors[i] == m_handle)
			return i;

	return 0; //Primary Monitor as fallback
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::Monitor::GetInternalMonitor() const
{
	return m_handle;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Monitor> TRAP::Monitor::GetAllMonitors()
{
	std::vector<Monitor> monitors;
	std::vector<INTERNAL::WindowingAPI::InternalMonitor*> internalMonitors = INTERNAL::WindowingAPI::GetMonitors();
	for (auto& internalMonitor : internalMonitors)
		monitors.emplace_back(internalMonitor);

	return monitors;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Monitor TRAP::Monitor::GetPrimaryMonitor()
{	
	return Monitor(INTERNAL::WindowingAPI::GetPrimaryMonitor().get());
}