#ifndef _TRAP_VULKANCOMMON_H_
#define _TRAP_VULKANCOMMON_H_

#include "TRAPPCH.h"

namespace TRAP::Graphics::API
{
	bool ErrorCheck(VkResult result, const char* function, const char* file, int32_t line);
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::Graphics::API::ErrorCheck(const VkResult result, const char* function, const char* file, const int32_t line)
{
	if (result < 0)
	{
		switch (result)
		{
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_OUT_OF_HOST_MEMORY: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_OUT_OF_DEVICE_MEMORY: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_INITIALIZATION_FAILED:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_INITIALIZATION_FAILED: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_DEVICE_LOST:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_DEVICE_LOST: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_MEMORY_MAP_FAILED:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_MEMORY_MAP_FAILED: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_LAYER_NOT_PRESENT: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_EXTENSION_NOT_PRESENT: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_FEATURE_NOT_PRESENT:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_FEATURE_NOT_PRESENT: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_INCOMPATIBLE_DRIVER: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_TOO_MANY_OBJECTS: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_FORMAT_NOT_SUPPORTED: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_SURFACE_LOST_KHR:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_SURFACE_LOST_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_SUBOPTIMAL_KHR:
			TRAP_ERROR("[Renderer][Vulkan] VK_SUBOPTIMAL_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_OUT_OF_DATE_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_VALIDATION_FAILED_EXT:
			TRAP_ERROR("[Renderer][Vulkan] VK_ERROR_VALIDATION_FAILED_EXT: ", function, " @[", file, ':', line, ']');
			break;

		default:
			TRAP_ERROR("[Renderer][Vulkan] Unknown error", function, " @[", file, ':', line, ']');
			break;
		}

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

namespace TRAP::Graphics
{
#ifdef TRAP_DEBUG
	#define VkCall(x) ::TRAP::Graphics::API::ErrorCheck(x, #x, __FILE__, __LINE__)
#else
	#define VkCall(x) x;
#endif
}

#endif /*_TRAP_VULKANCOMMON_H_*/