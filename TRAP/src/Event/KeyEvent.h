#ifndef _TRAP_KEYEVENT_H_
#define _TRAP_KEYEVENT_H_

#include "Input/Input.h"

namespace TRAP
{
	class KeyEvent : public Event
	{
	public:
		Input::Key GetKeyCode() const;

		int32_t GetCategoryFlags() const override;

	protected:
		explicit KeyEvent(const Input::Key keyCode)
			: m_keyCode(keyCode)
		{			
		}

		Input::Key m_keyCode;
	};

	class KeyPressedEvent final : public KeyEvent
	{
	public:
		KeyPressedEvent(const Input::Key keyCode, const int32_t repeatCount, std::string title)
			: KeyEvent(keyCode), m_repeatCount(repeatCount), m_title(std::move(title))
		{			
		}

		int32_t GetRepeatCount() const;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << static_cast<int32_t>(m_keyCode) << " (" << m_repeatCount << " repeats)";

			return ss.str();
		}

		std::string GetTitle() const;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;

	private:
		int32_t m_repeatCount;
		std::string m_title;
	};

	class KeyReleasedEvent final : public KeyEvent
	{
	public:
		explicit KeyReleasedEvent(const Input::Key keyCode, std::string title)
			: KeyEvent(keyCode), m_title(std::move(title))
		{			
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << static_cast<int32_t>(m_keyCode);

			return ss.str();
		}

		std::string GetTitle() const;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;

	private:
		std::string m_title;
	};

	class KeyTypedEvent final : public KeyEvent
	{
	public:
		explicit KeyTypedEvent(const Input::Key keyCode, std::string title)
			: KeyEvent(keyCode), m_title(std::move(title))
		{			
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << static_cast<int32_t>(m_keyCode);

			return ss.str();
		}

		std::string GetTitle() const;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;

	private:
		std::string m_title;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Input::Key TRAP::KeyEvent::GetKeyCode() const
{
	return m_keyCode;
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::KeyEvent::GetCategoryFlags() const
{
	return static_cast<int32_t>(EventCategory::Keyboard) | static_cast<int32_t>(EventCategory::Input);
}

//-------------------------------------------------------------------------------------------------------------------//

inline int32_t TRAP::KeyPressedEvent::GetRepeatCount() const
{
	return m_repeatCount;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::KeyPressedEvent::GetStaticType()
{
	return EventType::KeyPressed;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::KeyPressedEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::KeyPressedEvent::GetName() const
{
	return "KeyPressed";
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::KeyReleasedEvent::GetStaticType()
{
	return EventType::KeyReleased;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::KeyReleasedEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::KeyReleasedEvent::GetName() const
{
	return "KeyReleased";
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::KeyTypedEvent::GetStaticType()
{
	return EventType::KeyTyped;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::EventType TRAP::KeyTypedEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::KeyTypedEvent::GetName() const
{
	return "KeyTyped";
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::KeyPressedEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::KeyReleasedEvent::GetTitle() const
{
	return m_title;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::KeyTypedEvent::GetTitle() const
{
	return m_title;
}

#endif /*_TRAP_KEYEVENT_H_*/