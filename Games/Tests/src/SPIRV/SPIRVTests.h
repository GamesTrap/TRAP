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

	constexpr void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& e);

    TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer = nullptr;
    TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer = nullptr;

    std::array<f32, 50u> m_frameTimeHistory{};
	TRAP::Utils::Timer m_fpsTimer{};
	TRAP::Utils::Timer m_titleTimer{};
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr void SPIRVTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&SPIRVTests::OnKeyPress, this));
}

#endif /*GAMESTRAP_SPIRVTESTS_H*/
