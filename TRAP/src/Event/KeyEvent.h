#ifndef _TRAP_KEYEVENT_H_
#define _TRAP_KEYEVENT_H_

#include "Input/Input.h"
#include "Event.h"

namespace TRAP
{
	class KeyEvent : public Event
	{
	public:
		Input::Key GetKeyCode() const;

		int32_t GetCategoryFlags() const override;

	protected:
		explicit KeyEvent(Input::Key keyCode);

		Input::Key m_keyCode;
	};

	class KeyPressedEvent final : public KeyEvent
	{
	public:
		KeyPressedEvent(Input::Key keyCode, int32_t repeatCount, std::string title);

		int32_t GetRepeatCount() const;

		std::string ToString() const override;

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
		explicit KeyReleasedEvent(Input::Key keyCode, std::string title);

		std::string ToString() const override;

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
		explicit KeyTypedEvent(Input::Key keyCode, std::string title);

		std::string ToString() const override;

		std::string GetTitle() const;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;

	private:
		std::string m_title;
	};
}

#endif /*_TRAP_KEYEVENT_H_*/