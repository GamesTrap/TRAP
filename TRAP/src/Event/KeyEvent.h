#pragma once

namespace TRAP
{
	class KeyEvent : public Event
	{
	public:
		int GetKeyCode() const { return m_keyCode; }

		int GetCategoryFlags() const override { return static_cast<int>(EventCategory::EventCategoryKeyboard) | static_cast<int>(EventCategory::EventCategoryInput); }

	protected:
		explicit KeyEvent(const int keyCode)
			: m_keyCode(keyCode)
		{			
		}

		int m_keyCode;
	};

	class KeyPressedEvent final : public KeyEvent
	{
	public:
		KeyPressedEvent(const int keyCode, const int repeatCount)
			: KeyEvent(keyCode), m_repeatCount(repeatCount)
		{			
		}

		int GetRepeatCount() const { return m_repeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_keyCode << " (" << m_repeatCount << " repeats)";

			return ss.str();
		}

		static EventType GetStaticType() { return EventType::KeyPressed; }
		EventType GetEventType() const override { return GetStaticType(); }
		const char* GetName() const override { return "KeyPressed"; }

	private:
		int m_repeatCount;
	};

	class KeyReleasedEvent final : public KeyEvent
	{
	public:
		explicit KeyReleasedEvent(const int keyCode)
			: KeyEvent(keyCode)
		{			
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_keyCode;

			return ss.str();
		}

		static EventType GetStaticType() { return EventType::KeyReleased; }
		EventType GetEventType() const override { return GetStaticType(); }
		const char* GetName() const override { return "KeyReleased"; }
	};

	class KeyTypedEvent final : public KeyEvent
	{
	public:
		explicit KeyTypedEvent(const int keyCode)
			: KeyEvent(keyCode)
		{			
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_keyCode;

			return ss.str();
		}

		static EventType GetStaticType() { return EventType::KeyTyped; }
		EventType GetEventType() const override { return GetStaticType(); }
		const char* GetName() const override { return "KeyTyped"; }
	};
}
