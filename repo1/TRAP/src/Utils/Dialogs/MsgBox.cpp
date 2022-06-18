#include "TRAPPCH.h"
#include "MsgBox.h"

TRAP::Utils::Dialogs::MsgBox::Selection TRAP::Utils::Dialogs::MsgBox::Show(const char* message, const char* title, const Style style)
{
	TP_PROFILE_FUNCTION();

	return Show(message, title, style, Buttons::OK);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Dialogs::MsgBox::Selection TRAP::Utils::Dialogs::MsgBox::Show(const char* message, const char* title, const Buttons buttons)
{
	TP_PROFILE_FUNCTION();

	return Show(message, title, Style::Info, buttons);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Dialogs::MsgBox::Selection TRAP::Utils::Dialogs::MsgBox::Show(const char* message, const char* title)
{
	TP_PROFILE_FUNCTION();

	return Show(message, title, Style::Info, Buttons::OK);
}