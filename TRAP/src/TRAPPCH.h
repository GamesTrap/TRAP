#ifndef _TRAP_TRAPPCH_H_
#define _TRAP_TRAPPCH_H_

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

//OpenGL
#include "Graphics/API/OpenGL/Loader/OpenGLLoader.h"
//VulkanAPI
#include <vulkan/vulkan.h>
//SPIRV
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/ResourceLimits.h>
#include <StandAlone/DirStackFileIncluder.h>
//SPIRV to GLSL
#include <spirv_glsl.hpp>
//ImGUI
#include <imgui.h>
//TRAP Engine
#include "Core.h"
#include "Log/Log.h"
#include "Utils/Debug/Instrumentor.h"

#ifdef TRAP_PLATFORM_LINUX
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
#include <X11/Xresource.h>
#include <X11/cursorfont.h>
#include <X11/Xmd.h>
#include <dlfcn.h>

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

#endif

#ifdef TRAP_PLATFORM_WINDOWS
//WinAPI
#include "Utils/Win.h"
#include <Dbt.h>
#include <dwmapi.h>
#include <imm.h>
//DirectX
#include <dxgiformat.h>
#include <dxgi1_6.h>
#include <d3d12.h>
//XInput
#include <Xinput.h>
//DirectInput
#ifndef DIDFT_OPTIONAL
	#define DIDFT_OPTIONAL 0x80000000
#endif
#ifndef DIRECTINPUT_VERSION
	#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>

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
#endif

#endif /*_TRAP_TRAPPCH_H_*/