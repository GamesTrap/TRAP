#ifndef _GAMESTRAP_VULKANTESTS_H_
#define _GAMESTRAP_VULKANTESTS_H_

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
	bool OnWindowClose(TRAP::Events::WindowCloseEvent& e);
	bool OnKeyPress(TRAP::Events::KeyPressEvent& e);
	
	TRAP::Scope<TRAP::Window> m_window;

	TRAP::Utils::Timer m_fpsTimer;

	struct UniformBufferObject
	{
		alignas(16) TRAP::Math::Mat4 Model;
		alignas(16)	TRAP::Math::Mat4 View;
		alignas(16) TRAP::Math::Mat4 Proj;
	};
	TRAP::Scope<TRAP::Graphics::UniformBuffer> m_uniformBuffer;
	
	TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer;
	std::array<float, 18> m_triangleVertices
	{
		//XYZ RGB
		 -0.0f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f
	};
	std::array<float, 36> m_quadVertices
	{
		//XYZ RGB
		 -0.5f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
		 0.5f, 0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
	};
	std::array<float, 36> m_quadVerticesIndexed
	{
		//XYZ RGB
		-0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
		 -0.5f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
		 0.5f, 0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
	};

	TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer;
	std::array<uint16_t, 3> m_triangleIndices
	{
		0, 1, 2
	};
	std::array<uint16_t, 6> m_quadIndices
	{
		0, 1, 2, 0, 2, 3
	};

	bool m_wireFrame;
	bool m_quad;
	bool m_indexed;
	bool m_ubo;
	bool m_vsync;
};

#endif /*_GAMESTRAP_VULKANTESTS_H_*/