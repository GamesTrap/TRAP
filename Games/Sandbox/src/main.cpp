#include <TRAP.h>

#include "SandboxLayer.h"
#include "Sandbox2D.h"
#include "ParticleSystem2D/ParticleSystem2DLayer.h"

class Sandbox final : public TRAP::Application
{
public:
	Sandbox(const std::string& gameName)
		: Application(gameName)
	{
		PushLayer(TRAP::MakeScope<SandboxLayer>());
		// PushLayer(TRAP::MakeScope<Sandbox2D>());
		//PushLayer(TRAP::MakeScope<ParticleSystem2DLayer>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication()
{
	return std::make_unique<Sandbox>("Sandbox");
}