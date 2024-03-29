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

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& e);

	TRAP::Utils::Timer m_fpsTimer{};

	TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer = nullptr;
	inline static constexpr std::array<f32, 12ull * 6> m_icoSphereVerticesIndexed
	{
		//XYZ RGB
		-1.0f,                                     (1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f,    0.0f,                                        1.0f, 0.0f, 0.0f,
		 1.0f,                                     (1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f,    0.0f,                                        1.0f, 0.0f, 0.0f,
		-1.0f,                                    -((1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f),  0.0f,                                        1.0f, 0.0f, 0.0f,
		 1.0f,                                    -((1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f),  0.0f,                                        1.0f, 0.0f, 0.0f,

		 0.0f,                                    -1.0f,                                      (1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f,      0.0f, 1.0f, 0.0f,
		 0.0f,                                     1.0f,                                      (1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f,      0.0f, 1.0f, 0.0f,
		 0.0f,                                    -1.0f,                                     -((1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f),    0.0f, 1.0f, 0.0f,
		 0.0f,                                     1.0f,                                     -((1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f),    0.0f, 1.0f, 0.0f,

		 (1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f,   0.0f,                                     -1.0f,                                        0.0f, 0.0f, 1.0f,
		 (1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f,   0.0f,                                      1.0f,                                        0.0f, 0.0f, 1.0f,
		-((1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f), 0.0f,                                     -1.0f,                                        0.0f, 0.0f, 1.0f,
		-((1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f), 0.0f,                                      1.0f,                                        0.0f, 0.0f, 1.0f
	};

	TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer = nullptr;
	inline static constexpr std::array<u16, 20ull * 3> IcosphereIndices
	{
		0, 11, 5,
		0, 5, 1,
		0, 1, 7,
		0, 7, 10,
		0, 10, 11,

		1, 5, 9,
		5, 11, 4,
		11, 10, 2,
		10, 7, 6,
		7, 1, 8,

		3, 9, 4,
		3, 4, 2,
		3, 2, 6,
		3, 6, 8,
		3, 8, 9,

		4, 9, 5,
		2, 4, 11,
		6, 2, 10,
		8, 6, 7,
		9, 8, 1
	};

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

#endif /*GAMESTRAP_ICOSPHERETESTS_H*/