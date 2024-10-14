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
	constexpr void OnEvent(TRAP::Events::Event& event) override;

	enum class CursorMethod : u8
	{
		SyncQuery,
		InputMessage
	};

	//NVIDIA-Relfex stuff
	struct ReflexData
	{
	#ifdef NVIDIA_REFLEX_AVAILABLE
		std::array<f32, 50u> m_totalHistory{};
		std::array<f32, 50u> m_simulationDeltaHistory{};
		std::array<f32, 50u> m_renderDeltaHistory{};
		std::array<f32, 50u> m_presentDeltaHistory{};
		std::array<f32, 50u> m_driverDeltaHistory{};
		std::array<f32, 50u> m_OSRenderQueueDeltaHistory{};
		std::array<f32, 50u> m_GPURenderDeltaHistory{};
	#endif /*NVIDIA_REFLEX_AVAILABLE*/
	};

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
	constexpr bool OnMouseMove(const TRAP::Events::MouseMoveEvent& event);

	void SampleInput();
	void DrawMarker(u32 lead, TRAP::Math::Vec2 pos);

	CursorMethod m_cursorMethod = CursorMethod::SyncQuery;
	bool m_vsync = TRAP::Application::GetWindow()->GetVSync();
	TRAP::Math::Vec2 m_cursorNew{};
	TRAP::Math::Vec2 m_cursorPos{};
	TRAP::Math::Vec2 m_cursorVelocity{};
	bool m_showForecasts = true;

	ReflexData m_reflexData{};

	TRAP::Utils::Timer m_updateLatencyTimer{};
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr void InputLagTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&InputLagTests::OnKeyPress, this));
	dispatcher.Dispatch<TRAP::Events::MouseMoveEvent>(std::bind_front(&InputLagTests::OnMouseMove, this));
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool InputLagTests::OnMouseMove(const TRAP::Events::MouseMoveEvent& event)
{
	if(m_cursorMethod == CursorMethod::InputMessage)
		m_cursorNew = event.GetPosition();

	return true;
}

#endif /*GAMESTRAP_INPUTLAGTESTS_H*/
