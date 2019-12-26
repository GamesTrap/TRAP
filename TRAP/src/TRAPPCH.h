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

//VulkanAPI
#include <vulkan/vulkan.h>
//SPIRV
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/ResourceLimits.h>
#include <StandAlone/DirStackFileIncluder.h>
//GLAD
#include <glad/glad.h>
//SPIRV to GLSL
#include <spirv_glsl.hpp>
//ImGUI
#include <imgui.h>
//TRAP Engine
#include "Core.h"

#ifdef TRAP_PLATFORM_LINUX
#include <linux/input.h>
#include <linux/limits.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#endif

#ifdef TRAP_PLATFORM_WINDOWS
//WinAPI
#include "Utils/Win.h"
#include <Dbt.h>
#include <dwmapi.h>
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

#undef far
#undef min
#undef max
#undef CreateWindow
#undef GetWindowStyle
#undef GetWindowExStyle
#undef near
#undef DELETE
#endif

#endif /*_TRAP_TRAPPCH_H_*/