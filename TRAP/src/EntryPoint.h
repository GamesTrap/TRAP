#ifndef _TRAP_ENTRYPOINT_H_
#define _TRAP_ENTRYPOINT_H_

#ifdef TRAP_PLATFORM_WINDOWS
extern "C"
{
	inline _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;		  //Utilize NVIDIA GPU
	inline _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1; //Utilize AMD GPU
}
#endif

inline std::unique_ptr<TRAP::Log> logger = std::make_unique<TRAP::Log>();

extern std::unique_ptr<TRAP::Application> TRAP::CreateApplication();

int main()
{
	const auto app = TRAP::CreateApplication();
	app->Run();
}

#endif /*_TRAP_ENTRYPOINT_H_*/