#include "TRAPPCH.h"

#include "Application.h"
#include "Core/PlatformDetection.h"
#include "Utils/CrashHandler/CrashHandler.h"

#ifndef TRAP_UNITTESTS

int main(const int32_t argc, const char* const* const argv)
{
#ifdef TRAP_PLATFORM_WINDOWS
	SetConsoleOutputCP(CP_UTF8); //Enable UTF-8 Console output
#endif /*TRAP_PLATFORM_WINDOWS*/

	TRAP::Utils::CrashHandler crashHandler{};

	const std::vector<std::string_view> args(argv, std::next(argv, NumericCast<std::ptrdiff_t>(argc)));
	const auto app = TRAP::CreateApplication(args);

	app->Run();

	return 0;
}

#endif /*TRAP_UNITTESTS*/