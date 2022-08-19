#ifndef GAMESTRAP_TRAPEDITORLAYER_H
#define GAMESTRAP_TRAPEDITORLAYER_H

#include <TRAP.h>
#include "Panels/SceneGraphPanel.h"

class TRAPEditorLayer final : public TRAP::Layer
{
public:
	TRAPEditorLayer();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnTick() override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);
	void NewScene();
	void OpenScene();
	void SaveScene();
	void SaveSceneAs();

	// TRAP::Graphics::OrthographicCameraController m_cameraController;

	TRAP::Graphics::RendererAPI::LoadActionsDesc m_renderTargetLoadActions;
	TRAP::Graphics::RendererAPI::RenderTargetDesc m_renderTargetDesc;
	TRAP::Ref<TRAP::Graphics::RenderTarget> m_renderTarget;
	TRAP::Math::Vec2 m_viewportSize;
	bool m_viewportFocused;
	bool m_viewportHovered;

	TRAP::Ref<TRAP::Scene> m_activeScene;
	TRAP::Entity m_squareEntity;
	TRAP::Entity m_cameraEntity;

	int32_t m_gizmoType;

	//Panels
	TRAP::SceneGraphPanel m_sceneGraphPanel;
	std::string m_lastScenePath;
};

#endif /*GAMESTRAP_TRAPEDITORLAYER_H*/