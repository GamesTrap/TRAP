#ifndef GAMESTRAP_MULTIWINDOWTESTS_H
#define GAMESTRAP_MULTIWINDOWTESTS_H

#include <TRAP.h>

class MultiWindowTests final : public TRAP::Layer
{
public:
	MultiWindowTests();

	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnImGuiRender() override;

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnWindowClose(const TRAP::Events::WindowCloseEvent& e);
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& e);

	TRAP::Utils::Timer m_fpsTimer{};

	TRAP::Scope<TRAP::Graphics::VertexBuffer> m_vertexBuffer = nullptr;
	TRAP::Scope<TRAP::Graphics::IndexBuffer> m_indexBuffer = nullptr;

	bool m_wireFrameMainWindow = false;
	bool m_wireFrameSecondWindow = false;

	TRAP::Scope<TRAP::Window> m_window = nullptr;

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
	bool m_useUBO = false;
};

#endif /*GAMESTRAP_MULTIWINDOWTESTS_H*/
