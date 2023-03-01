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
	void OnTick(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);
	void NewScene();
	void OpenScene();
	void SaveScene();
	void SaveSceneAs();

	void OnOverlayRender();

	void MousePicking();
	void DuplicateEntity();
	void DeleteEntity();
	void SerializeScene(TRAP::Ref<TRAP::Scene> scene, const std::filesystem::path& path);

	void OnScenePlay();
	void OnSceneStop();
	// void OnScenePause();
	// void OnSceneSimulate();

	//UI Panels
	void UIToolbar();

	//UI Icons
	TRAP::Ref<TRAP::Graphics::Texture> m_iconPlay;
	TRAP::Ref<TRAP::Graphics::Texture> m_iconStop;

	//Color RenderTarget
	TRAP::Graphics::RendererAPI::LoadActionsDesc m_renderTargetLoadActions;
	TRAP::Graphics::RendererAPI::RenderTargetDesc m_renderTargetDesc;
	TRAP::Ref<TRAP::Graphics::RenderTarget> m_renderTarget;

	//Viewport
	TRAP::Math::Vec2 m_viewportSize;
	std::array<TRAP::Math::Vec2, 2> m_viewportBounds;
	bool m_viewportFocused;
	bool m_viewportHovered;

	//Gizmo
	int32_t m_gizmoType;
	bool m_enableGizmo;
	bool m_allowViewportCameraEvents;

	//Editor Camera
	TRAP::Graphics::EditorCamera m_editorCamera;
	bool m_startedCameraMovement;

	//Mouse picking
	uint32_t m_leftMouseBtnRepeatCount;
	bool m_entityChanged;
	TRAP::Graphics::RendererAPI::BufferDesc m_mousePickBufferDesc;
	TRAP::Ref<TRAP::Graphics::Buffer> m_mousePickBuffer;
	TRAP::Ref<TRAP::Graphics::RenderTarget> m_IDRenderTarget;

	//Panels
	TRAP::SceneGraphPanel m_sceneGraphPanel;
	std::string m_lastScenePath;

	//Scene
	TRAP::Ref<TRAP::Scene> m_activeScene;
	TRAP::Ref<TRAP::Scene> m_editorScene;

	enum class SceneState
	{
		Edit,
		Play,
		// Pause,
		// Simulate
	};
	SceneState m_sceneState;

	//Other Settings
	bool m_showPhysicsColliders;
};

#endif /*GAMESTRAP_TRAPEDITORLAYER_H*/