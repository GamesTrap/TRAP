#include <TRAP.h>

#include "TRAPEditorLayer.h"

class TRAPEditor final : public TRAP::Application
{
public:
	TRAPEditor()
	{
		PushLayer(std::make_unique<TRAPEditorLayer>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication()
{
	return std::make_unique<TRAPEditor>();
}