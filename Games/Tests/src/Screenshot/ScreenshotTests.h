#ifndef GAMESTRAP_SCREENSHOTTESTS_H
#define GAMESTRAP_SCREENSHOTTESTS_H

#include <TRAP.h>

class ScreenshotTests final : public TRAP::Layer
{
public:
	ScreenshotTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;

private:
	TRAP::Graphics::OrthographicCameraController m_cameraController;
};

#endif /*GAMESTRAP_SCREENSHOTTESTS_H*/