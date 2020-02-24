#include "TRAPPCH.h"
#include "MsgBox.h"

TRAP::Utils::MsgBox::Selection TRAP::Utils::MsgBox::Show(const char* message, const char* title, const Style style)
{
	TP_PROFILE_FUNCTION();

	return Show(message, title, style, Buttons::OK);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::MsgBox::Selection TRAP::Utils::MsgBox::Show(const char* message, const char* title, const Buttons buttons)
{
	TP_PROFILE_FUNCTION();

	return Show(message, title, Style::Info, buttons);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::MsgBox::Selection TRAP::Utils::MsgBox::Show(const char* message, const char* title)
{
	TP_PROFILE_FUNCTION();

	return Show(message, title, Style::Info, Buttons::OK);
}