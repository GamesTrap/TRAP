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
	struct RateData
	{
		TRAP::Graphics::ShadingRate ShadingRate;
		std::string Name;
		bool Supported;
	};

	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
	bool OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& event);

	TRAP::Ref<TRAP::Graphics::RenderTarget> CreateShadingRateTexture(uint32_t framebufferWidth, uint32_t framebufferHeight);

	TRAP::Graphics::OrthographicCameraController m_cameraController;

	TRAP::Ref<TRAP::Graphics::RenderTarget> m_shadingRateTexture;

    TRAP::Graphics::ShadingRate m_shadingRate;
    std::vector<RateData> m_shadingRates;
    float m_currRenderScale;

	bool m_supportsPerDrawVRS;
	bool m_supportsPerTileVRS;

	bool m_perDrawActive;

	bool m_visualizeShadingRate;
};

#endif /*GAMESTRAP_VRSTESTS_H*/