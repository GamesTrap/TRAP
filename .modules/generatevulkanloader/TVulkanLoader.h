/*
volk

Copyright (C) 2018-2024, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
Report bugs and download new versions at https://github.com/zeux/volk

This library is distributed under the MIT License. See notice at the end of this file.

Modified by: Jan "GamesTrap" Schuerkamp
*/
#ifndef TRAP_VULKANLOADER_H
#define TRAP_VULKANLOADER_H

#include <optional>

#include "Core/Types.h"
#include "Utils/Optional.h"

#if defined(VULKAN_H_) && !defined(VK_NO_PROTOTYPES)
    #error To use VulkanLoader, you need to define VK_NO_PROTOTYPES before including vulkan.h
#endif /*VULKAN_H_ && !VK_NO_PROTOTYPES*/

/* VULKANLOADER_GENERATE_VERSION_DEFINE */
#define VULKANLOADER_HEADER_VERSION 175
/* VULKANLOADER_GENERATE_VERSION_DEFINE */

#ifndef VK_NO_PROTOTYPES
    #define VK_NO_PROTOTYPES
#endif /*VK_NO_PROTOTYPES*/

#ifndef VULKAN_H_
    #ifdef VK_USE_PLATFORM_WIN32_KHR
        #include <vulkan/vk_platform.h>
        #include <vulkan/vulkan_core.h>

        /*
		When VK_USE_PLATFORM_WIN32_KHR is defined, instead of including vulkan.h directly, we include individual parts of the SDK
		This is necessary to avoid including <windows.h> which is very heavy - it takes 200ms to parse without WIN32_LEAN_AND_MEAN
		and 100ms to parse with it. vulkan_win32.h only needs a few symbols that are easy to redefine ourselves.
		*/
		using DWORD = unsigned long;
		using LPCWSTR = const wchar_t*;
		using HANDLE = void*;
		using HINSTANCE = struct HINSTANCE__*;
		using HWND = struct HWND__*;
		using HMONITOR = struct HMONITOR__*;
		using SECURITY_ATTRIBUTES = struct _SECURITY_ATTRIBUTES;

        #include <vulkan/vulkan_win32.h>

        #ifdef VK_ENABLE_BETA_EXTENSIONS
            #include <vulkan/vulkan_beta.h>
        #endif /*VK_ENABLE_BETA_EXTENSIONS*/
    #else
        #include <vulkan/vulkan.h>
    #endif
#endif /*VULKAN_H_*/

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/

struct VkDeviceTable;

/// @brief Initialize library by loading Vulkan loader; call this function before creating the Vulkan instance.
/// @return VK_SUCCESS on success and VK_ERROR_INITIALIZATION_FAILED otherwise.
[[nodiscard]] VkResult VkInitialize();

/// @brief Initialize library by providing a custom handler to load global symbols.
///        This function can be used instead of VkInitialize.
///        The handler function pointer will be asked to load global Vulkan symbols which require no instance
///        (such as vkCreateInstance, vkEnumerateInstance*and vkEnumerateInstanceVersion if available).
/// @param handler Function pointer for custom loading.
void VkInitializeCustom(PFN_vkGetInstanceProcAddr handler);

/// @brief Finalize library by unloading Vulkan loader and resetting global symbols to nullptr.
void VkFinalize();

/// @brief Get Vulkan instance version supported by the Vulkan loader, or an empty optional if Vulkan isn't supported.
/// @return Empty optional if VkInitialize wasn't called or failed.
[[nodiscard]] TRAP::Optional<u32> VkGetInstanceVersion();

/// @brief Load global function pointers using application-created VkInstance; call this function after creating the Vulkan instance.
/// @param instance VkInstance to load functions with.
void VkLoadInstance(VkInstance instance);

/// @brief Load global function pointers using application-created VkInstance; call this function after creating the Vulkan instance.
///        Skips loading device-based function pointers, requires usage of VkLoadDevice afterwards.
/// @param instance VkInstance to load functions with.
void VkLoadInstanceOnly(VkInstance instance);

/// @brief Load global function pointers using application-created VkDevice; call this function after creating the Vulkan device.
/// @param device VkDevice to load functions with.
/// @note This is not suitable for applications that want to use multiple VkDevice objects concurrently.
void VkLoadDevice(VkDevice device);

