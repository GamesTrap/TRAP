#include "TRAPPCH.h"
#include "Monitor.h"

#include "WindowingAPI.h"

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

	for(const INTERNAL::WindowingAPI::InternalVideoMode mode : INTERNAL::WindowingAPI::GetVideoModes(m_handle))
		modes.push_back({ mode.Width, mode.Height, mode.RefreshRate });
	
	return modes;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Monitor::VideoMode TRAP::Monitor::GetCurrentVideoMode() const
{	
	return VideoMode{ m_handle->CurrentMode.Width, m_handle->CurrentMode.Height, m_handle->CurrentMode.RefreshRate };
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::Monitor::GetContentScale() const
{
	float xScale, yScale;
	INTERNAL::WindowingAPI::GetMonitorContentScale(m_handle, xScale, yScale);
	
	return { xScale, yScale };
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Monitor::GetContentScaleX() const
{
	float xScale, yScale;
	INTERNAL::WindowingAPI::GetMonitorContentScale(m_handle, xScale, yScale);

	return xScale;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Monitor::GetContentScaleY() const
{
	float xScale, yScale;
	INTERNAL::WindowingAPI::GetMonitorContentScale(m_handle, xScale, yScale);

	return yScale;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2i TRAP::Monitor::GetPosition() const
{
	int32_t xPos, yPos;
	INTERNAL::WindowingAPI::GetMonitorPos(m_handle, xPos, yPos);

	return { xPos, yPos };
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetPositionX() const
{
	int32_t xPos, yPos;
	INTERNAL::WindowingAPI::GetMonitorPos(m_handle, xPos, yPos);

	return xPos;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetPositionY() const
{
	int32_t xPos, yPos;
	INTERNAL::WindowingAPI::GetMonitorPos(m_handle, xPos, yPos);

	return yPos;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec4i TRAP::Monitor::GetWorkArea() const
{
	int32_t xPos, yPos, width, height;
	INTERNAL::WindowingAPI::GetMonitorWorkArea(m_handle, xPos, yPos, width, height);

	return { xPos, yPos, width, height };
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetWorkAreaWidth() const
{
	int32_t xPos, yPos, width, height;
	INTERNAL::WindowingAPI::GetMonitorWorkArea(m_handle, xPos, yPos, width, height);

	return width;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetWorkAreaHeight() const
{
	int32_t xPos, yPos, width, height;
	INTERNAL::WindowingAPI::GetMonitorWorkArea(m_handle, xPos, yPos, width, height);

	return height;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetWorkAreaX() const
{
	int32_t xPos, yPos, width, height;
	INTERNAL::WindowingAPI::GetMonitorWorkArea(m_handle, xPos, yPos, width, height);

	return xPos;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetWorkAreaY() const
{
	int32_t xPos, yPos, width, height;
	INTERNAL::WindowingAPI::GetMonitorWorkArea(m_handle, xPos, yPos, width, height);

	return yPos;
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
	const uint32_t internalMonitorsSize = static_cast<uint32_t>(INTERNAL::WindowingAPI::GetMonitors().size());
	for (uint32_t i = 0; i < internalMonitorsSize; i++)
		monitors.emplace_back(i);

	return monitors;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Monitor TRAP::Monitor::GetPrimaryMonitor()
{	
	return Monitor(0);
}