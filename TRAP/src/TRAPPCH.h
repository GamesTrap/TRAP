#ifndef TRAP_TRAPPCH_H
#define TRAP_TRAPPCH_H

#include "Core/PlatformDetection.h"
#include "TRAP_Assert.h"

#ifdef TRAP_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
#endif

//C++ STL
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <set>
#include <filesystem>
#include <optional>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <thread>
#include <chrono>
#include <ctime>
#include <cstdio>
#include <iomanip>
#include <cstring>
#include <deque>
#include <future>
#include <cctype>
#include <any>
#include <climits>
#include <forward_list>
#include <random>
#include <queue>
#include <tuple>
#include <cassert>
#include <numeric>
#include <variant>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
//ImGUI
#include <imgui.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

//TRAP
#include "Core/Base.h"
#include "TRAP_Assert.h"
#include "Log/Log.h"
#include "Utils/Profiler/Instrumentor.h"

#ifdef TRAP_PLATFORM_LINUX
#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
#include <linux/input.h>
#include <linux/limits.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <sys/select.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/extensions/Xrandr.h>
#include <X11/XKBlib.h>
#include <X11/extensions/Xinerama.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/shape.h>
#include <X11/Xresource.h>
#include <X11/cursorfont.h>
#include <X11/Xmd.h>
#include <dlfcn.h>
#include <arpa/inet.h>
#include <signal.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#ifdef None
	#undef None
#endif
#ifdef Bool
	#undef Bool
#endif
#ifdef Always
	#undef Always
#endif
#ifdef True
	#undef True
#endif
#ifdef False
	#undef False
#endif
#ifdef KeyPress
	#undef KeyPress
#endif
#ifdef KeyRelease
	#undef KeyRelease
#endif
#ifdef GrayScale
	#undef GrayScale
#endif
#ifdef Status
	#undef Status
#endif
#ifdef BadRequest
	#undef BadRequest
#endif
#ifdef Success
	#undef Success
#endif

#endif

#ifdef TRAP_PLATFORM_WINDOWS
#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
//WinAPI
#include "Utils/Win.h"
#include <Dbt.h>
#include <dwmapi.h>
#include <imm.h>
#include <KnownFolders.h>
#include <ShlObj_core.h>
//DirectX
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
#endif

//VulkanAPI
#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
//SPIRV
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
//SPIRV to GLSL
#include <spirv_glsl.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#endif /*TRAP_TRAPPCH_H*/