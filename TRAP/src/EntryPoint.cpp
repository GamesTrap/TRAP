#include "TRAPPCH.h"
#include "EntryPoint.h"

int32_t main()
{
	const auto app = TRAP::CreateApplication();
	app->Run();

	return 0;
}