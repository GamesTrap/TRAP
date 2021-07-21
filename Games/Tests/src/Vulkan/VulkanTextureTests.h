#ifndef _GAMESTRAP_VULKANTEXTURETESTS_H_
#define _GAMESTRAP_VULKANTEXTURETESTS_H_

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

    std::vector<uint8_t> m_vulkanLogoImgData;
    bool m_debugImgVisible;

    TRAP::Graphics::Shader* m_shader;
    TRAP::Graphics::Texture* m_texture;
};

#endif /*_GAMESTRAP_VULKANTEXTURETESTS_H_*/