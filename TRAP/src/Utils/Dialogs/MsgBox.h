#ifndef _TRAP_MSGBOX_H_
#define _TRAP_MSGBOX_H_

namespace TRAP::Utils::Dialogs::MsgBox
{
	/// <summary>
	/// Options for styles to apply to a message box.
	/// </summary>
	enum class Style
	{
		Info,
		Warning,
		Error,
		Question
	};

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Options for buttons to provide on a message box.
	/// </summary>
	enum class Buttons
	{
		OK,
		OKCancel,
		YesNo,
		Quit
	};

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Possible responses from a message box.<br>
	/// "None" signifies that no option was chosen.<br>
	/// "Error" signifies that an error was encountered while creating the message box.
	/// </summary>
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
	
	/// <summary>
	/// Blocking call to create a modal message box with the given message, title, style and buttons.
	/// </summary>
	/// <param name="message">Message for the message box.</param>
	/// <param name="title">Title for the message box.</param>
	/// <param name="style">Style for the message box.</param>
	/// <param name="buttons">Buttons for the message box.</param>
	/// <returns>Selection made by the user.</returns>
	Selection Show(const char* message, const char* title, Style style, Buttons buttons);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Convenience function to call Show() with default button.
	/// </summary>
	/// <param name="message">Message for the message box.</param>
	/// <param name="title">Title for the message box.</param>
	/// <param name="style">Style for the message box.</param>
	/// <returns>Selection made by the user.</returns>
	Selection Show(const char* message, const char* title, Style style);

	//-------------------------------------------------------------------------------------------------------------------//

	//

	/// <summary>
	/// Convenience function to call Show() with default style.
	/// </summary>
	/// <param name="message">Message for the message box-</param>
	/// <param name="title">Title for the message box.</param>
	/// <param name="buttons">Button(s) for the message box.</param>
	/// <returns>Selection made by the user.</returns>
	Selection Show(const char* message, const char* title, Buttons buttons);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Convenience function to call Show() with default style and button.
	/// </summary>
	/// <param name="message">Message for the message box.</param>
	/// <param name="title">Title message box.</param>
	/// <returns>Selection made by the user.</returns>
	Selection Show(const char* message, const char* title);
}

#endif /*_TRAP_MSGBOX_H_*/