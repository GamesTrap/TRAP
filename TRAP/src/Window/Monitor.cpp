#include "TRAPPCH.h"
#include "Monitor.h"

#include "Utils/ErrorCodes/ErrorCodes.h"

#ifndef TRAP_HEADLESS_MODE

TRAP::Monitor::Monitor(INTERNAL::WindowingAPI::InternalMonitor* const monitor)
	: m_handle(monitor)
{
	TRAP_ASSERT(m_handle, "Monitor::Monitor(): monitor is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<TRAP::Monitor::VideoMode> TRAP::Monitor::GetVideoModes() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	std::vector<VideoMode> modes{};
	const std::vector<INTERNAL::WindowingAPI::InternalVideoMode>& internalModes = INTERNAL::WindowingAPI::GetVideoModes(*m_handle);

	for(const auto& internalMode : std::ranges::reverse_view(internalModes))
		modes.emplace_back(NumericCast<u32>(internalMode.Width), NumericCast<u32>(internalMode.Height),
	                       internalMode.RefreshRate);

	return modes;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<TRAP::Monitor::VideoMode> TRAP::Monitor::GetNativeVideoMode() const noexcept
{
	const auto nativeVideoMode = INTERNAL::WindowingAPI::GetNativeVideoMode(*m_handle);
	if(!nativeVideoMode)
		return std::nullopt;

	return VideoMode{ NumericCast<u32>(nativeVideoMode->Width), NumericCast<u32>(nativeVideoMode->Height),
	                  nativeVideoMode->RefreshRate };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<TRAP::Monitor::VideoMode> TRAP::Monitor::GetCurrentVideoMode() const noexcept
{
	const auto currVideoMode = INTERNAL::WindowingAPI::GetVideoMode(*m_handle);
	if(!currVideoMode)
		return std::nullopt;

	return VideoMode{ static_cast<u32>(currVideoMode->Width), static_cast<u32>(currVideoMode->Height),
	                  currVideoMode->RefreshRate };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2 TRAP::Monitor::GetContentScale() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	TRAP::Math::Vec2 scale{};
	INTERNAL::WindowingAPI::GetMonitorContentScale(*m_handle, scale.x(), scale.y());

	return scale;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Monitor::GetContentScaleX() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetContentScale().x();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Monitor::GetContentScaleY() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetContentScale().y();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2i TRAP::Monitor::GetPosition() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	TRAP::Math::Vec2i pos{};
	INTERNAL::WindowingAPI::GetMonitorPos(*m_handle, pos.x(), pos.y());

	return pos;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] i32 TRAP::Monitor::GetPositionX() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetPosition().x();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] i32 TRAP::Monitor::GetPositionY() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetPosition().y();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec4i TRAP::Monitor::GetWorkArea() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	TRAP::Math::Vec4i workArea{};
	INTERNAL::WindowingAPI::GetMonitorWorkArea(*m_handle, workArea.x(), workArea.y(),
	                                           workArea.z(), workArea.w());

	return workArea;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] i32 TRAP::Monitor::GetWorkAreaWidth() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetWorkArea().z();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] i32 TRAP::Monitor::GetWorkAreaHeight() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetWorkArea().w();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] i32 TRAP::Monitor::GetWorkAreaX() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetWorkArea().x();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] i32 TRAP::Monitor::GetWorkAreaY() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetWorkArea().y();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u32 TRAP::Monitor::GetID() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	const std::vector<INTERNAL::WindowingAPI::InternalMonitor*> monitors = INTERNAL::WindowingAPI::GetMonitors();
	for(usize i = 0; i < monitors.size(); i++)
	{
		if (monitors[i] == m_handle)
			return NumericCast<u32>(i);
	}

	return 0; //Primary Monitor as fallback
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<TRAP::Monitor> TRAP::Monitor::GetAllMonitors()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window);

	if (!INTERNAL::WindowingAPI::Init())
		Utils::DisplayError(Utils::ErrorCode::WindowingAPIFailedInitialization);

	std::vector<Monitor> monitors{};
	for(auto *internalMonitor : INTERNAL::WindowingAPI::GetMonitors())
		monitors.push_back(Monitor(internalMonitor));

	return monitors;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Monitor TRAP::Monitor::GetPrimaryMonitor()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Window) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if (!INTERNAL::WindowingAPI::Init())
		Utils::DisplayError(Utils::ErrorCode::WindowingAPIFailedInitialization);

	return Monitor(INTERNAL::WindowingAPI::GetPrimaryMonitor());
}

#endif /*TRAP_HEADLESS_MODE*/