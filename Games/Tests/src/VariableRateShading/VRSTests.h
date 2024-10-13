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

	TRAP::Ref<TRAP::Graphics::RenderTarget> CreateShadingRateTexture(u32 framebufferWidth, u32 framebufferHeight);

	TRAP::Graphics::OrthographicCameraController m_cameraController{TRAP::Application::GetWindow()->GetAspectRatio(), true};

	TRAP::Ref<TRAP::Graphics::RenderTarget> m_shadingRateTexture = nullptr;

    TRAP::Graphics::ShadingRate m_shadingRate = TRAP::Graphics::ShadingRate::NotSupported;
    f32 m_currRenderScale = 1.0f;

	bool m_supportsPerDrawVRS = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps & TRAP::Graphics::RendererAPI::ShadingRateCaps::PerDraw) != TRAP::Graphics::RendererAPI::ShadingRateCaps::NotSupported;
	bool m_supportsPerTileVRS = (TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps & TRAP::Graphics::RendererAPI::ShadingRateCaps::PerTile) != TRAP::Graphics::RendererAPI::ShadingRateCaps::NotSupported;

	bool m_perDrawActive = true;

	bool m_visualizeShadingRate = false;
};

#endif /*GAMESTRAP_VRSTESTS_H*/
