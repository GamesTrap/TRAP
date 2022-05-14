#ifndef TRAP_AFTERMATHTRACKER_H
#define TRAP_AFTERMATHTRACKER_H

namespace TRAP::Graphics::AftermathTracker
{
	[[maybe_unused]] bool Initialize();
	void Shutdown();

	void SetAftermathMarker(void* cmdList, const std::string& name);

#ifdef ENABLE_NSIGHT_AFTERMATH
	void AftermathCall(GFSDK_Aftermath_Result res);

	GFSDK_Aftermath_Result GetCrashDumpStatus(GFSDK_Aftermath_CrashDump_Status* outStatus);
#endif
}

#endif /*TRAP_AFTERMATHTRACKER_H*/