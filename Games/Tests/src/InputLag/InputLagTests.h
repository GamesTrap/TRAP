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
	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);
	bool OnMouseMove(TRAP::Events::MouseMoveEvent& event);

	void SampleInput();
	void DrawMarker(uint32_t lead, TRAP::Math::Vec2 pos);

	CursorMethod m_cursorMethod;
	bool m_vsync;
	TRAP::Math::Vec2 m_cursorNew;
	TRAP::Math::Vec2 m_cursorPos;
	TRAP::Math::Vec2 m_cursorVelocity;
	bool m_showForecasts;

	//NVIDIA-Relfex stuff
	TRAP::Graphics::LatencyMode m_latencyMode;

#ifdef NVIDIA_REFLEX_AVAILABLE
	std::array<float, 50> m_totalHistory{};
	std::array<float, 50> m_simulationDeltaHistory{};
	std::array<float, 50> m_renderDeltaHistory{};
	std::array<float, 50> m_presentDeltaHistory{};
	std::array<float, 50> m_driverDeltaHistory{};
	std::array<float, 50> m_OSRenderQueueDeltaHistory{};
	std::array<float, 50> m_GPURenderDeltaHistory{};
#endif /*NVIDIA_REFLEX_AVAILABLE*/

	TRAP::Utils::Timer m_updateLatencyTimer;
};

#endif /*GAMESTRAP_INPUTLAGTESTS_H*/