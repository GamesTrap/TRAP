#include "TRAPPCH.h"
#include "KeyEvent.h"

#include <utility>

#include "Window/WindowingAPI.h"

TRAP::Input::Key TRAP::Events::KeyEvent::GetKey() const
{
	return m_key;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Events::KeyEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Keyboard) | static_cast<int32_t>(EventCategory::Input);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyEvent::KeyEvent(const Input::Key key)
	: m_key(key)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyEvent::NonPrintableKeyToString(const Input::Key key)
{
	switch(key)
	{
	case Input::Key::Unknown:
		return "ERROR Unknown";

	case Input::Key::Space:
		return "Space";
		
	case Input::Key::Grave_Accent:
		return "Grave Accent";

	case Input::Key::Escape:
		return "Escape";

	case Input::Key::Enter:
		return "Enter";

	case Input::Key::Tab:
		return "Tab";

	case Input::Key::Backspace:
		return "Backspace";

	case Input::Key::Insert:
		return "Insert";

	case Input::Key::Delete:
		return "Delete";

	case Input::Key::Right:
		return "Right";

	case Input::Key::Left:
		return "Left";

	case Input::Key::Down:
		return "Down";

	case Input::Key::Up:
		return "Up";

	case Input::Key::Page_Up:
		return "Page Up";

	case Input::Key::Page_Down:
		return "Page Down";

	case Input::Key::Home:
		return "Home";

	case Input::Key::End:
		return "End";

	case Input::Key::Caps_Lock:
		return "Caps Lock";

	case Input::Key::Scroll_Lock:
		return "Scroll Lock";

	case Input::Key::Num_Lock:
		return "Num Lock";

	case Input::Key::Print_Screen:
		return "Print Screen";

	case Input::Key::Pause:
		return "Pause";

	case Input::Key::F1:
		return "F1";

	case Input::Key::F2:
		return "F2";

	case Input::Key::F3:
		return "F3";

	case Input::Key::F4:
		return "F4";

	case Input::Key::F5:
		return "F5";

	case Input::Key::F6:
		return "F6";

	case Input::Key::F7:
		return "F7";

	case Input::Key::F8:
		return "F8";

	case Input::Key::F9:
		return "F9";

	case Input::Key::F10:
		return "F10";

	case Input::Key::F11:
		return "F11";

	case Input::Key::F12:
		return "F12";

	case Input::Key::F13:
		return "F13";

	case Input::Key::F14:
		return "F14";

	case Input::Key::F15:
		return "F15";

	case Input::Key::F16:
		return "F16";

	case Input::Key::F17:
		return "F17";

	case Input::Key::F18:
		return "F18";

	case Input::Key::F19:
		return "F19";

	case Input::Key::F20:
		return "F20";

	case Input::Key::F21:
		return "F21";

	case Input::Key::F22:
		return "F22";

	case Input::Key::F23:
		return "F23";

	case Input::Key::F24:
		return "F24";

	case Input::Key::F25:
		return "F25";

	case Input::Key::KP_Enter:
		return "Numpad Enter";

	case Input::Key::Left_Shift:
		return "Left Shift";

	case Input::Key::Left_Control:
		return "Left Control";

	case Input::Key::Left_ALT:
		return "Left ALT";

	case Input::Key::Left_Super:
		return "Left Windows/Super";

	case Input::Key::Right_Shift:
		return "Right Shift";

	case Input::Key::Right_Control:
		return "Right Control";

	case Input::Key::Right_ALT:
		return "Right ALT";

	case Input::Key::Right_Super:
		return "Right Windows/Super";

	case Input::Key::Menu:
		return "Menu";
		
	default:
		return "";
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyPressEvent::KeyPressEvent(const Input::Key key, const int32_t repeatCount, const std::string_view title)
	: KeyEvent(key), m_repeatCount(repeatCount), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Events::KeyPressEvent::GetRepeatCount() const
{
	return m_repeatCount;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyPressEvent::ToString() const
{
	const char* name = INTERNAL::WindowingAPI::GetKeyName(m_key, 0);
	
	std::stringstream ss;
	ss << "KeyPressEvent: ";
	
	if(name)
		ss << name;
	else
		ss << NonPrintableKeyToString(m_key);
	
	ss << "(" << static_cast<int32_t>(m_key) << ")" << " (" << m_repeatCount << " repeats)";

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Events::KeyPressEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyPressEvent::GetStaticType()
{
	return EventType::KeyPress;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyPressEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::KeyPressEvent::GetName() const
{
	return "KeyPress";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyReleaseEvent::KeyReleaseEvent(const Input::Key key, const std::string_view title)
	: KeyEvent(key), m_title(title)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyReleaseEvent::ToString() const
{
	const char* name = INTERNAL::WindowingAPI::GetKeyName(m_key, 0);
	
	std::stringstream ss;	
	ss << "KeyReleaseEvent: ";

	if (name)
		ss << name;
	else
		ss << NonPrintableKeyToString(m_key);

	ss << "(" << static_cast<int32_t>(m_key) << ")";

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Events::KeyReleaseEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyReleaseEvent::GetStaticType()
{
	return EventType::KeyRelease;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyReleaseEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::KeyReleaseEvent::GetName() const
{
	return "KeyRelease";
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyTypeEvent::KeyTypeEvent(const uint32_t codePoint, const std::string_view title)
	: m_title(title), m_codePoint(codePoint)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyTypeEvent::ToString() const
{
	std::stringstream ss;
	
	ss << "KeyTypeEvent: " << EncodeUTF8(m_codePoint) << "(" << m_codePoint << ")";

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Events::KeyTypeEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Events::KeyTypeEvent::GetCodePoint() const
{
	return m_codePoint;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyTypeEvent::GetStaticType()
{
	return EventType::KeyType;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyTypeEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::KeyTypeEvent::GetName() const
{
	return "KeyType";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Events::KeyTypeEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Keyboard) | static_cast<int32_t>(EventCategory::Input);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyTypeEvent::EncodeUTF8(const uint32_t codePoint)
{
	std::string result{};

	if (codePoint < 0x80)
		result.push_back(static_cast<uint8_t>(codePoint));
	else if (codePoint < 0x800)
	{
		result.push_back((codePoint >> 6) | 0xC0);
		result.push_back((codePoint & 0x3F) | 0x80);
	}
	else if (codePoint < 0x10000)
	{
		result.push_back((codePoint >> 12) | 0xE0);
		result.push_back(((codePoint >> 6) & 0x3F) | 0x80);
		result.push_back((codePoint & 0x3F) | 0x80);
	}
	else if (codePoint < 0x110000)
	{
		result.push_back((codePoint >> 18) | 0xF0);
		result.push_back(((codePoint >> 12) & 0x3F) | 0x80);
		result.push_back(((codePoint >> 6) & 0x3F) | 0x80);
		result.push_back((codePoint & 0x3F) | 0x80);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::KeyLayoutEvent::KeyLayoutEvent(std::string layout)
	: m_layout(std::move(layout))
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyLayoutEvent::GetLayout() const
{
	return m_layout;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::KeyLayoutEvent::ToString() const
{
	std::stringstream ss;

	ss << "KeyLayoutEvent: " << m_layout;

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyLayoutEvent::GetStaticType()
{
	return EventType::KeyLayout;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::KeyLayoutEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::KeyLayoutEvent::GetName() const
{
	return "KeyLayout";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Events::KeyLayoutEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Keyboard) | static_cast<int32_t>(EventCategory::Input);
}