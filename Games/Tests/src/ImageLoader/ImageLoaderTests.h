#ifndef GAMESTRAP_IMAGELOADERTESTS_H
#define GAMESTRAP_IMAGELOADERTESTS_H

#include <TRAP.h>

class ImageLoaderTests final : public TRAP::Layer
{
public:
	ImageLoaderTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;
	bool OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& event);
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);

private:
	TRAP::Graphics::OrthographicCamera m_camera{-TRAP::Application::GetWindow()->GetAspectRatio(),
	                                             TRAP::Application::GetWindow()->GetAspectRatio(),
	                                            -1.0f, 1.0f, -1.0f, 1.0f};

	bool m_png = false;
	bool m_tga = true;
	bool m_bmp = false;
	bool m_pm = false;
	bool m_radiance = false;
	bool m_qoi = false;
};

#endif /*GAMESTRAP_IMAGELOADERTESTS_H*/