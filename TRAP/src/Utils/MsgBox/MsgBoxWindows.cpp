#include "TRAPPCH.h"
#include "MsgBox.h"

//Internal MsgBox Stuff
namespace TRAP::Utils::MsgBox::INTERNAL
{
	uint32_t GetIcon(const Style style)
	{
		switch(style)
		{
		case Style::Info:
			return MB_ICONINFORMATION;

		case Style::Warning:
			return MB_ICONWARNING;

		case Style::Error:
			return MB_ICONERROR;

		case Style::Question:
			return MB_ICONQUESTION;

		default:
			return MB_ICONINFORMATION;
		}
	}

	uint32_t GetButtons(const Buttons buttons)
	{
		switch(buttons)
		{
		case Buttons::OK:
		case Buttons::Quit: //There's no 'Quit' button on Windows so use OK
			return MB_OK;

		case Buttons::OKCancel:
			return MB_OKCANCEL;

		case Buttons::YesNo:
			return MB_YESNO;

		default:
			return MB_OK;
		}
	}

	Selection GetSelection(const int32_t response, const Buttons buttons)
	{
		switch(response)
		{
		case IDOK:
			return buttons == Buttons::Quit ? Selection::Quit : Selection::OK;

		case IDCANCEL:
			return Selection::Cancel;

		case IDYES:
			return Selection::Yes;

		case IDNO:
			return Selection::No;

		default:
			return Selection::None;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::MsgBox::Selection TRAP::Utils::MsgBox::Show(const char* message,
														 const char* title,
														 const Style style,
														 const Buttons buttons)
{
	uint32_t flags = MB_TASKMODAL;

	flags |= INTERNAL::GetIcon(style);
	flags |= INTERNAL::GetButtons(buttons);

	return INTERNAL::GetSelection(MessageBoxA(nullptr, message, title, flags), buttons);
}