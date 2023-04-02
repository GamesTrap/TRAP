#ifndef GAMESTRAP_COMPUTETESTS_H
#define GAMESTRAP_COMPUTETESTS_H

#include <TRAP.h>

class ComputeTests final : public TRAP::Layer
{
public:
    ComputeTests();

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;

private:
    TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer;
    TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer;
    TRAP::Ref<TRAP::Graphics::Sampler> m_textureSampler;

    inline static constexpr std::array<float, 5ull * 4> m_quadVerticesIndexed
	{
		//XYZ UV
		-0.5f, -0.5f, 0.0f,    0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,    1.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,    0.0f, 0.0f
	};
    inline static constexpr std::array<uint16_t, 6> m_quadIndices
	{
		0, 1, 2, 2, 3, 0
	};

    TRAP::Ref<TRAP::Graphics::Texture> m_colTex;
    TRAP::Ref<TRAP::Graphics::Texture> m_compTex;
};

#endif /*GAMESTRAP_COMPUTETESTS_H*/