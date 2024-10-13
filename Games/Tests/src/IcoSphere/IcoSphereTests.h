#ifndef GAMESTRAP_ICOSPHERETESTS_H
#define GAMESTRAP_ICOSPHERETESTS_H

#include <TRAP.h>

class IcoSphereTests final : public TRAP::Layer
{
public:
	IcoSphereTests();

	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnImGuiRender() override;

	constexpr void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& e);

	TRAP::Utils::Timer m_fpsTimer{};

	TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer = nullptr;
	TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer = nullptr;

	bool m_wireFrame = false;
	bool m_vsync = TRAP::Application::GetWindow()->GetVSync();

	TRAP::Scope<TRAP::Graphics::UniformBuffer> m_cameraUBO = nullptr;

	struct CameraUBOData
	{
		TRAP::Math::Mat4 Projection;
		TRAP::Math::Mat4 View;
		TRAP::Math::Mat4 Model;
	};
	f32 m_FOV = 45.0f;
	f32 m_rotationSpeed = 20.0f;
	TRAP::SceneCamera m_camera{};
	TRAP::TransformComponent m_cameraTransform{};

	TRAP::Ref<TRAP::Graphics::Shader> m_shader = nullptr;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr void IcoSphereTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&IcoSphereTests::OnKeyPress, this));
}

#endif /*GAMESTRAP_ICOSPHERETESTS_H*/