/// @brief Retrieve the last VkInstance for which global function pointers have been loaded via VkLoadInstance().
/// @return VkInstance, or VK_NULL_HANDLE if VkLoadInstance() has not been called.
[[nodiscard]] VkInstance VkGetLoadedInstance() noexcept;

/// @brief Retrieve the last VkDevice for which global function pointers have been loaded via VkLoadDevice().
/// @return VkDevice, or VK_NULL_HANDLE if VkLoadDevice() has not been called.
[[nodiscard]] VkDevice VkGetLoadedDevice() noexcept;

/// @brief Load function pointers using application-created VkDevice into a table.
///        Application should use function pointers from that table instead of using global function pointers.
/// @param table Table where to store loaded functions into.
/// @param device Device from which function pointers should be loaded.
void VkLoadDeviceTable(VkDeviceTable& table, VkDevice device);

//Device-specific function pointer table
struct VkDeviceTable
{
	/* VULKANLOADER_GENERATE_DEVICE_TABLE */
	/* VULKANLOADER_GENERATE_DEVICE_TABLE */
};

/* VULKANLOADER_GENERATE_PROTOTYPES_H */
/* VULKANLOADER_GENERATE_PROTOTYPES_H */

#endif /*TRAP_VULKANLOADER_H*/

// For Visual Studio IntelliSense.
#if defined(__cplusplus) && defined(__INTELLISENSE__)
#define VULKANLOADER_IMPLEMENTATION
#endif /*__cplusplus && __INTELLISENSE__*/

#ifdef VULKANLOADER_IMPLEMENTATION
#undef VULKANLOADER_IMPLEMENTATION

#ifdef _WIN32
	using LPCSTR = const char*;
	using HINSTANCE = struct HINSTANCE__*;
	using HMODULE = HINSTANCE;
	#if defined(_MINWINDEF_)
		/*minwindef.h defined FARPROC, and attempting to redefine it may conflict with -Wstrict-prototypes*/
	#elif defined(_WIN64)
		typedef __int64 (__stdcall* FARPROC)(void);
	#else
		typedef i32 (__stdcall* FARPROC)(void);
	#endif
#else
    #include <dlfcn.h>
#endif

#include "Utils/DynamicLoading/DynamicLoading.h"

static void* loadedModule = NULL;
static VkInstance loadedInstance = VK_NULL_HANDLE;
static VkDevice loadedDevice = VK_NULL_HANDLE;

using VkGenLoaderFunction = PFN_vkVoidFunction (*)(void* const, const std::string_view);

static void VkGenLoadLoader(void* const context, VkGenLoaderFunction load);
static void VkGenLoadInstance(VkInstance instance, VkGenLoaderFunction load);
static void VkGenLoadDevice(void* const context, VkGenLoaderFunction load);
static void VkGenLoadDeviceTable(VkDeviceTable& table, VkDevice device, VkGenLoaderFunction load);

static PFN_vkVoidFunction vkGetInstanceProcAddrStub(void* const context, const std::string_view name)
{
	return vkGetInstanceProcAddr(static_cast<VkInstance>(context), name.data());
}

static PFN_vkVoidFunction vkGetDeviceProcAddrStub(void* const context, const std::string_view name)
{
	return vkGetDeviceProcAddr(static_cast<VkDevice>(context), name.data());
}

static PFN_vkVoidFunction nullProcAddrStub([[maybe_unused]] void* const context, [[maybe_unused]] const std::string_view name) noexcept
{
	return nullptr;
}

[[nodiscard]] VkResult VkInitialize()
{
#if defined(_WIN32)
	HMODULE module = static_cast<HMODULE>(TRAP::Utils::DynamicLoading::LoadLibrary("vulkan-1.dll"));
#else
	void* module = TRAP::Utils::DynamicLoading::LoadLibrary("libvulkan.so.1");
	if (!module)
		module = TRAP::Utils::DynamicLoading::LoadLibrary("libvulkan.so");
#endif

	if (!module)
		return VK_ERROR_INITIALIZATION_FAILED;

	vkGetInstanceProcAddr = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_vkGetInstanceProcAddr>(static_cast<void*>(module), "vkGetInstanceProcAddr");

	loadedModule = module;
	VkGenLoadLoader(nullptr, vkGetInstanceProcAddrStub);

	return VK_SUCCESS;
}

