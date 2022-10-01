#include "TRAPPCH.h"
#include "Monitor.h"

TRAP::Monitor::Monitor(const uint32_t monitor)
{
	ZoneScoped;

	const std::vector<INTERNAL::WindowingAPI::InternalMonitor*> monitors = INTERNAL::WindowingAPI::GetMonitors();
	m_handle = monitors[monitor];
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Monitor::GetName() const
{
	ZoneScoped;

	return m_handle->Name;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Monitor::VideoMode> TRAP::Monitor::GetVideoModes() const
{
	ZoneScoped;

	std::vector<VideoMode> modes{};
	const std::vector<INTERNAL::WindowingAPI::InternalVideoMode>& internalModes = INTERNAL::WindowingAPI::GetVideoModes(m_handle);

	for(auto i = internalModes.rbegin(); i != internalModes.rend(); ++i)
		modes.emplace_back(i->Width, i->Height, i->RefreshRate);

	return modes;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::Monitor::GetContentScale() const
{
	ZoneScoped;

	TRAP::Math::Vec2 scale{};
	INTERNAL::WindowingAPI::GetMonitorContentScale(m_handle, scale.x, scale.y);

	return scale;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Monitor::GetContentScaleX() const
{
	ZoneScoped;

	return GetContentScale().x;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Monitor::GetContentScaleY() const
{
	ZoneScoped;

	return GetContentScale().y;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2i TRAP::Monitor::GetPosition() const
{
	ZoneScoped;

	TRAP::Math::Vec2i pos{};
	INTERNAL::WindowingAPI::GetMonitorPos(m_handle, pos.x, pos.y);

	return pos;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetPositionX() const
{
	ZoneScoped;

	return GetPosition().x;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetPositionY() const
{
	ZoneScoped;

	return GetPosition().y;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec4i TRAP::Monitor::GetWorkArea() const
{
	ZoneScoped;

	TRAP::Math::Vec4i workArea{};
	INTERNAL::WindowingAPI::GetMonitorWorkArea(m_handle, workArea.x, workArea.y,
	                                           workArea.z, workArea.w);

	return workArea;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetWorkAreaWidth() const
{
	ZoneScoped;

	return GetWorkArea().z;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetWorkAreaHeight() const
{
	ZoneScoped;

	return GetWorkArea().w;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetWorkAreaX() const
{
	ZoneScoped;

	return GetWorkArea().x;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetWorkAreaY() const
{
	ZoneScoped;

	return GetWorkArea().y;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Monitor::GetID() const
{
	ZoneScoped;

	const std::vector<INTERNAL::WindowingAPI::InternalMonitor*> monitors = INTERNAL::WindowingAPI::GetMonitors();
	for(std::size_t i = 0; i < monitors.size(); i++)
	{
		if (monitors[i] == m_handle)
			return i;
	}

	return 0; //Primary Monitor as fallback
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Monitor> TRAP::Monitor::GetAllMonitors()
{
	ZoneScoped;

	std::vector<Monitor> monitors{};
	const uint32_t monitorSize = static_cast<uint32_t>(INTERNAL::WindowingAPI::GetMonitors().size());
	for (uint32_t i = 0; i < monitorSize; i++)
		monitors.emplace_back(Monitor(i));

	return monitors;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Monitor TRAP::Monitor::GetPrimaryMonitor()
{
	ZoneScoped;

	return Monitor(0);
}