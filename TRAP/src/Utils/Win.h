#ifndef TRAP_WIN_H
#define TRAP_WIN_H

#ifndef NOMINMAX
	#define NOMINMAX
#endif

//Target Windows 8 or later
#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0602
#endif
#include <sdkddkver.h>

#ifndef OEMRESOURCE
	#define OEMRESOURCE
#endif

#define WIN32_LEAN_AND_MEAN
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
	#define _WIN32_WINNT_WINBLUE 0x0603
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

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
//WinAPI
#include <Dbt.h>
#include <dwmapi.h>
#include <imm.h>
#include <KnownFolders.h>
#include <ShlObj_core.h>
#include <Shobjidl.h>
//XInput
#include <Xinput.h>
//DirectInput
#ifndef DIDFT_OPTIONAL
	#define DIDFT_OPTIONAL 0x80000000
#endif
#ifndef DIRECTINPUT_VERSION
	#define DIRECTINPUT_VERSION 0x0800
#endif
#include <basetsd.h>
#include <dinput.h>
#include <WinSock2.h>
#include <winsock.h>
#include <WS2tcpip.h>
#include <shellapi.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#ifdef far
	#undef far
#endif
#ifdef min
	#undef min
#endif
#ifdef max
	#undef max
#endif
#ifdef CreateWindow
	#undef CreateWindow
#endif
#ifdef GetWindowStyle
	#undef GetWindowStyle
#endif
#ifdef GetWindowExStyle
	#undef GetWindowExStyle
#endif
#ifdef IsMaximized
	#undef IsMaximized
#endif
#ifdef IsMinimized
	#undef IsMinimized
#endif
#ifdef IsRestored
	#undef IsRestored
#endif
#ifdef near
	#undef near
#endif
#ifdef DELETE
	#undef DELETE
#endif
#ifdef isnan
	#undef isnan
#endif
#ifdef isinf
	#undef isinf
#endif
#ifdef log2
	#undef log2
#endif
#ifdef GetKeyboardLayoutName
	#undef GetKeyboardLayoutName
#endif
#ifdef GetMessage
	#undef GetMessage
#endif
#ifdef CreateDirectory
	#undef CreateDirectory
#endif
#ifdef DeleteFile
	#undef DeleteFile
#endif
#ifdef BeginUpdateResource
	#undef BeginUpdateResource
#endif
#ifdef EndUpdateResource
	#undef EndUpdateResource
#endif
#ifdef LoadLibrary
	#undef LoadLibrary
#endif

#endif /*TRAP_WIN_H*/