#ifndef TRAP_TRAPPCH_H
#define TRAP_TRAPPCH_H

#include "Core/PlatformDetection.h"

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
#include <mutex>
#include <shared_mutex>
#include <bit>
#include <source_location>
#include <csignal>
#include <ranges>
#include <regex>

#include "Core/Backports.h"
#include "Core/Types.h"

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/

#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/ostream.h>
#include <fmt/std.h>

//Tracy - Profiler
#include <tracy/Tracy.hpp>

#ifndef TRAP_HEADLESS_MODE
//ImGUI
#include <imgui.h>
#include <ImGuizmo.h>
#endif /*TRAP_HEADLESS_MODE*/

//TRAP
#include "Core/Base.h"
#include "TRAP_Assert.h"
#include "Log/Log.h"
#include "Utils/NumericCasts.h"

#ifdef TRAP_PLATFORM_LINUX
#include "Utils/Linux.h"
#elif defined(TRAP_PLATFORM_WINDOWS)
#include "Utils/Win.h"
#endif /*TRAP_PLATFORM_LINUX*/

//VulkanAPI
#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"

//SPIRV
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>
//SPIRV to GLSL
#include <spirv_glsl.hpp>

//VulkanMemoryAllocator
#include <vk_mem_alloc.h>

#ifdef ENABLE_NSIGHT_AFTERMATH
#include <GFSDK_Aftermath.h>
#include <GFSDK_Aftermath_Defines.h>
#include <GFSDK_Aftermath_GpuCrashDump.h>
#endif /*ENABLE_NSIGHT_AFTERMATH*/

#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#endif /*TRAP_TRAPPCH_H*/
