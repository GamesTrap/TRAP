#include <TRAP.h>

#include "SandboxLayer.h" // IWYU pragma: keep
#include "Sandbox2D.h" // IWYU pragma: keep
#include "ParticleSystem2D/ParticleSystem2DLayer.h" // IWYU pragma: keep

class Sandbox final : public TRAP::Application
{
public:
	explicit Sandbox(std::string gameName)
		: Application(std::move(gameName))
	{
		// PushLayer(std::make_unique<SandboxLayer>());
		PushLayer(std::make_unique<Sandbox2D>());
		// PushLayer(std::make_unique<ParticleSystem2DLayer>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication([[maybe_unused]] const std::span<const std::string_view> args)
{
	return std::make_unique<Sandbox>("Sandbox");
}