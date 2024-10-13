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

    constexpr void OnEvent(TRAP::Events::Event& event) override;

private:
    bool OnKeyPress(const TRAP::Events::KeyPressEvent& e);

    TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer = nullptr;
    TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer = nullptr;
    TRAP::Ref<TRAP::Graphics::Sampler> m_textureSampler = nullptr;

    bool m_cycleMips = false;
    u32 m_currentMipLevel = 0u;
    u32 m_maxMipLevel = 0u;
    bool m_updateTexture = false;
    u32 m_currentTexture = 0u;

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

//-------------------------------------------------------------------------------------------------------------------//

constexpr void VulkanTextureTests::OnEvent(TRAP::Events::Event& event)
{
    const TRAP::Events::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&VulkanTextureTests::OnKeyPress, this));
}

#endif /*GAMESTRAP_VULKANTEXTURETESTS_H*/
