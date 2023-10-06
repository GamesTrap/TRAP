#include <TRAP.h>

#include "TRAPEditorLayer.h"

class TRAPEditor final : public TRAP::Application
{
public:
	explicit TRAPEditor(std::string gameName)
		: Application(std::move(gameName))
	{
		PushLayer(std::make_unique<TRAPEditorLayer>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication([[maybe_unused]] const std::span<const std::string_view> args)
{
	return std::make_unique<TRAPEditor>("TRAP-Editor");
}