#include "TRAPPCH.h"
#include "KeyEvent.h"

#ifndef TRAP_HEADLESS_MODE

#include "Utils/String/String.h"
#include "Window/WindowingAPI.h"

[[nodiscard]] TRAP::Events::EventCategory TRAP::Events::KeyEvent::GetCategoryFlags() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return EventCategory::Keyboard | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyPressEvent::KeyPressEvent(const Input::Key key, TRAP::Window* window)
	: KeyEvent(key), m_window(window)
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "Events::KeyPressEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyPressEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const std::string name = TRAP::Input::GetKeyName(m_key);

	return fmt::format("KeyPressEvent: {}({})", name, ToUnderlying(m_key));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Events::EventType TRAP::Events::KeyPressEvent::GetEventType() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyPressEvent::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "KeyPress";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyRepeatEvent::KeyRepeatEvent(const Input::Key key, TRAP::Window* window)
	: KeyEvent(key), m_window(window)
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "Events::KeyRepeatEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyRepeatEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const std::string name = TRAP::Input::GetKeyName(m_key);

	return fmt::format("KeyRepeatEvent: {}({})", name, ToUnderlying(m_key));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Events::EventType TRAP::Events::KeyRepeatEvent::GetEventType() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyRepeatEvent::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "KeyRepeat";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyReleaseEvent::KeyReleaseEvent(const Input::Key key, TRAP::Window* window)
	: KeyEvent(key), m_window(window)
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "Events::KeyReleaseEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyReleaseEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const std::string name = TRAP::Input::GetKeyName(m_key);

	return fmt::format("KeyReleaseEvent: {}({})", name, ToUnderlying(m_key));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Events::EventType TRAP::Events::KeyReleaseEvent::GetEventType() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyReleaseEvent::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "KeyRelease";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyTypeEvent::KeyTypeEvent(const uint32_t codePoint, TRAP::Window* window)
	: m_window(window), m_codePoint(codePoint)
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(window, "Events::KeyTypeEvent(): Window is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyTypeEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("KeyTypeEvent: {}({})", Utils::String::EncodeUTF8(m_codePoint), m_codePoint);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Events::EventType TRAP::Events::KeyTypeEvent::GetEventType() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyTypeEvent::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "KeyType";
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Events::EventCategory TRAP::Events::KeyTypeEvent::GetCategoryFlags() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return EventCategory::Keyboard | EventCategory::Input;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyLayoutEvent::KeyLayoutEvent(std::string layout) noexcept
	: m_layout(std::move(layout))
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyLayoutEvent::GetLayout() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_layout;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyLayoutEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return fmt::format("KeyLayoutEvent: {}", m_layout);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Events::EventType TRAP::Events::KeyLayoutEvent::GetEventType() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::KeyLayoutEvent::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "KeyLayout";
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Events::EventCategory TRAP::Events::KeyLayoutEvent::GetCategoryFlags() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return EventCategory::Keyboard | EventCategory::Input;
}

#endif /*TRAP_HEADLESS_MODE*/