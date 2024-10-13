#ifndef GAMESTRAP_COMPUTETESTS_H
#define GAMESTRAP_COMPUTETESTS_H

#include <TRAP.h>

class ComputeTests final : public TRAP::Layer
{
public:
    ComputeTests();

    void OnAttach() override;
    void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
    void OnImGuiRender() override;

    constexpr void OnEvent(TRAP::Events::Event& event) override;

private:
    bool OnKeyPress(const TRAP::Events::KeyPressEvent& e);

    TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer = nullptr;
    TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer = nullptr;
    TRAP::Ref<TRAP::Graphics::Sampler> m_textureSampler = nullptr;

    TRAP::Ref<TRAP::Graphics::Texture> m_colTex = nullptr;
    TRAP::Ref<TRAP::Graphics::Texture> m_compTex = nullptr;

    bool m_disabled = true;
    bool m_sharpen = false;
    bool m_emboss = false;
    bool m_edgedetect = false;

    bool m_reset = false;

    std::array<f32, 50u> m_frameTimeHistory{};
	TRAP::Utils::Timer m_fpsTimer{};
	TRAP::Utils::Timer m_titleTimer{};
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr void ComputeTests::OnEvent(TRAP::Events::Event& event)
{
    const TRAP::Events::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&ComputeTests::OnKeyPress, this));
}

#endif /*GAMESTRAP_COMPUTETESTS_H*/
