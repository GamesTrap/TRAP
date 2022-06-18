#include "TRAPPCH.h"
#include "EntryPoint.h"
#include "Core/PlatformDetection.h"

int main(int32_t argc, char** argv)
{
#ifdef TRAP_PLATFORM_WINDOWS
	SetConsoleOutputCP(CP_UTF8); //Enable UTF-8 Console output
#endif

	TP_PROFILE_BEGIN_SESSION("Startup", "TRAPProfile-Startup.json");
	const auto app = TRAP::CreateApplication(argc, argv);
	TP_PROFILE_END_SESSION();

	TP_PROFILE_BEGIN_SESSION("Runtime", "TRAPProfile-Runtime.json");
	app->Run();
	TP_PROFILE_END_SESSION();

	return 0;
}