/*
volk

Copyright (C) 2018-2019, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
Report bugs and download new versions at https://github.com/zeux/volk

This library is distributed under the MIT License. See notice at the end of this file.

Modified by: Jan "GamesTrap" Schuerkamp
*/
#ifndef TRAP_VULKANLOADER_H
#define TRAP_VULKANLOADER_H

#if defined(VULKAN_H_) && !defined(VK_NO_PROTOTYPES)
    #error To use VulkanLoader, you need to define VK_NO_PROTOTYPES before including vulkan.h
#endif

/* VULKANLOADER_GENERATE_VERSION_DEFINE */
#define VULKANLOADER_HEADER_VERSION 175
/* VULKANLOADER_GENERATE_VERSION_DEFINE */

#ifndef VK_NO_PROTOTYPES
    #define VK_NO_PROTOTYPES
#endif

#ifndef VULKAN_H_
    #ifdef VK_USE_PLATFORM_WIN32_KHR
        #include <vulkan/vk_platform.h>
        #include <vulkan/vulkan_core.h>

        /*
		When VK_USE_PLATFORM_WIN32_KHR is defined, instead of including vulkan.h directly, we include individual parts of the SDK
		This is necessary to avoid including <windows.h> which is very heavy - it takes 200ms to parse without WIN32_LEAN_AND_MEAN
		and 100ms to parse with it. vulkan_win32.h only needs a few symbols that are easy to redefine ourselves.
		*/
		typedef unsigned long DWORD;
		typedef const wchar_t* LPCWSTR;
		typedef void* HANDLE;
		typedef struct HINSTANCE__* HINSTANCE;
		typedef struct HWND__* HWND;
		typedef struct HMONITOR__* HMONITOR;
		typedef struct _SECURITY_ATTRIBUTES SECURITY_ATTRIBUTES;

        #include <vulkan/vulkan_win32.h>

        #ifdef VK_ENABLE_BETA_EXTENSIONS
            #include <vulkan/vulkan_beta.h>
        #endif
    #else
        #include <vulkan/vulkan.h>
    #endif
#endif

//Disable VK_NVX_image_view_handle because SDK 140 introduced a change that can't be used with prior versions */
#if VK_HEADER_VERSION < 140
    #undef VK_NVX_image_view_handle
#endif

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
#ifdef __cplusplus
extern "C"
{
#endif

struct VkDeviceTable;

/// <summary>
/// Initialize library by loading Vulkan loader; call this function before creating the Vulkan instance.
/// </summary>
/// <returns>VK_SUCCESS on success and VK_ERROR_INITIALIZATION_FAILED otherwise.</returns>
VkResult VkInitialize();

/// <summary>
/// Initialize library by providing a custom handler to load global symbols.
/// This function can be used instead of VkInitialize.
/// The handler function pointer will be asked to load global Vulkan symbols which require no instance
/// (such as vkCreateInstance, vkEnumerateInstance*and vkEnumerateInstanceVersion if available).
/// </summary>
/// <param name="handler">Function pointer for custom loading</param>
void VkInitializeCustom(PFN_vkGetInstanceProcAddr handler);

/// <summary>
/// Get Vulkan instance version supported by the Vulkan loader, or 0 if Vulkan isn't supported
/// </summary>
/// <returns>0 if VkInitialize wasn't called or failed.</returns>
uint32_t VkGetInstanceVersion();

/// <summary>
/// Load global function pointers using application-created VkInstance; call this function after creating the Vulkan instance.
/// </summary>
/// <param name="instance"></param>
void VkLoadInstance(VkInstance instance);

/// <summary>
/// Load global function pointers using application-created VkInstance; call this function after creating the Vulkan instance.
/// Skips loading device-based function pointers, requires usage of VkLoadDevice afterwards.
/// </summary>
/// <param name="instance">VkInstance</param>
void VkLoadInstanceOnly(VkInstance instance);

/// <summary>
/// Load global function pointers using application-created VkDevice; call this function after creating the Vulkan device.
/// Note: this is not suitable for applications that want to use multiple VkDevice objects concurrently.
/// </summary>
/// <param name="device">VkDevice</param>
void VkLoadDevice(VkDevice device);

/// <summary>
/// Retrieve the last VkInstance for which global function pointers have been loaded via VkLoadInstance().
/// </summary>
/// <returns>VkInstance, or VK_NULL_HANDLE if VkLoadInstance() has not been called.</returns>
VkInstance VkGetLoadedInstance();

/// <summary>
/// Retrieve the last VkDevice for which global function pointers have been loaded via VkLoadDevice().
/// </summary>
/// <returns>VkDevice, or VK_NULL_HANDLE if VkLoadDevice() has not been called.</returns>
VkDevice VkGetLoadedDevice();

/// <summary>
/// Load function pointers using application-created VkDevice into a table.
/// Application should use function pointers from that table instead of using global function pointers.
/// </summary>
/// <param name="table">Pointer where to store loaded functions into</param>
/// <param name="device">Device from which function pointers should be loaded</param>
void VkLoadDeviceTable(struct VkDeviceTable* table, VkDevice device);

//Device-specific function pointer table
struct VkDeviceTable
{
	/* VULKANLOADER_GENERATE_DEVICE_TABLE */
	/* VULKANLOADER_GENERATE_DEVICE_TABLE */
};

/* VULKANLOADER_GENERATE_PROTOTYPES_H */
/* VULKANLOADER_GENERATE_PROTOTYPES_H */

#ifdef __cplusplus
}
#endif

