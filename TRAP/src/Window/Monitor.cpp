#include "TRAPPCH.h"
#include "Monitor.h"

#ifndef TRAP_HEADLESS_MODE

TRAP::Monitor::Monitor(const uint32_t monitor)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	const std::vector<INTERNAL::WindowingAPI::InternalMonitor*> monitors = INTERNAL::WindowingAPI::GetMonitors();
	m_handle = monitors[monitor];
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Monitor::GetName() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_handle->Name;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<TRAP::Monitor::VideoMode> TRAP::Monitor::GetVideoModes() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	std::vector<VideoMode> modes{};
	const std::vector<INTERNAL::WindowingAPI::InternalVideoMode>& internalModes = INTERNAL::WindowingAPI::GetVideoModes(*m_handle);

	for(auto i = internalModes.rbegin(); i != internalModes.rend(); ++i)
		modes.emplace_back(i->Width, i->Height, i->RefreshRate);

	return modes;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2 TRAP::Monitor::GetContentScale() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	TRAP::Math::Vec2 scale{};
	INTERNAL::WindowingAPI::GetMonitorContentScale(*m_handle, scale.x, scale.y);

	return scale;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Monitor::GetContentScaleX() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetContentScale().x;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Monitor::GetContentScaleY() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetContentScale().y;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2i TRAP::Monitor::GetPosition() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	TRAP::Math::Vec2i pos{};
	INTERNAL::WindowingAPI::GetMonitorPos(*m_handle, pos.x, pos.y);

	return pos;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] int32_t TRAP::Monitor::GetPositionX() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetPosition().x;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] int32_t TRAP::Monitor::GetPositionY() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetPosition().y;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec4i TRAP::Monitor::GetWorkArea() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	TRAP::Math::Vec4i workArea{};
	INTERNAL::WindowingAPI::GetMonitorWorkArea(*m_handle, workArea.x, workArea.y,
	                                           workArea.z, workArea.w);

	return workArea;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] int32_t TRAP::Monitor::GetWorkAreaWidth() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetWorkArea().z;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] int32_t TRAP::Monitor::GetWorkAreaHeight() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetWorkArea().w;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] int32_t TRAP::Monitor::GetWorkAreaX() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetWorkArea().x;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] int32_t TRAP::Monitor::GetWorkAreaY() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetWorkArea().y;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Monitor::GetID() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	const std::vector<INTERNAL::WindowingAPI::InternalMonitor*> monitors = INTERNAL::WindowingAPI::GetMonitors();
	for(std::size_t i = 0; i < monitors.size(); i++)
	{
		if (monitors[i] == m_handle)
			return NumericCast<uint32_t>(i);
	}

	return 0; //Primary Monitor as fallback
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<TRAP::Monitor> TRAP::Monitor::GetAllMonitors()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	std::vector<Monitor> monitors{};
	const std::size_t monitorSize = INTERNAL::WindowingAPI::GetMonitors().size();
	for (std::size_t i = 0; i < monitorSize; i++)
		monitors.emplace_back(Monitor(NumericCast<uint32_t>(i)));

	return monitors;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Monitor TRAP::Monitor::GetPrimaryMonitor()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return Monitor(0);
}

#endif /*TRAP_HEADLESS_MODE*/