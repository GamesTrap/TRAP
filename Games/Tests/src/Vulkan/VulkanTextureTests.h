#ifndef GAMESTRAP_VULKANTEXTURETESTS_H
#define GAMESTRAP_VULKANTEXTURETESTS_H

#include <TRAP.h>

class VulkanTextureTests final : public TRAP::Layer
{
public:
    constexpr VulkanTextureTests();

    void OnAttach() override;
    void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
    void OnImGuiRender() override;

    void OnEvent(TRAP::Events::Event& event) override;

private:
    bool OnKeyPress(const TRAP::Events::KeyPressEvent& e);

    TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer = nullptr;
    TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer = nullptr;
    TRAP::Ref<TRAP::Graphics::Sampler> m_textureSampler = nullptr;

    bool m_cycleMips = false;
    u32 m_currentMipLevel = 0;
    u32 m_maxMipLevel = 0;
    bool m_updateTexture = false;
    u32 m_currentTexture = 0;

    static constexpr std::array<f32, 5ull * 4> QuadVerticesIndexed
	{
		//XYZ UV
		-0.5f, -0.5f, 0.0f,    0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,    1.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,    0.0f, 0.0f
	};
    static constexpr std::array<u16, 6> QuadIndices
	{
		0, 1, 2, 2, 3, 0
	};

    TRAP::Ref<TRAP::Graphics::Shader> m_shader = nullptr;
    TRAP::Ref<TRAP::Graphics::Texture> m_texture = nullptr;
    TRAP::Scope<TRAP::Image> m_vulkanLogo = nullptr;
    TRAP::Scope<TRAP::Image> m_vulkanLogoTransparent = nullptr;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr VulkanTextureTests::VulkanTextureTests()
    : Layer("VulkanTextureTests")
{
}

#endif /*GAMESTRAP_VULKANTEXTURETESTS_H*/