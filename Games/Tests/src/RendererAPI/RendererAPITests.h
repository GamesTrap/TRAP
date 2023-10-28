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

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& e);

	TRAP::Utils::Timer m_fpsTimer{};

	TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer = nullptr;
	inline static constexpr std::array<f32, 18> TriangleVertices
	{
		//XYZ RGB
		 0.0f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
	};
	inline static constexpr std::array<f32, 36> QuadVertices
	{
		//XYZ RGB
		-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f
	};
	inline static constexpr std::array<f32, 32> QuadVerticesIndexed
	{
		//XYZ RGB UV
		-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f,    0.0f, 0.0f
	};

	TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer = nullptr;
	inline static constexpr std::array<u16, 3> TriangleIndices
	{
		0, 1, 2
	};
	inline static constexpr std::array<u16, 6> QuadIndices
	{
		0, 1, 2, 2, 3, 0
	};

	bool m_wireFrame = false;
	bool m_quad = false;
	bool m_indexed = false;
	bool m_vsync = TRAP::Application::GetWindow()->GetVSync();
	u8 m_pushConstantOrUBO = 0;

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

#endif /*GAMESTRAP_RENDERERAPITESTS_H*/