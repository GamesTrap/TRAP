#include "TRAPPCH.h"

#ifdef TRAP_PLATFORM_LINUX

#include "MsgBox.h"
#include "MsgBoxLinuxX11.h"
#include "Application.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::MsgBox::Selection TRAP::Utils::MsgBox::Show(const char* message,
	const char* title,
	const Style style,
	const Buttons buttons)
{
	if (Application::GetLinuxWindowManager() == Application::LinuxWindowManager::X11)
		return INTERNAL::X11::ShowX11(message, title, style, buttons);
	if (Application::GetLinuxWindowManager() == Application::LinuxWindowManager::Wayland)
	{
		//TODO Implement
		return Selection::None;
	}
	
	return Selection::None;
}

#endif