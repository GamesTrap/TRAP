#ifndef GAMESTRAP_FRAMEBUFFERTESTS_H
#define GAMESTRAP_FRAMEBUFFERTESTS_H

#include <TRAP.h>

class FrameBufferTests final : public TRAP::Layer
{
public:
    FrameBufferTests();

    void OnAttach() override;
    void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
    void OnImGuiRender() override;

    void OnEvent(TRAP::Events::Event& event) override;

private:
    static bool OnKeyPress(const TRAP::Events::KeyPressEvent& e);

    TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer = nullptr;
    TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer = nullptr;
    TRAP::Ref<TRAP::Graphics::Sampler> m_textureSampler = nullptr;

    TRAP::Ref<TRAP::Graphics::Shader> m_shader = nullptr;
    TRAP::Ref<TRAP::Graphics::Texture> m_texture = nullptr;

    TRAP::Ref<TRAP::Graphics::RenderTarget> m_renderTarget = nullptr;
    TRAP::Ref<TRAP::Graphics::RenderTarget> m_resolveTarget = nullptr;
    bool m_MSAAEnabled = false;

    std::array<f32, 50> m_frameTimeHistory{};
	TRAP::Utils::Timer m_fpsTimer{};
	TRAP::Utils::Timer m_titleTimer{};
};

#endif /*GAMESTRAP_FRAMEBUFFERTESTS_H*/
