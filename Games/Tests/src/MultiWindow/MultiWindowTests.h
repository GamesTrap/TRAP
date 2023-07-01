#ifndef GAMESTRAP_MULTIWINDOWTESTS_H
#define GAMESTRAP_MULTIWINDOWTESTS_H

#include <TRAP.h>

class MultiWindowTests final : public TRAP::Layer
{
public:
	MultiWindowTests();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnImGuiRender() override;

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnWindowClose(const TRAP::Events::WindowCloseEvent& e);
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

	TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer;
	inline static constexpr std::array<uint16_t, 3> TriangleIndices
	{
		0, 1, 2
	};

	bool m_wireFrameMainWindow;
	bool m_wireFrameSecondWindow;

	TRAP::Scope<TRAP::Window> m_window;

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
	bool m_useUBO;
};

#endif /*GAMESTRAP_MULTIWINDOWTESTS_H*/