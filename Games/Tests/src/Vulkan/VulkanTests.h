#ifndef GAMESTRAP_VULKANTESTS_H
#define GAMESTRAP_VULKANTESTS_H

#include <TRAP.h>

class VulkanTests final : public TRAP::Layer
{
public:
	VulkanTests();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnImGuiRender() override;

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(TRAP::Events::KeyPressEvent& e);

	TRAP::Utils::Timer m_fpsTimer;


	TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer;
	std::array<float, 18> m_triangleVertices
	{
		//XYZ RGB
		 0.0f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
	};
	std::array<float, 36> m_quadVertices
	{
		//XYZ RGB
		-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f
	};
	std::array<float, 32> m_quadVerticesIndexed
	{
		//XYZ RGB UV
		-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f,    0.0f, 0.0f
	};

	TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer;
	std::array<uint16_t, 3> m_triangleIndices
	{
		0, 1, 2
	};
	std::array<uint16_t, 6> m_quadIndices
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

#endif /*GAMESTRAP_VULKANTESTS_H*/