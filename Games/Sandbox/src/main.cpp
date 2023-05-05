#include <TRAP.h>

#include "SandboxLayer.h"
#include "Sandbox2D.h"
#include "ParticleSystem2D/ParticleSystem2DLayer.h"

class Sandbox final : public TRAP::Application
{
public:
	explicit Sandbox(std::string gameName)
		: Application(std::move(gameName), 1007)
	{
		// PushLayer(std::make_unique<SandboxLayer>());
		PushLayer(std::make_unique<Sandbox2D>());
		// PushLayer(std::make_unique<ParticleSystem2DLayer>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication([[maybe_unused]] const std::vector<std::string_view>& args)
{
	return std::make_unique<Sandbox>("Sandbox");
}