#include "TRAPPCH.h"
#include "MsgBox.h"

TRAP::Utils::MsgBox::Selection TRAP::Utils::MsgBox::Show(const char* message, const char* title, const Style style)
{
	return Show(message, title, style, Buttons::OK);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::MsgBox::Selection TRAP::Utils::MsgBox::Show(const char* message, const char* title, const Buttons buttons)
{
	return Show(message, title, Style::Info, buttons);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::MsgBox::Selection TRAP::Utils::MsgBox::Show(const char* message, const char* title)
{
	return Show(message, title, Style::Info, Buttons::OK);
}