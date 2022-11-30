#ifndef GAMESTRAP_VRSTESTS_H
#define GAMESTRAP_VRSTESTS_H

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
	bool OnFrameBufferResize(TRAP::Events::FrameBufferResizeEvent& event);

	TRAP::Ref<TRAP::Graphics::Texture> CreateShadingRateTexture(uint32_t framebufferWidth, uint32_t framebufferHeight);

	TRAP::Graphics::OrthographicCameraController m_cameraController;

	TRAP::Ref<TRAP::Graphics::Texture> m_shadingRateTexture;

    TRAP::Graphics::ShadingRate m_shadingRate;
    std::vector<TRAP::Graphics::ShadingRate> m_supportedShadingRates;
    std::vector<const char*> m_supportedShadingRatesStrings;

	bool m_supportsPerDrawVRS;
	bool m_supportsPerTileVRS;

	bool m_perDrawActive;
};

#endif /*GAMESTRAP_VRSTESTS_H*/