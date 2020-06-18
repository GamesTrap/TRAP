#include <TRAP.h>

#include "SandboxLayer.h"
#include "Sandbox2D.h"
#include "ParticleSystem2D/ParticleSystem2DLayer.h"

class Sandbox final : public TRAP::Application
{
public:
	Sandbox()
	{		
		PushLayer(std::make_unique<SandboxLayer>());
		//PushLayer(std::make_unique<Sandbox2D>());
		//PushLayer(std::make_unique<ParticleSystem2DLayer>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication()
{
	return std::make_unique<Sandbox>();
}