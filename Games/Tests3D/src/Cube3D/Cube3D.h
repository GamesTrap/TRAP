#ifndef _GAMESTRAP_TITLETESTS_H_
#define _GAMESTRAP_TITLETESTS_H_

#include <TRAP.h>

#include "../Cameras/Perspective/PerspectiveCamera.h"
#include "../Cameras/Perspective/PerspectiveCameraController.h"

class Cube3D final : public TRAP::Layer
{
public:
	Cube3D();

	void OnAttach() override;
	void OnDetach() override;
	void OnImGuiRender() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);
	
private:
	TRAP::Scope<TRAP::Graphics::VertexArray> m_vertexArray;
	
	PerspectiveCameraController m_camera;
	
	TRAP::Math::Vec3 m_cubePosition;
	TRAP::Math::Vec3 m_cubeRotation;
	TRAP::Math::Vec3 m_cubeScale;

	std::array<float, 50> m_frameTimeHistory;
	TRAP::Utils::Timer m_titleTimer;

	bool m_ignoreImGui;

	std::vector<std::string> m_shaderNames;
	uint32_t m_currentShader;
	bool m_wireFrame;
};

#endif /*_GAMESTRAP_TITLETESTS_H_*/