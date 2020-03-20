#include "TRAPPCH.h"
#include "EntryPoint.h"

int main()
{
	TRAP::Log::Get();
	
	TP_PROFILE_BEGIN_SESSION("Startup", "TRAPProfile-Startup.json");	
	const auto app = TRAP::CreateApplication();
	TP_PROFILE_END_SESSION();
	
	TP_PROFILE_BEGIN_SESSION("Runtime", "TRAPProfile-Runtime.json");
	app->Run();
	TP_PROFILE_END_SESSION();
	
	return 0;
}