void VkInitializeCustom(PFN_vkGetInstanceProcAddr handler)
{
	vkGetInstanceProcAddr = handler;

	loadedModule = nullptr;
	VkGenLoadLoader(nullptr, vkGetInstanceProcAddrStub);
}

void VkFinalize()
{
	if(loadedModule)
		TRAP::Utils::DynamicLoading::FreeLibrary(loadedModule);

	vkGetInstanceProcAddr = nullptr;
	VkGenLoadLoader(nullptr, nullProcAddrStub);
	VkGenLoadInstance(VK_NULL_HANDLE, nullProcAddrStub);
	VkGenLoadDevice(VK_NULL_HANDLE, nullProcAddrStub);

	loadedModule = nullptr;
	loadedInstance = VK_NULL_HANDLE;
	loadedDevice = VK_NULL_HANDLE;
}

[[nodiscard]] TRAP::Optional<u32> VkGetInstanceVersion()
{
#if defined(VK_VERSION_1_1)
	u32 apiVersion = 0;
	if (vkEnumerateInstanceVersion && vkEnumerateInstanceVersion(&apiVersion) == VK_SUCCESS)
		return apiVersion;
#endif /*VK_VERSION_1_1*/

	if (vkCreateInstance)
		return VK_API_VERSION_1_0;

	return TRAP::NullOpt;
}

void VkLoadInstance(VkInstance instance)
{
	loadedInstance = instance;
	VkGenLoadInstance(instance, vkGetInstanceProcAddrStub);
	VkGenLoadDevice(instance, vkGetInstanceProcAddrStub);
}

void VkLoadInstanceOnly(VkInstance instance)
{
    loadedInstance = instance;
    VkGenLoadInstance(instance, vkGetInstanceProcAddrStub);
}

[[nodiscard]] VkInstance VkGetLoadedInstance() noexcept
{
	return loadedInstance;
}

void VkLoadDevice(VkDevice device)
{
	loadedDevice = device;
	VkGenLoadDevice(device, vkGetDeviceProcAddrStub);
}

[[nodiscard]] VkDevice VkGetLoadedDevice() noexcept
{
	return loadedDevice;
}

void VkLoadDeviceTable(VkDeviceTable& table, VkDevice device)
{
	VkGenLoadDeviceTable(table, device, vkGetDeviceProcAddrStub);
}

static void VkGenLoadLoader(void* const context, VkGenLoaderFunction load)
{
	/* VULKANLOADER_GENERATE_LOAD_LOADER */
	/* VULKANLOADER_GENERATE_LOAD_LOADER */
}

static void VkGenLoadInstance(VkInstance instance, VkGenLoaderFunction load)
{
	/* VULKANLOADER_GENERATE_LOAD_INSTANCE */
	/* VULKANLOADER_GENERATE_LOAD_INSTANCE */
}

static void VkGenLoadDevice(void* const context, VkGenLoaderFunction load)
{
	/* VULKANLOADER_GENERATE_LOAD_DEVICE */
	/* VULKANLOADER_GENERATE_LOAD_DEVICE */
}

static void VkGenLoadDeviceTable(VkDeviceTable& table, VkDevice device, VkGenLoaderFunction load)
{
	/* VULKANLOADER_GENERATE_LOAD_DEVICE_TABLE */
	/* VULKANLOADER_GENERATE_LOAD_DEVICE_TABLE */
}

#ifdef __GNUC__
    #ifdef VULKANLOADER_DEFAULT_VISIBILITY
        #pragma GCC visibility push(default)
    #else
        #pragma GCC visibility push(hidden)
    #endif
#endif /*__GNUC__*/

/* VULKANLOADER_GENERATE_PROTOTYPES_C */
/* VULKANLOADER_GENERATE_PROTOTYPES_C */

#ifdef __GNUC__
    #pragma GCC visibility pop
#endif /*__GNUC__*/

#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#endif /*VULKANLOADER_IMPLEMENTATION*/

/*
Copyright (c) 2018-2024 Arseny Kapoulkine

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Modified by: Jan "GamesTrap" Schuerkamp
*/
