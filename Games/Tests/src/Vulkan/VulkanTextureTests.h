#ifndef GAMESTRAP_VULKANTEXTURETESTS_H
#define GAMESTRAP_VULKANTEXTURETESTS_H

#include <TRAP.h>

class VulkanTextureTests final : public TRAP::Layer
{
public:
    VulkanTextureTests();

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
    void OnImGuiRender() override;

    void OnEvent(TRAP::Events::Event& event) override;

private:
    bool OnKeyPress(TRAP::Events::KeyPressEvent& e);

    TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer;
    TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer;
    TRAP::Ref<TRAP::Graphics::Sampler> m_textureSampler;

    bool m_cycleMips;
    uint32_t m_currentMipLevel;
    uint32_t m_maxMipLevel;
    bool m_updateTexture;
    uint32_t m_currentTexture;

    inline static constexpr std::array<float, 5 * 4> QuadVerticesIndexed
	{
		//XYZ UV
		-0.5f, -0.5f, 0.0f,    0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,    1.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,    0.0f, 0.0f
	};
    inline static constexpr std::array<uint16_t, 6> QuadIndices
	{
		0, 1, 2, 2, 3, 0
	};

    TRAP::Ref<TRAP::Graphics::Shader> m_shader;
    TRAP::Ref<TRAP::Graphics::Texture> m_texture;
    TRAP::Scope<TRAP::Image> m_vulkanLogo;
    TRAP::Scope<TRAP::Image> m_vulkanLogoTransparent;
};

#endif /*GAMESTRAP_VULKANTEXTURETESTS_H*/