#ifndef GAMESTRAP_INPUTLAGTESTS_H
#define GAMESTRAP_INPUTLAGTESTS_H

#include <TRAP.h>

class InputLagTests final : public TRAP::Layer
{
public:
	InputLagTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

	enum class CursorMethod
	{
		SyncQuery,
		InputMessage
	};

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
	bool OnMouseMove(const TRAP::Events::MouseMoveEvent& event);

	void SampleInput();
	void DrawMarker(u32 lead, TRAP::Math::Vec2 pos);

	CursorMethod m_cursorMethod = CursorMethod::SyncQuery;
	bool m_vsync = TRAP::Application::GetWindow()->GetVSync();
	TRAP::Math::Vec2 m_cursorNew{};
	TRAP::Math::Vec2 m_cursorPos{};
	TRAP::Math::Vec2 m_cursorVelocity{};
	bool m_showForecasts = true;

	//NVIDIA-Relfex stuff
	TRAP::Graphics::LatencyMode m_latencyMode = TRAP::Graphics::LatencyMode::Disabled;

#ifdef NVIDIA_REFLEX_AVAILABLE
	std::array<f32, 50> m_totalHistory{};
	std::array<f32, 50> m_simulationDeltaHistory{};
	std::array<f32, 50> m_renderDeltaHistory{};
	std::array<f32, 50> m_presentDeltaHistory{};
	std::array<f32, 50> m_driverDeltaHistory{};
	std::array<f32, 50> m_OSRenderQueueDeltaHistory{};
	std::array<f32, 50> m_GPURenderDeltaHistory{};
#endif /*NVIDIA_REFLEX_AVAILABLE*/

	TRAP::Utils::Timer m_updateLatencyTimer{};
};

#endif /*GAMESTRAP_INPUTLAGTESTS_H*/