#ifndef GAMESTRAP_RENDERERAPITESTS_H
#define GAMESTRAP_RENDERERAPITESTS_H

#include <TRAP.h>

class RendererAPITests final : public TRAP::Layer
{
public:
	RendererAPITests();

	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnImGuiRender() override;

	constexpr void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& e);

	TRAP::Utils::Timer m_fpsTimer{};

	TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer = nullptr;
	TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer = nullptr;

	bool m_wireFrame = false;
	bool m_quad = false;
	bool m_indexed = false;
	bool m_vsync = TRAP::Application::GetWindow()->GetVSync();
	u8 m_pushConstantOrUBO = 0u;

	struct ColorData
	{
		TRAP::Math::Vec3 Color;
	} m_colorData{};
	struct SizeMultiplicatorData
	{
		TRAP::Math::Vec3 Multiplier;
	} m_sizeMultiplicatorData{};

	TRAP::Utils::Timer m_colorTimer{};
	TRAP::Utils::Timer m_vertexTimer{};

	TRAP::Scope<TRAP::Graphics::UniformBuffer> m_colorUniformBuffer = nullptr;
	TRAP::Scope<TRAP::Graphics::UniformBuffer> m_sizeMultiplicatorUniformBuffer = nullptr;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr void RendererAPITests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&RendererAPITests::OnKeyPress, this));
}

#endif /*GAMESTRAP_RENDERERAPITESTS_H*/
