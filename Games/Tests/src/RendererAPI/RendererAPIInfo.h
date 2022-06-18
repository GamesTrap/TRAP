#ifndef GAMESTRAP_RENDERERAPIINFO_H
#define GAMESTRAP_RENDERERAPIINFO_H

#include <TRAP.h>

class RendererAPIInfo final : public TRAP::Layer
{
public:
	RendererAPIInfo();

	void OnAttach() override;
	void OnImGuiRender() override;
};

#endif /*GAMESTRAP_RENDERERAPIINFO_H*/