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
#include <mutex>
#include <shared_mutex>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
//ImGUI
#include <imgui.h>
#include <ImGuizmo.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

//TRAP
#include "Core/Base.h"
#include "TRAP_Assert.h"
#include "Log/Log.h"

#ifdef TRAP_PLATFORM_LINUX
#include "Utils/Linux.h"
#elif defined(TRAP_PLATFORM_WINDOWS)
#include "Utils/Win.h"
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

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
#include <vk_mem_alloc.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#ifdef ENABLE_NSIGHT_AFTERMATH
#include <GFSDK_Aftermath.h>
#include <GFSDK_Aftermath_Defines.h>
#include <GFSDK_Aftermath_GpuCrashDump.h>
#endif

#ifdef NVIDIA_REFLEX_AVAILABLE
#include <NvLowLatencyVk.h>
#include <reflexstats.h>
#endif

#endif /*TRAP_TRAPPCH_H*/