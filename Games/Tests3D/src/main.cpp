#include <TRAP.h>

#include "Cube3D/Cube3D.h"

class Tests3D final : public TRAP::Application
{
public:
	Tests3D()
	{
		PushLayer(TRAP::MakeScope<Cube3D>());
	}
};

std::unique_ptr<TRAP::Application> TRAP::CreateApplication()
{
	return std::make_unique<Tests3D>();
}