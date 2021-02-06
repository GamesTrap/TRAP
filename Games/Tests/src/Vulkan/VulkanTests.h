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

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnWindowClose(TRAP::Events::WindowCloseEvent& e);
	bool OnKeyPress(TRAP::Events::KeyPressEvent& e);
	
	TRAP::Scope<TRAP::Window> m_window;

	TRAP::Utils::Timer m_fpsTimer;

	TRAP::Ref<TRAP::Graphics::PipelineCache> m_defaultPipelineCache;
	
	TRAP::Ref<TRAP::Graphics::VertexBuffer> m_vertexBuffer;
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

	bool m_wireFrame;
	bool m_quad;
};

#endif /*_GAMESTRAP_VULKANTESTS_H_*/