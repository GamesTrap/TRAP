#include "TRAPPCH.h"
#include "Application.h"
#include "Core/PlatformDetection.h"

int main(const int32_t argc, const char* const* const argv)
{
#ifdef TRAP_PLATFORM_WINDOWS
	SetConsoleOutputCP(CP_UTF8); //Enable UTF-8 Console output
#endif

	const std::vector<std::string_view> args(argv, std::next(argv, static_cast<std::ptrdiff_t>(argc)));
	const auto app = TRAP::CreateApplication(args);

	app->Run();

	return 0;
}