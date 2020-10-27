#ifndef _TRAP_TRAPPCH_H_
#define _TRAP_TRAPPCH_H_

#include "Core/PlatformDetection.h"

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

//OpenGL
#include "Graphics/API/OpenGL/Loader/OpenGLLoader.h"
#pragma warning(push, 0)
//ImGUI
#include <imgui.h>
#pragma warning(pop)

//TRAP Engine
#include "Core/Base.h"
#include "Log/Log.h"
#include "Utils/Profiler/Instrumentor.h"

#ifdef TRAP_PLATFORM_LINUX
#pragma warning(push, 0)
#include <linux/input.h>
#include <linux/limits.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <sys/select.h>
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
#pragma warning(pop)

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

#endif

#ifdef TRAP_PLATFORM_WINDOWS
#pragma warning(push, 0)
//WinAPI
#include "Utils/Win.h"
#include <Dbt.h>
#include <dwmapi.h>
#include <imm.h>
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
#pragma warning(pop)

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
#endif

//VulkanAPI
#include "Graphics/API/Vulkan/Loader/VulkanLoader.h"

#pragma warning(push, 0)
//SPIRV
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
//SPIRV to GLSL
#include <spirv_glsl.hpp>
#pragma warning(pop)

#endif /*_TRAP_TRAPPCH_H_*/