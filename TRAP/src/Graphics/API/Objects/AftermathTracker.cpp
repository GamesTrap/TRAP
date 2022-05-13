#include "TRAPPCH.h"
#include "AftermathTracker.h"

#include "FS/FS.h"
#include "Application.h"

std::mutex TRAP::Graphics::AftermathTracker::s_mutex;
std::atomic<uint32_t> TRAP::Graphics::AftermathTracker::s_refCount{0};

void* TRAP::Graphics::AftermathTracker::s_handle = nullptr;
#ifdef ENABLE_NSIGHT_AFTERMATH
PFN_GFSDK_Aftermath_EnableGpuCrashDumps TRAP::Graphics::AftermathTracker::s_enableGPUCrashDumps = nullptr;
PFN_GFSDK_Aftermath_DisableGpuCrashDumps TRAP::Graphics::AftermathTracker::s_disableGPUCrashDumps = nullptr;
PFN_GFSDK_Aftermath_GetCrashDumpStatus TRAP::Graphics::AftermathTracker::s_getGPUCrashDumpStatus = nullptr;
#endif

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::AftermathTracker::AftermathTracker()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererAftermathTrackerPrefix, "Creating AftermathTracker");
#endif

#ifdef ENABLE_NSIGHT_AFTERMATH
    ++s_refCount;
    LoadFunctions();

    AftermathCall(s_enableGPUCrashDumps(GFSDK_Aftermath_Version_API,
                                        GFSDK_Aftermath_GpuCrashDumpWatchedApiFlags_Vulkan,
                                        GFSDK_Aftermath_GpuCrashDumpFeatureFlags_Default,
                                        OnCrashDump, nullptr, nullptr, nullptr));
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::AftermathTracker::~AftermathTracker()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererAftermathTrackerPrefix, "Destroying AftermathTracker");
#endif

#ifdef ENABLE_NSIGHT_AFTERMATH
    AftermathCall(s_disableGPUCrashDumps());

    --s_refCount;
    UnloadFunctions();
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::AftermathTracker::SetAftermathMarker(void* /*cmdList*/, const std::string& /*name*/)
{
#ifdef ENABLE_NSIGHT_AFTERMATH
    //TODO Only used by D3D12/D3D11
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef ENABLE_NSIGHT_AFTERMATH
void TRAP::Graphics::AftermathTracker::AftermathCall(const GFSDK_Aftermath_Result res)
{
    //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

GFSDK_Aftermath_Result TRAP::Graphics::AftermathTracker::GetCrashDumpStatus(GFSDK_Aftermath_CrashDump_Status* outStatus)
{
    return s_getGPUCrashDumpStatus(outStatus);

}
#endif

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::AftermathTracker::OnCrashDump([[maybe_unused]] const void* gpuCrashDump,
                                                   [[maybe_unused]] const uint32_t gpuCrashDumpSize,
                                                   void* /*userData*/)
{
#ifdef ENABLE_NSIGHT_AFTERMATH
    //TODO Use time stamp in file name
    std::string filePath = TRAP::FS::GetDocumentsFolderPath() / TRAP::Application::GetGameName() / "crash-dumps" / "latest.dump";
    std::lock_guard lock(s_mutex);
    std::vector<uint8_t> buffer(gpuCrashDumpSize);
    std::memcpy(buffer.data(), gpuCrashDump, gpuCrashDumpSize);
    TRAP::FS::WriteFile(filePath, buffer);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::AftermathTracker::LoadFunctions()
{
#ifdef ENABLE_NSIGHT_AFTERMATH
    if(!s_enableGPUCrashDumps || !s_disableGPUCrashDumps || !s_getGPUCrashDumpStatus)
    {
#ifdef TRAP_PLATFORM_WINDOWS
	    s_handle LoadLibraryA("GFSDK_Aftermath_Lib.x64.dll");
#elif defined(TRAP_PLATFORM_LINUX)
        s_handle = dlopen("libGFSDK_Aftermath_Lib.x64.so", RTLD_LAZY | RTLD_LOCAL);
#endif
    }

    if(!s_enableGPUCrashDumps)
    {
		s_enableGPUCrashDumps = reinterpret_cast<PFN_GFSDK_Aftermath_EnableGpuCrashDumps>
        (
#ifdef TRAP_PLATFORM_WINDOWS
	        ::GetProcAddress(static_cast<HMODULE>(s_handle), "GFSDK_Aftermath_EnableGpuCrashDumps")
#elif defined(TRAP_PLATFORM_LINUX)
#endif
            dlsym(s_handle, "GFSDK_Aftermath_EnableGpuCrashDumps")
        );
    }
    if(!s_disableGPUCrashDumps)
    {
		s_disableGPUCrashDumps = reinterpret_cast<PFN_GFSDK_Aftermath_DisableGpuCrashDumps>
        (
#ifdef TRAP_PLATFORM_WINDOWS
	        ::GetProcAddress(static_cast<HMODULE>(s_handle), "GFSDK_Aftermath_DisableGpuCrashDumps")
#elif defined(TRAP_PLATFORM_LINUX)
#endif
            dlsym(s_handle, "GFSDK_Aftermath_DisableGpuCrashDumps")
        );
    }
    if(!s_getGPUCrashDumpStatus)
    {
		s_getGPUCrashDumpStatus = reinterpret_cast<PFN_GFSDK_Aftermath_GetCrashDumpStatus>
        (
#ifdef TRAP_PLATFORM_WINDOWS
	        ::GetProcAddress(static_cast<HMODULE>(s_handle), "GFSDK_Aftermath_GetCrashDumpStatus")
#elif defined(TRAP_PLATFORM_LINUX)
#endif
            dlsym(s_handle, "GFSDK_Aftermath_GetCrashDumpStatus")
        );
    }
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::AftermathTracker::UnloadFunctions()
{
#ifdef ENABLE_NSIGHT_AFTERMATH
    if(s_refCount == 0)
    {
#ifdef TRAP_PLATFORM_WINDOWS
        FreeLibrary(static_cast<HMODULE>(s_handle));
#elif defined(TRAP_PLATFORM_LINUX)
        dlclose(s_handle);
#endif

        s_handle = nullptr;
        s_enableGPUCrashDumps = nullptr;
        s_disableGPUCrashDumps = nullptr;
        s_getGPUCrashDumpStatus = nullptr;
    }
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::AftermathTracker> TRAP::Graphics::AftermathTracker::Create()
{
    return TRAP::Ref<TRAP::Graphics::AftermathTracker>(new TRAP::Graphics::AftermathTracker);
}