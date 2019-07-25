#ifndef _TRAP_TRAPPCH_H_
#define _TRAP_TRAPPCH_H_

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <set>
#include <filesystem>
#include <optional>

#include <string>
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

#include <vulkan/vulkan.h>
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/ResourceLimits.h>
#include <StandAlone/DirStackFileIncluder.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>

#ifdef TRAP_PLATFORM_WINDOWS
#include <dxgiformat.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include "Utils/Win.h"

#undef near
#undef far
#endif

#include "Core.h"

#endif /*_TRAP_TRAPPCH_H_*/