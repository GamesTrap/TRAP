#ifndef _GAMESTRAP_VULKANTEXTURETESTS_H_
#define _GAMESTRAP_VULKANTEXTURETESTS_H_

#include <TRAP.h>
#include <Graphics/API/Vulkan/Objects/VulkanTexture.h>

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
    std::vector<TRAP::Ref<TRAP::Graphics::Sampler>> m_textureSamplers;

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
	
    //////////////////////////////////////
	//INTERNAL CODE USE AT YOUR OWN RISK//
	//////////////////////////////////////
    TRAP::Ref<TRAP::Graphics::API::VulkanTexture> m_texture;
	//////////////////////////////////////
};

#endif /*_GAMESTRAP_VULKANTEXTURETESTS_H_*/