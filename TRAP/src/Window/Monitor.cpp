#include "TRAPPCH.h"
#include "Monitor.h"

TRAP::Monitor::Monitor(const uint32_t monitor)
{
	TP_PROFILE_FUNCTION();
	
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
	TP_PROFILE_FUNCTION();

	std::vector<VideoMode> modes{};
	const std::vector<INTERNAL::WindowingAPI::InternalVideoMode>& internalModes = INTERNAL::WindowingAPI::GetVideoModes(m_handle);

	//for (const INTERNAL::WindowingAPI::InternalVideoMode mode : INTERNAL::WindowingAPI::GetVideoModes(m_handle))
	for(auto i = internalModes.rbegin(); i != internalModes.rend(); ++i)
		modes.push_back({ i->Width, i->Height, i->RefreshRate });
		
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
	TP_PROFILE_FUNCTION();

	float xScale, yScale;
	INTERNAL::WindowingAPI::GetMonitorContentScale(m_handle, xScale, yScale);
	
	return { xScale, yScale };
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Monitor::GetContentScaleX() const
{
	const auto vec = GetContentScale();

	return vec.x;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Monitor::GetContentScaleY() const
{
	const auto vec = GetContentScale();

	return vec.y;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2i TRAP::Monitor::GetPosition() const
{
	TP_PROFILE_FUNCTION();

	int32_t xPos, yPos;
	INTERNAL::WindowingAPI::GetMonitorPos(m_handle, xPos, yPos);

	return { xPos, yPos };
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetPositionX() const
{
	const auto vec = GetPosition();

	return vec.x;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetPositionY() const
{
	const auto vec = GetPosition();

	return vec.y;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec4i TRAP::Monitor::GetWorkArea() const
{
	TP_PROFILE_FUNCTION();

	int32_t xPos, yPos, width, height;
	INTERNAL::WindowingAPI::GetMonitorWorkArea(m_handle, xPos, yPos, width, height);

	return { xPos, yPos, width, height };
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetWorkAreaWidth() const
{
	const auto vec = GetWorkArea();
	
	return vec.z;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetWorkAreaHeight() const
{
	const auto vec = GetWorkArea();

	return vec.w;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetWorkAreaX() const
{
	const auto vec = GetWorkArea();

	return vec.x;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Monitor::GetWorkAreaY() const
{
	const auto vec = GetWorkArea();
	
	return vec.y;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Monitor::IsInUse() const
{
	return m_handle->Window ? true : false;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Monitor::GetID() const
{
	TP_PROFILE_FUNCTION();

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
	TP_PROFILE_FUNCTION();

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