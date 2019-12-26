#ifndef _TRAP_WIN_H_
#define _TRAP_WIN_H_

//Target Windows 8 or later
#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0601
#endif
#include <sdkddkver.h>

#ifndef OEMRESOURCE
	#define OEMRESOURCE
#endif

#include <Windows.h>
#include <windowsx.h>

//HACK: Define macros that some windows.h variants don't
#ifndef WM_MOUSEHWHEEL
	#define WM_MOUSEHWHEEL 0x020E
#endif
#ifndef WM_DWMCOMPOSITIONCHANGED
	#define WM_DWMCOMPOSITIONCHANGED 0x031E
#endif
#ifndef WM_COPYGLOBALDATA
	#define WM_COPYGLOBALDATA 0x0049
#endif
#ifndef WM_UNICHAR
	#define WM_UNICHAR 0x0109
#endif
#ifndef UNICODE_NOCHAR
	#define UNICODE_NOCHAR 0xFFFF
#endif
#ifndef WM_DPICHANGED
	#define WM_DPICHANGED 0x02E0
#endif
#ifndef GET_XBUTTON_WPARAM
	#define GET_XBUTTON_WPARAM(w) (HIWORD(w))
#endif
#ifndef EDS_ROTATEDMODE
	#define EDS_ROTATEDMODE 0x00000004
#endif
#ifndef DISPLAY_DEVICE_ACTIVE
	#define DISPLAY_DEVICE_ACTIVE 0x00000001
#endif
#ifndef _WIN32_WINNT_WINBLUE
	#define _WIN32_WINNT_WINBLUE 0x0602
#endif
#ifndef _WIN32_WINNT_WIN8
	#define _WIN32_WINNT_WIN8 0x0602
#endif
#ifndef WM_GETDPISCALEDSIZE
	#define WM_GETDPISCALEDSIZE 0x02e4
#endif
#ifndef USER_DEFAULT_SCREEN_DPI
	#define USER_DEFAULT_SCREEN_DPI 96
#endif
#ifndef OCR_HAND
	#define OCR_HAND 32649
#endif

#endif /*_TRAP_WIN_H_*/