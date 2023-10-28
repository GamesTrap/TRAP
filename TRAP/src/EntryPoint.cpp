#include "TRAPPCH.h"

#include "Application.h"
#include "Utils/CrashHandler/CrashHandler.h"

#ifndef TRAP_UNITTESTS

int main(const i32 argc, const char* const* const argv)
{
#ifdef TRAP_PLATFORM_WINDOWS
	SetConsoleOutputCP(CP_UTF8); //Enable UTF-8 Console output
#endif /*TRAP_PLATFORM_WINDOWS*/

	TRAP::Utils::CrashHandler crashHandler{};

	const std::vector<std::string_view> args(argv, std::next(argv, NumericCast<isize>(argc)));
	const auto app = TRAP::CreateApplication(args);

	app->Run();

	return 0;
}

#endif /*TRAP_UNITTESTS*/