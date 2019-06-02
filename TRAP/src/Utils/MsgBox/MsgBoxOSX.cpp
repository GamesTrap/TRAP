#include "TRAPPCH.h"
#include "MsgBox.h"

namespace TRAP::Utils::MsgBox::INTERNAL
{
	NSString* const kOkStr = @"OK";
	NSString* const kCancelStr = @"Cancel";
	NSString* const kYesStr = @"Yes";
	NSString* const kNoStr = @"No";
	NSString* const kQuitStr = @"Quit";

	//-------------------------------------------------------------------------------------------------------------------//

	NSAlertStyle GetAlertStyle(const Style style)
	{
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_12
		switch(style)
		{
		case Style::Info:
			return NSAlertStyleInformational;

		case Style::Warning:
			return NSAlertStyleWarning;

		case Style::Error:
			return NSAlertStyleCritical;

		case Style::Question:
			return NSAlertStyleWarning;

		default:
			return NSAlertStyleInformational;
		}
#else
		switch(style)
		{
		case Style::Info:
			return NSInformationalAlertStyle;

		case Style::Warning:
			return NSWarningAlertStyle;

		case Style::Error:
			return NSCriticalAlertStyle;

		case Style::Question:
			return NSWarningAlertStyle;

		default:
			return NSInformationalAlertStyle;
		}
#endif
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void SetButtons(NSAlert* alert, const Buttons buttons)
	{
		switch(buttons)
		{
			case Buttons::OK:
				[alert addButtonWithTitle:kOkStr];
				break;

			case Buttons::OKCancel:
				[alert addButtonWithTitle:kOkStr];
				[alert addButtonWithTitle:kCancelStr];
				break;

			case Buttons::YesNo:
				[alert addButtonWithTitle:kYesStr];
				[alert addButtonWithTitle:kNoStr];
				break;

			case Buttons::Quit:
				[alert addButtonWithTitle:kQuitStr];
				break;

		default:
			[alert addButtonWithTitle:kOkStr];
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	Selection GetSelection(int index, const Buttons buttons)
	{
		switch(buttons)
		{
		case Buttons::OK:
			return index == NSAlertFirstButtonReturn ? Selection::OK : Selection::None;

		case Buttons::OKCancel:
		{
			if (index == NSAlertFirstButtonReturn)
				return Selection::OK;

			if (index == NSAlertSecondButtonReturn)
				return Selection::Cancel;

			return Selection::None;
		}

		case Buttons::YesNo:
		{
			if (index == NSAlertFirstButtonReturn)
				return Selection::Yes;

			if (index == NSAlertSecondButtonReturn)
				return Selection::No;

			return Selection::None;
		}

		case Buttons::Quit:
			return index == NSAlertFirstButtonReturn ? Selection::Quit : Selection::None;

		default:
			return Selection::None;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::MsgBox::Selection TRAP::Utils::MsgBox::Show(const char* message,
                                                         const char* title,
                                                         Style style,
                                                         Buttons buttons)
{
	NSAlert* alert = [[NSAlert alloc] init];

	[alert setMessageText:[NSString stringWithCString:title encoding:[NSString defaultCStringEncoding]]];
	[alert setInformativeText:[NSString stringWithCString:message encoding:[NSString defaultCStringEncoding]]];

	[alert setAlertStyle:INTERNAL::GetAlertStyle(style)];
	INTERNAL::SetButtons(alert, buttons);

	//Force the alert to appear on top of any other window
	[[alert window] setLevel:NSModalPanelWindowLevel];

	const Selection selection = INTERNAL::GetSelection([alert runModal], buttons);
	[alert release];

	return selection;
}