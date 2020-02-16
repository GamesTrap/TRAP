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

		static std::string NonPrintableKeyToString(Input::Key keyCode);

		Input::Key m_keyCode;
	};

	class KeyPressEvent final : public KeyEvent
	{
	public:
		KeyPressEvent(Input::Key keyCode, int32_t repeatCount, std::string_view title);

		int32_t GetRepeatCount() const;

		std::string_view GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;

	private:
		int32_t m_repeatCount;
		std::string_view m_title;
	};

	class KeyReleaseEvent final : public KeyEvent
	{
	public:
		explicit KeyReleaseEvent(Input::Key keyCode, std::string_view title);

		std::string_view GetTitle() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;

	private:
		std::string_view m_title;
	};

	class KeyTypeEvent final : public Event
	{
	public:
		explicit KeyTypeEvent(uint32_t codePoint, std::string_view title);

		std::string_view GetTitle() const;
		uint32_t GetCodePoint() const;

		std::string ToString() const override;

		static EventType GetStaticType();
		EventType GetEventType() const override;
		const char* GetName() const override;
		int32_t GetCategoryFlags() const override;
		
	private:
		static std::string EncodeUTF8(uint32_t codePoint);
		
		std::string_view m_title;
		uint32_t m_codePoint;
	};
}

#endif /*_TRAP_KEYEVENT_H_*/