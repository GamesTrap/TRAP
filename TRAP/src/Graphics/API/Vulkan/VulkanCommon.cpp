#include "TRAPPCH.h"
#include "VulkanCommon.h"

bool TRAP::Graphics::API::ErrorCheck(const VkResult result, const char* function, const char* file, const int32_t line)
{
	if (result < 0)
	{
		switch (result)
		{
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_OUT_OF_HOST_MEMORY: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_OUT_OF_DEVICE_MEMORY: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_INITIALIZATION_FAILED:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_INITIALIZATION_FAILED: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_DEVICE_LOST:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_DEVICE_LOST: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_MEMORY_MAP_FAILED:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_MEMORY_MAP_FAILED: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_LAYER_NOT_PRESENT: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_EXTENSION_NOT_PRESENT: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_FEATURE_NOT_PRESENT:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_FEATURE_NOT_PRESENT: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_INCOMPATIBLE_DRIVER: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_TOO_MANY_OBJECTS: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_FORMAT_NOT_SUPPORTED: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_SURFACE_LOST_KHR:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_SURFACE_LOST_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_SUBOPTIMAL_KHR:
			TP_ERROR("[Renderer][Vulkan] VK_SUBOPTIMAL_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_OUT_OF_DATE_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: ", function, " @[", file, ':', line, ']');
			break;
		case VK_ERROR_VALIDATION_FAILED_EXT:
			TP_ERROR("[Renderer][Vulkan] VK_ERROR_VALIDATION_FAILED_EXT: ", function, " @[", file, ':', line, ']');
			break;

		default:
			TP_ERROR("[Renderer][Vulkan] Unknown error", function, " @[", file, ':', line, ']');
			break;
		}

		return false;
	}

	return true;
}