#endif

// For Visual Studio IntelliSense.
#if defined(__cplusplus) && defined(__INTELLISENSE__)
#define VULKANLOADER_IMPLEMENTATION
#endif

#ifdef VULKANLOADER_IMPLEMENTATION
#undef VULKANLOADER_IMPLEMENTATION

#ifdef _WIN32
	typedef const char* LPCSTR;
	typedef struct HINSTANCE__* HINSTANCE;
	typedef HINSTANCE HMODULE;
	#ifdef _WIN64
		typedef __int64 (__stdcall* FARPROC)(void);
	#else
		typedef int (__stdcall* FARPROC)(void);
	#endif
#else
    #include <dlfcn.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32
    __declspec(dllimport) HMODULE __stdcall LoadLibraryA(LPCSTR);
    __declspec(dllimport) FARPROC __stdcall GetProcAddress(HMODULE, LPCSTR);
#endif

static VkInstance loadedInstance = VK_NULL_HANDLE;
static VkDevice loadedDevice = VK_NULL_HANDLE;

static void VkGenLoadLoader(void* context, PFN_vkVoidFunction (*load)(void*, const char*));
static void VkGenLoadInstance(void* context, PFN_vkVoidFunction (*load)(void*, const char*));
static void VkGenLoadDevice(void* context, PFN_vkVoidFunction (*load)(void*, const char*));
static void VkGenLoadDeviceTable(struct VkDeviceTable* table, void* context, PFN_vkVoidFunction (*load)(void*, const char*));

static PFN_vkVoidFunction vkGetInstanceProcAddrStub(void* context, const char* name)
{
	return vkGetInstanceProcAddr((VkInstance)context, name);
}

static PFN_vkVoidFunction vkGetDeviceProcAddrStub(void* context, const char* name)
{
	return vkGetDeviceProcAddr((VkDevice)context, name);
}

VkResult VkInitialize()
{
#if defined(_WIN32)
	HMODULE module = LoadLibraryA("vulkan-1.dll");
	if (!module)
		return VK_ERROR_INITIALIZATION_FAILED;

	//Note: function pointer is cast through void function pointer to silence cast-function-type warning on gcc8
	vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)(void(*)(void))GetProcAddress(module, "vkGetInstanceProcAddr");
#else
	void* module = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
	if (!module)
		module = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
	if (!module)
		return VK_ERROR_INITIALIZATION_FAILED;

	vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(module, "vkGetInstanceProcAddr");
#endif

	VkGenLoadLoader(NULL, vkGetInstanceProcAddrStub);

	return VK_SUCCESS;
}

void VkInitializeCustom(PFN_vkGetInstanceProcAddr handler)
{
	vkGetInstanceProcAddr = handler;

	VkGenLoadLoader(NULL, vkGetInstanceProcAddrStub);
}

uint32_t VkGetInstanceVersion()
{
#if defined(VK_VERSION_1_1)
	uint32_t apiVersion = 0;
	if (vkEnumerateInstanceVersion && vkEnumerateInstanceVersion(&apiVersion) == VK_SUCCESS)
		return apiVersion;
#endif

	if (vkCreateInstance)
		return VK_API_VERSION_1_0;

	return 0;
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

VkInstance VkGetLoadedInstance()
{
	return loadedInstance;
}

void VkLoadDevice(VkDevice device)
{
	loadedDevice = device;
	VkGenLoadDevice(device, vkGetDeviceProcAddrStub);
}

VkDevice VkGetLoadedDevice()
{
	return loadedDevice;
}

void VkLoadDeviceTable(struct VkDeviceTable* table, VkDevice device)
{
	VkGenLoadDeviceTable(table, device, vkGetDeviceProcAddrStub);
}

static void VkGenLoadLoader(void* context, PFN_vkVoidFunction (*load)(void*, const char*))
{
	/* VULKANLOADER_GENERATE_LOAD_LOADER */
	/* VULKANLOADER_GENERATE_LOAD_LOADER */
}

static void VkGenLoadInstance(void* context, PFN_vkVoidFunction (*load)(void*, const char*))
{
	/* VULKANLOADER_GENERATE_LOAD_INSTANCE */
	/* VULKANLOADER_GENERATE_LOAD_INSTANCE */
}

static void VkGenLoadDevice(void* context, PFN_vkVoidFunction (*load)(void*, const char*))
{
	/* VULKANLOADER_GENERATE_LOAD_DEVICE */
	/* VULKANLOADER_GENERATE_LOAD_DEVICE */
}

static void VkGenLoadDeviceTable(struct VkDeviceTable* table, void* context, PFN_vkVoidFunction (*load)(void*, const char*))
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
#endif

/* VULKANLOADER_GENERATE_PROTOTYPES_C */
/* VULKANLOADER_GENERATE_PROTOTYPES_C */

#ifdef __GNUC__
    #pragma GCC visibility pop
#endif

#ifdef __cplusplus
}
#endif
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#endif //VULKANLOADER_IMPLEMENTATION

/*
Copyright (c) 2018-2019 Arseny Kapoulkine

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
