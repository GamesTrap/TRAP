#ifndef _GAMESTRAP_TRAPEDITORLAYER_H_
#define _GAMESTRAP_TRAPEDITORLAYER_H_

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
	
	//TRAP::Graphics::OrthographicCameraController m_cameraController;
	
	TRAP::Scope<TRAP::Graphics::FrameBuffer> m_frameBuffer;
	TRAP::Math::Vec2 m_viewportSize;
	bool m_viewportFocused;
	bool m_viewportHovered;

	TRAP::Ref<TRAP::Scene> m_activeScene;
	TRAP::Entity m_squareEntity;
	TRAP::Entity m_cameraEntity;

	//Panels
	TRAP::SceneGraphPanel m_sceneGraphPanel;
	std::string m_lastScenePath;
};

#endif /*_GAMESTRAP_TRAPEDITORLAYER_H_*/