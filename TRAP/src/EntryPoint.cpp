#include "TRAPPCH.h"
#include "EntryPoint.h"
#include "Core/PlatformDetection.h"

void* p;

int main(int32_t argc, char** argv)
{
#ifdef TRAP_PLATFORM_WINDOWS
	SetConsoleOutputCP(CP_UTF8); //Enable UTF-8 Console output
#endif

	const auto app = TRAP::CreateApplication(argc, argv);

	app->Run();

	return 0;
}