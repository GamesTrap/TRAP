#ifndef _GAMESTRAP_IMAGELOADERTESTS_H_
#define _GAMESTRAP_IMAGELOADERTESTS_H_

#include <TRAP.h>

class ImageLoaderTests final : public TRAP::Layer
{
public:
	ImageLoaderTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;
	bool OnFrameBufferResize(TRAP::Events::FrameBufferResizeEvent& event);
	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);

private:
	TRAP::Graphics::OrthographicCamera m_camera;
	
	bool m_png;
	bool m_tga;
	bool m_bmp;
	bool m_pm;
};

#endif /*_GAMESTRAP_IMAGELOADERTESTS_H_*/