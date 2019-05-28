#include <TRAP.h>

#include "SandboxLayer.h"

class Sandbox final : public TRAP::Application
{
public:
	Sandbox()
	{		
		PushLayer(std::make_unique<SandboxLayer>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication()
{
	return std::make_unique<Sandbox>();
}