#ifndef TRAP_AFTERMATHTRACKER_H
#define TRAP_AFTERMATHTRACKER_H

namespace TRAP::Graphics
{
	class AftermathTracker
	{
	public:
		/// <summary>
		/// Create a new buffer from the given description.
		/// </summary>
		/// <param name="desc">Buffer description.</param>
		/// <returns>Created buffer.</returns>
		static TRAP::Ref<AftermathTracker> Create();

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~AftermathTracker();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		AftermathTracker(const AftermathTracker&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		AftermathTracker& operator=(const AftermathTracker&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		AftermathTracker(AftermathTracker&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		AftermathTracker& operator=(AftermathTracker&&) = default;

        void SetAftermathMarker(void* cmdList, const std::string& name);

#ifdef ENABLE_NSIGHT_AFTERMATH
        static void AftermathCall(GFSDK_Aftermath_Result res);

		static GFSDK_Aftermath_Result GetCrashDumpStatus(GFSDK_Aftermath_CrashDump_Status* outStatus);
#endif

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		AftermathTracker();

    private:
        static void OnCrashDump(const void* gpuCrashDump, const uint32_t gpuCrashDumpSize, void* userData);

		static void LoadFunctions();
		static void UnloadFunctions();

        static std::mutex s_mutex;
		static std::atomic<uint32_t> s_refCount;

		static void* s_handle;
#ifdef ENABLE_NSIGHT_AFTERMATH
		static PFN_GFSDK_Aftermath_EnableGpuCrashDumps s_enableGPUCrashDumps;
		static PFN_GFSDK_Aftermath_DisableGpuCrashDumps s_disableGPUCrashDumps;
		static PFN_GFSDK_Aftermath_GetCrashDumpStatus s_getGPUCrashDumpStatus;
#endif
	};
}

#endif /*TRAP_AFTERMATHTRACKER_H*/