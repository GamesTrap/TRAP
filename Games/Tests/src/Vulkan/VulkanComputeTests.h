#ifndef GAMESTRAP_VULKANCOMPUTETESTS_H
#define GAMESTRAP_VULKANCOMPUTETESTS_H

#include <TRAP.h>

class VulkanComputeTests final : public TRAP::Layer
{
public:
    VulkanComputeTests();

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
    void OnImGuiRender() override;

    void OnEvent(TRAP::Events::Event& event) override;

private:
    bool OnKeyPress(TRAP::Events::KeyPressEvent& e);

    static TRAP::Scope<TRAP::Graphics::Texture> PrepareTextureTarget(uint32_t width, uint32_t height);

    TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer;
    TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer;
    TRAP::Ref<TRAP::Graphics::Sampler> m_textureSampler;

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

    TRAP::Graphics::Shader* m_texShader;
    TRAP::Scope<TRAP::Graphics::Texture> m_colorTextureUAV;
    TRAP::Scope<TRAP::Graphics::Texture> m_computeTarget;
    TRAP::Scope<TRAP::Image> m_vulkanLogo;

    bool m_disabled;
    bool m_sharpen;
    bool m_emboss;
    bool m_edgedetect;

    std::array<float, 50> m_frameTimeHistory{};
	TRAP::Utils::Timer m_fpsTimer{};
	TRAP::Utils::Timer m_titleTimer{};
};

#endif /*GAMESTRAP_VULKANCOMPUTETESTS_H*/