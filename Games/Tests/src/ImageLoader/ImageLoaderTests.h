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
	constexpr void OnEvent(TRAP::Events::Event& event) override;
	constexpr bool OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& event);
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

//-------------------------------------------------------------------------------------------------------------------//

constexpr void ImageLoaderTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>(std::bind_front(&ImageLoaderTests::OnFrameBufferResize, this));
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&ImageLoaderTests::OnKeyPress, this));
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool ImageLoaderTests::OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& event)
{
	m_camera.SetProjection(-event.GetAspectRatio(), event.GetAspectRatio(), -1.0f, 1.0f, -1.0f, 1.0f);

	return false;
}

#endif /*GAMESTRAP_IMAGELOADERTESTS_H*/
