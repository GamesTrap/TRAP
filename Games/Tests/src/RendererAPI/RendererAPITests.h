#ifndef GAMESTRAP_RENDERERAPITESTS_H
#define GAMESTRAP_RENDERERAPITESTS_H

#include <TRAP.h>

class RendererAPITests final : public TRAP::Layer
{
public:
	RendererAPITests();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnImGuiRender() override;

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& e);

	TRAP::Utils::Timer m_fpsTimer;


	TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer;
	inline static constexpr std::array<float, 18> TriangleVertices
	{
		//XYZ RGB
		 0.0f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
	};
	inline static constexpr std::array<float, 36> QuadVertices
	{
		//XYZ RGB
		-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f
	};
	inline static constexpr std::array<float, 32> QuadVerticesIndexed
	{
		//XYZ RGB UV
		-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f,    0.0f, 0.0f
	};

	TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer;
	inline static constexpr std::array<uint16_t, 3> TriangleIndices
	{
		0, 1, 2
	};
	inline static constexpr std::array<uint16_t, 6> QuadIndices
	{
		0, 1, 2, 2, 3, 0
	};

	bool m_wireFrame;
	bool m_quad;
	bool m_indexed;
	bool m_vsync;
	uint8_t m_pushConstantOrUBO;

	struct ColorData
	{
		TRAP::Math::Vec3 Color;
	} m_colorData;
	struct SizeMultiplicatorData
	{
		TRAP::Math::Vec3 Multiplier;
	} m_sizeMultiplicatorData;

	TRAP::Utils::Timer m_colorTimer;
	TRAP::Utils::Timer m_vertexTimer;

	TRAP::Scope<TRAP::Graphics::UniformBuffer> m_colorUniformBuffer;
	TRAP::Scope<TRAP::Graphics::UniformBuffer> m_sizeMultiplicatorUniformBuffer;
};

#endif /*GAMESTRAP_RENDERERAPITESTS_H*/