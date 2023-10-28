#ifndef GAMESTRAP_SPIRVTESTS_H
#define GAMESTRAP_SPIRVTESTS_H

#include <TRAP.h>

class SPIRVTests final : public TRAP::Layer
{
public:
	SPIRVTests();

	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnImGuiRender() override;

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& e);

    TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer = nullptr;
    TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer = nullptr;

    inline static constexpr std::array<f32, 5ull * 4> QuadVerticesIndexed
	{
		//XYZ UV
		-0.5f, -0.5f, 0.0f,    0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,    1.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,    0.0f, 0.0f
	};
    inline static constexpr std::array<u16, 6> QuadIndices
	{
		0, 1, 2, 2, 3, 0
	};

    std::array<f32, 50> m_frameTimeHistory{};
	TRAP::Utils::Timer m_fpsTimer{};
	TRAP::Utils::Timer m_titleTimer{};
};

#endif /*GAMESTRAP_SPIRVTESTS_H*/