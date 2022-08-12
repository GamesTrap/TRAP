#ifndef GAMESTRAP_SPIRVTESTS_H
#define GAMESTRAP_SPIRVTESTS_H

#include <TRAP.h>

class SPIRVTests final : public TRAP::Layer
{
public:
	SPIRVTests();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnImGuiRender() override;

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(TRAP::Events::KeyPressEvent& e);
	bool OnFrameBufferResize(TRAP::Events::FrameBufferResizeEvent& e);

    TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer;
    TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer;

    std::array<float, 5 * 4> m_quadVerticesIndexed
	{
		//XYZ UV
		-0.5f, -0.5f, 0.0f,    0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,    1.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,    0.0f, 0.0f
	};
    std::array<uint16_t, 6> m_quadIndices
	{
		0, 1, 2, 2, 3, 0
	};

    std::array<float, 50> m_frameTimeHistory{};
	TRAP::Utils::Timer m_fpsTimer{};
	TRAP::Utils::Timer m_titleTimer{};
};

#endif /*GAMESTRAP_SPIRVTESTS_H*/