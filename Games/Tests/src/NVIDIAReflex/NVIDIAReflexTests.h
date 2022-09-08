#ifndef GAMESTRAP_NVIDIAREFLEXTESTS_H
#define GAMESTRAP_NVIDIAREFLEXTESTS_H

#include <TRAP.h>

class NVIDIAReflexTests final : public TRAP::Layer
{
public:
	NVIDIAReflexTests();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnImGuiRender() override;

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(TRAP::Events::KeyPressEvent& e);

	TRAP::Utils::Timer m_fpsTimer;

	TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer;
	std::array<float, 12 * 6> m_icoSphereVerticesIndexed
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

	TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer;
	std::array<uint16_t, 20 * 3> m_icosphereIndices
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

	TRAP::Scope<TRAP::Graphics::UniformBuffer> m_cameraUBO;

	struct CameraUBOData
	{
		TRAP::Math::Mat4 Projection;
		TRAP::Math::Mat4 View;
		TRAP::Math::Mat4 Model;
	};
	TRAP::SceneCamera m_camera;
	TRAP::TransformComponent m_cameraTransform;

	TRAP::Ref<TRAP::Graphics::Shader> m_shader;

	TRAP::Graphics::LatencyMode m_latencyMode;

	std::array<float, 50> m_totalHistory;
	std::array<float, 50> m_simulationDeltaHistory;
	std::array<float, 50> m_renderDeltaHistory;
	std::array<float, 50> m_presentDeltaHistory;
	std::array<float, 50> m_driverDeltaHistory;
	std::array<float, 50> m_OSRenderQueueDeltaHistory;
	std::array<float, 50> m_GPURenderDeltaHistory;
	TRAP::Utils::Timer m_updateLatencyTimer;
};

#endif /*GAMESTRAP_NVIDIAREFLEXTESTS_H*/