#ifndef GAMESTRAP_FRAMEBUFFERTESTS_H
#define GAMESTRAP_FRAMEBUFFERTESTS_H

#include <TRAP.h>

class FrameBufferTests final : public TRAP::Layer
{
public:
    FrameBufferTests();

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
    void OnImGuiRender() override;

    void OnEvent(TRAP::Events::Event& event) override;

private:
    static bool OnKeyPress(TRAP::Events::KeyPressEvent& e);

    TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer;
    TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer;
    TRAP::Ref<TRAP::Graphics::Sampler> m_textureSampler;

    inline static constexpr std::array<float, 5ull * 4> QuadVerticesIndexed
	{
		//XYZ UV
		-1.0f, -1.0f, 0.0f,    0.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,    1.0f, 1.0f,
		 1.0f,  1.0f, 0.0f,    1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,    0.0f, 0.0f
	};
    inline static constexpr std::array<uint16_t, 6> QuadIndices
	{
		0, 1, 2, 2, 3, 0
	};

    TRAP::Ref<TRAP::Graphics::Shader> m_shader;
    TRAP::Ref<TRAP::Graphics::Texture> m_texture;

    TRAP::Ref<TRAP::Graphics::RenderTarget> m_renderTarget;
    TRAP::Ref<TRAP::Graphics::RenderTarget> m_resolveTarget;
    bool m_renderedFrame = false;
    bool m_MSAAEnabled = false;

    std::array<float, 50> m_frameTimeHistory{};
	TRAP::Utils::Timer m_fpsTimer{};
	TRAP::Utils::Timer m_titleTimer{};
};

#endif /*GAMESTRAP_FRAMEBUFFERTESTS_H*/