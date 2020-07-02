#ifndef _GAMESTRAP_TRAPEDITORLAYER_H_
#define _GAMESTRAP_TRAPEDITORLAYER_H_

#include <TRAP.h>

class TRAPEditorLayer final : public TRAP::Layer
{
public:
	TRAPEditorLayer();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	TRAP::Graphics::OrthographicCameraController m_cameraController;
	
	TRAP::Scope<TRAP::Graphics::FrameBuffer> m_frameBuffer;
	TRAP::Math::Vec2 m_viewportSize;
	bool m_viewportFocused;
	bool m_viewportHovered;
};

#endif /*_GAMESTRAP_TRAPEDITORLAYER_H_*/