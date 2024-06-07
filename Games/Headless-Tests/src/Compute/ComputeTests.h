#ifndef GAMESTRAP_COMPUTETESTS_H
#define GAMESTRAP_COMPUTETESTS_H

#include <TRAP.h>

class ComputeTests final : public TRAP::Layer
{
public:
    constexpr ComputeTests();

    void OnAttach() override;
    void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;

private:
    TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer = nullptr;
    TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer = nullptr;
    TRAP::Ref<TRAP::Graphics::Sampler> m_textureSampler = nullptr;

    TRAP::Ref<TRAP::Graphics::Texture> m_colTex = nullptr;
    TRAP::Ref<TRAP::Graphics::Texture> m_compTex = nullptr;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr ComputeTests::ComputeTests()
    : Layer("HeadlessComputeTests")
{
}

#endif /*GAMESTRAP_COMPUTETESTS_H*/
