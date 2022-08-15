#ifndef GAMESTRAP_VRSTESTS_H
#define GAMESTRAP_VRSTESTS_H

#include "Graphics/RenderCommand.h"
#include <TRAP.h>

class VRSTests final : public TRAP::Layer
{
public:
	VRSTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);

	TRAP::Graphics::OrthographicCameraController m_cameraController;

    TRAP::Graphics::ShadingRate m_shadingRate;
    std::vector<TRAP::Graphics::ShadingRate> m_supportedShadingRates;
    std::vector<const char*> m_supportedShadingRatesStrings;
};

#endif /*GAMESTRAP_VRSTESTS_H*/