#ifndef _GAMESTRAP_VULKANMULTIWINDOWTESTS_H_
#define _GAMESTRAP_VULKANMULTIWINDOWTESTS_H_

#include <TRAP.h>

class VulkanMultiWindowTests final : public TRAP::Layer
{
public:
	VulkanMultiWindowTests();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnImGuiRender() override;

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnWindowClose(TRAP::Events::WindowCloseEvent& e);
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

	TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer;
	std::array<uint16_t, 3> m_triangleIndices
	{
		0, 1, 2
	};

	bool m_wireFrameMainWindow;
	bool m_wireFrameSecondWindow;

	TRAP::Scope<TRAP::Window> m_window;
};

#endif /*_GAMESTRAP_VULKANMULTIWINDOWTESTS_H_*/