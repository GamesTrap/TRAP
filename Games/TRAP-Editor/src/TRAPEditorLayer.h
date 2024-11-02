#ifndef GAMESTRAP_TRAPEDITORLAYER_H
#define GAMESTRAP_TRAPEDITORLAYER_H

#include <TRAP.h>
#include <ImGuizmo.h>

#include "Panels/SceneGraphPanel.h"

class TRAPEditorLayer final : public TRAP::Layer
{
public:
	TRAPEditorLayer();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnTick(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
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
	TRAP::Ref<TRAP::Graphics::Texture> m_iconPlay = nullptr;
	TRAP::Ref<TRAP::Graphics::Texture> m_iconStop = nullptr;

	//Color RenderTarget
	TRAP::Graphics::RendererAPI::LoadActionsDesc m_renderTargetLoadActions{};
	TRAP::Graphics::RendererAPI::RenderTargetDesc m_renderTargetDesc{};
	TRAP::Ref<TRAP::Graphics::RenderTarget> m_renderTarget = nullptr;
	TRAP::Ref<TRAP::Graphics::RenderTarget> m_resolveRenderTarget = nullptr;

	//Viewport
	TRAP::Math::Vec2 m_viewportSize{};
	std::array<TRAP::Math::Vec2, 2u> m_viewportBounds{};
	bool m_viewportFocused = false;
	bool m_viewportHovered = false;
	TRAP::Graphics::AntiAliasing m_cachedAAMode = TRAP::Graphics::AntiAliasing::Off;
	TRAP::Graphics::SampleCount m_cachedAASampleCount = TRAP::Graphics::SampleCount::Two;

	//Gizmo
	i32 m_gizmoType = ImGuizmo::OPERATION::TRANSLATE;
	bool m_enableGizmo = false;
	bool m_allowViewportCameraEvents = false;

	//Editor Camera
	TRAP::Graphics::EditorCamera m_editorCamera{45.0f, 16.0f / 9.0f, 0.1f};
	bool m_startedCameraMovement = false;

	//Mouse picking
	u32 m_leftMouseBtnRepeatCount = 0u;
	bool m_entityChanged = false;
	TRAP::Graphics::RendererAPI::BufferDesc m_mousePickBufferDesc{};
	TRAP::Ref<TRAP::Graphics::Buffer> m_mousePickBuffer = nullptr;
	TRAP::Ref<TRAP::Graphics::RenderTarget> m_IDRenderTarget = nullptr;

	//Panels
	TRAP::SceneGraphPanel m_sceneGraphPanel{};
	std::string m_lastScenePath{};

	//Scene
	TRAP::Ref<TRAP::Scene> m_activeScene = nullptr;
	TRAP::Ref<TRAP::Scene> m_editorScene = nullptr;

	enum class SceneState : u8
	{
		Edit,
		Play,
		// Pause,
		// Simulate
	};
	SceneState m_sceneState = SceneState::Edit;

	//Other Settings
	bool m_showPhysicsColliders = false;
};

#endif /*GAMESTRAP_TRAPEDITORLAYER_H*/
