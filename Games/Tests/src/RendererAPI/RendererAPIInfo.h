#ifndef GAMESTRAP_RENDERERAPIINFO_H
#define GAMESTRAP_RENDERERAPIINFO_H

#include <TRAP.h>

class RendererAPIInfo final : public TRAP::Layer
{
public:
	constexpr RendererAPIInfo();

	void OnAttach() override;
	void OnImGuiRender() override;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr RendererAPIInfo::RendererAPIInfo()
	: Layer("RendererAPIInfo")
{
}

#endif /*GAMESTRAP_RENDERERAPIINFO_H*/