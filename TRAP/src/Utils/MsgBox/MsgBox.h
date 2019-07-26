#ifndef _TRAP_MSGBOX_H_
#define _TRAP_MSGBOX_H_

namespace TRAP::Utils::MsgBox
{
	//Options for styles to apply to a message box
	enum class Style
	{
		Info,
		Warning,
		Error,
		Question
	};

	//-------------------------------------------------------------------------------------------------------------------//

	//Options for buttons to provide on a message box
	enum class Buttons
	{
		OK,
		OKCancel,
		YesNo,
		Quit
	};

	//-------------------------------------------------------------------------------------------------------------------//

	//Possible responses from a message box.
	//'None' signifies that no option was chosen.
	//'Error' signifies that an error was encountered while creating the message box.
	enum class Selection
	{
		OK,
		Cancel,
		Yes,
		No,
		Quit,
		None,
		Error
	};

	//-------------------------------------------------------------------------------------------------------------------//

	//Blocking call to create a modal message box with the given message, title, style and buttons
	Selection Show(const char* message, const char* title, Style style, Buttons buttons);

	//-------------------------------------------------------------------------------------------------------------------//

	//Convenience function to call Show() with the default buttons
	inline Selection Show(const char* message, const char* title, Style style)
	{
		return Show(message, title, style, Buttons::OK);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	//Convenience function to call Show() with the default style
	inline Selection Show(const char* message, const char* title, Buttons buttons)
	{
		return Show(message, title, Style::Info, buttons);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	//Convenience function to call Show() with the default style and buttons
	inline Selection Show(const char* message, const char* title)
	{
		return Show(message, title, Style::Info, Buttons::OK);
	}
}

#endif /*_TRAP_MSGBOX_H_*/