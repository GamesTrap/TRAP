#ifndef GAMESTRAP_VULKANFRAMEBUFFERTESTS_H
#define GAMESTRAP_VULKANFRAMEBUFFERTESTS_H

#include <TRAP.h>

class VulkanFrameBufferTests final : public TRAP::Layer
{
public:
    VulkanFrameBufferTests();

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

    std::array<float, 5 * 4> m_quadVerticesIndexed
	{
		//XYZ UV
		-1.0f, -1.0f, 0.0f,    0.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,    1.0f, 1.0f,
		 1.0f,  1.0f, 0.0f,    1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,    0.0f, 0.0f
	};
    std::array<uint16_t, 6> m_quadIndices
	{
		0, 1, 2, 2, 3, 0
	};

    TRAP::Graphics::Shader* m_shader;
    TRAP::Graphics::Texture* m_texture;

    TRAP::Ref<TRAP::Graphics::RenderTarget> m_renderTarget;
    bool m_renderedFrame = false;

    std::array<float, 50> m_frameTimeHistory{};
	TRAP::Utils::Timer m_fpsTimer{};
	TRAP::Utils::Timer m_titleTimer{};
};

#endif /*GAMESTRAP_VULKANTEXTURETESTS_H*/