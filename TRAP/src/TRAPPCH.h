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

//VulkanAPI
#include <vulkan/vulkan.h>
//SPIRV
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/ResourceLimits.h>
#include <StandAlone/DirStackFileIncluder.h>
//GLAD
#include <glad/glad.h>
//GLFW
#include <GLFW/glfw3.h>
//SPIRV to GLSL
#include <spirv_glsl.hpp>
//ImGUI
#include <imgui.h>
//TRAP Engine
#include "Core.h"

#ifdef TRAP_PLATFORM_WINDOWS
//DirectX
#include <dxgiformat.h>
#include <dxgi1_6.h>
#include <d3d12.h>
//WinAPI
#include "Utils/Win.h"

#undef far
#undef min
#undef max
#endif

#endif /*_TRAP_TRAPPCH_H_*/