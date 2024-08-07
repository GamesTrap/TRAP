#include "InputLagTests.h"

InputLagTests::InputLagTests()
	: Layer("InputLag")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("InputLag");

	m_latencyMode = TRAP::Graphics::RenderCommand::GetLatencyMode();
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::OnImGuiRender()
{
	ImGui::Begin("InputLag", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
	                                  ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Press ESC to close");
	ImGui::Text("VSync (V): %s", m_vsync ? "Enabled" : "Disabled");
	ImGui::Separator();
	//Draw indicators on mouse pos
	for(i32 lead = m_showForecasts ? 3 : 0; lead >= 0; --lead)
		DrawMarker(NumericCast<u32>(lead), TRAP::Math::Vec2(m_cursorPos + m_cursorVelocity * NumericCast<f32>(lead)));
	//Draw instructions
	ImGui::Text("Move mouse uniformly and check marker under cursor:");
	for(u32 lead = 0; lead <= 3; ++lead)
	{
		if(lead == 0)
			ImGui::Text("Red circle - Current cursor position (no input lag)");
		else if(lead == 1)
			ImGui::Text("Yellow circle - %d-frame forecast (input lag is %d frame)", lead, lead);
		else if(lead == 2)
			ImGui::Text("Green circle  - %d-frame forecast (input lag is %d frame)", lead, lead);
		else if(lead == 3)
			ImGui::Text("Blue circle   - %d-frame forecast (input lag is %d frame)", lead, lead);
	}
	ImGui::Text("Current input method: %s", m_cursorMethod == CursorMethod::SyncQuery ? "Sync query" : "Latest input message");
	ImGui::Separator();
	ImGui::Checkbox("Show forecasts", &m_showForecasts);
	if(ImGui::RadioButton("TRAP::Input::GetMousePosition (sync query)", m_cursorMethod == CursorMethod::SyncQuery))
		m_cursorMethod = CursorMethod::SyncQuery;
	if(ImGui::RadioButton("TRAP::Events::MouseMoveEvent (latest input message)", m_cursorMethod == CursorMethod::InputMessage))
		m_cursorMethod = CursorMethod::InputMessage;
	ImGui::End();

	ImGui::Begin("Latency Stats", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	             ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Performance:");
    ImGui::Text("CPU: %ix %s", TRAP::Utils::GetCPUInfo().LogicalCores, TRAP::Utils::GetCPUInfo().Model.c_str());
	ImGui::Text("GPU: %s", TRAP::Graphics::RenderCommand::GetGPUName().c_str());
    ImGui::Text("CPU FPS: %u", TRAP::Graphics::RenderCommand::GetCPUFPS());
    ImGui::Text("GPU FPS: %u", TRAP::Graphics::RenderCommand::GetGPUFPS());
    ImGui::Text("CPU FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetCPUFrameTime());
    ImGui::Text("GPU Graphics FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUGraphicsFrameTime());
    ImGui::Text("GPU Compute FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUComputeFrameTime());
    ImGui::Separator();
	ImGui::Text("NVIDIA Reflex:");
	if(!TRAP::Graphics::RendererAPI::GPUSettings.ReflexSupported)
		ImGui::Text("NVIDIA Reflex is not supported on this GPU!");
	else
	{
		ImGui::Text("Current Latency Mode: %s", fmt::format("{}", m_latencyMode).c_str());

		if(ImGui::BeginCombo("Latency Mode", fmt::format("{}", m_latencyMode).c_str()))
		{
			for(u32 i = 0; i <= std::to_underlying(TRAP::Graphics::LatencyMode::EnabledBoost); ++i)
			{
				const bool isSelected = (std::to_underlying(m_latencyMode) == i);
				if(ImGui::Selectable(fmt::format("{}", static_cast<TRAP::Graphics::LatencyMode>(i)).c_str(), isSelected))
				{
					TRAP::Graphics::RenderCommand::SetLatencyMode(static_cast<TRAP::Graphics::LatencyMode>(i));
					m_latencyMode = TRAP::Graphics::RenderCommand::GetLatencyMode();
				}
				if(isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
	}
	ImGui::Separator();
	ImGui::Text("This software contains source code provided by NVIDIA Corporation.");
	ImGui::End();

#ifdef NVIDIA_REFLEX_AVAILABLE
	if(TRAP::Graphics::RendererAPI::GPUSettings.ReflexSupported)
	{
		ImGui::Begin("NVIDIA Reflex Latency", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::PlotLines("Total Game to Render Latency", m_totalHistory.data(), NumericCast<i32>(m_totalHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("Simulation Delta", m_simulationDeltaHistory.data(), NumericCast<i32>(m_simulationDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("Render Delta", m_renderDeltaHistory.data(), NumericCast<i32>(m_renderDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("Present Delta", m_presentDeltaHistory.data(), NumericCast<i32>(m_presentDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("Driver Delta", m_driverDeltaHistory.data(), NumericCast<i32>(m_driverDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("OS Render Queue Delta", m_OSRenderQueueDeltaHistory.data(), NumericCast<i32>(m_OSRenderQueueDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("GPU Render Delta", m_GPURenderDeltaHistory.data(), NumericCast<i32>(m_GPURenderDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::End();
	}
#endif /*NVIDIA_REFLEX_AVAILABLE*/
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	SampleInput();

#ifdef NVIDIA_REFLEX_AVAILABLE
	//Update latency history
	if (TRAP::Graphics::RendererAPI::GPUSettings.ReflexSupported && m_updateLatencyTimer.Elapsed() >= 0.025f)
	{
		const auto latencyData = TRAP::Graphics::RendererAPI::GetRenderer()->ReflexGetLatency();
		const auto& curr = latencyData.frameReport[63];
		const f32 totalGameToRenderLatencyMs = (curr.gpuRenderEndTime - curr.simStartTime) / 1000.0f;
		const f32 simulationDeltaMs = (curr.simEndTime - curr.simStartTime) / 1000.0f;
		const f32 renderDeltaMs = (curr.renderSubmitEndTime - curr.renderSubmitStartTime) / 1000.0f;
		const f32 presentDeltaMs = (curr.presentEndTime - curr.presentStartTime) / 1000.0f;
		const f32 driverDeltaMs = (curr.driverEndTime - curr.driverStartTime) / 1000.0f;
		const f32 OSRenderQueueDeltaMs = (curr.osRenderQueueEndTime - curr.osRenderQueueStartTime) / 1000.0f;
		const f32 GPURenderDeltaMs = (curr.gpuRenderEndTime - curr.gpuRenderStartTime) / 1000.0f;

		constinit static usize frameTimeIndex = 0;
		m_updateLatencyTimer.Reset();
		if (frameTimeIndex < m_totalHistory.size() - 1)
		{
			if(curr.gpuRenderEndTime != 0)
			{
				m_totalHistory[frameTimeIndex] = totalGameToRenderLatencyMs;
				m_simulationDeltaHistory[frameTimeIndex] = simulationDeltaMs;
				m_renderDeltaHistory[frameTimeIndex] = renderDeltaMs;
				m_presentDeltaHistory[frameTimeIndex] = presentDeltaMs;
				m_driverDeltaHistory[frameTimeIndex] = driverDeltaMs;
				m_OSRenderQueueDeltaHistory[frameTimeIndex] = OSRenderQueueDeltaMs;
				m_GPURenderDeltaHistory[frameTimeIndex] = GPURenderDeltaMs;
			}
			frameTimeIndex++;
		}
		else
		{
			if(curr.gpuRenderEndTime != 0)
			{
				std::shift_left(m_totalHistory.begin(), m_totalHistory.end(), 1);
				m_totalHistory.back() = totalGameToRenderLatencyMs;
				std::shift_left(m_simulationDeltaHistory.begin(), m_simulationDeltaHistory.end(), 1);
				m_simulationDeltaHistory.back() = simulationDeltaMs;
				std::shift_left(m_renderDeltaHistory.begin(), m_renderDeltaHistory.end(), 1);
				m_renderDeltaHistory.back() = renderDeltaMs;
				std::shift_left(m_presentDeltaHistory.begin(), m_presentDeltaHistory.end(), 1);
				m_presentDeltaHistory.back() = presentDeltaMs;
				std::shift_left(m_driverDeltaHistory.begin(), m_driverDeltaHistory.end(), 1);
				m_driverDeltaHistory.back() = driverDeltaMs;
				std::shift_left(m_OSRenderQueueDeltaHistory.begin(), m_OSRenderQueueDeltaHistory.end(), 1);
				m_OSRenderQueueDeltaHistory.back() = OSRenderQueueDeltaMs;
				std::shift_left(m_GPURenderDeltaHistory.begin(), m_GPURenderDeltaHistory.end(), 1);
				m_GPURenderDeltaHistory.back() = GPURenderDeltaMs;
			}
		}
	}
#endif /*NVIDIA_REFLEX_AVAILABLE*/
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&InputLagTests::OnKeyPress, this));
	dispatcher.Dispatch<TRAP::Events::MouseMoveEvent>(std::bind_front(&InputLagTests::OnMouseMove, this));
}

//-------------------------------------------------------------------------------------------------------------------//

bool InputLagTests::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
{
	switch(event.GetKey())
	{
	case TRAP::Input::Key::Escape:
		TRAP::Application::Shutdown();
		break;

	case TRAP::Input::Key::V:
	{
		m_vsync = !m_vsync;
		TP_TRACE("[InputLag] VSync: ", m_vsync ? "On" : "Off");
		event.GetWindow().SetVSync(m_vsync);
		break;
	}

	default:
		break;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool InputLagTests::OnMouseMove(const TRAP::Events::MouseMoveEvent& event)
{
	if(m_cursorMethod == CursorMethod::InputMessage)
		m_cursorNew = event.GetPosition();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::SampleInput()
{
	static constexpr f32 a = 0.25f; //Exponential smoothing factor

	if(m_cursorMethod == CursorMethod::SyncQuery)
		m_cursorNew = TRAP::Input::GetMousePosition();

	m_cursorVelocity = (m_cursorNew - m_cursorPos) * a + m_cursorVelocity * (1 - a);
	m_cursorPos = m_cursorNew;
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::DrawMarker(const u32 lead, TRAP::Math::Vec2 pos)
{
	static constexpr std::array<ImColor, 4> colors{ImColor(1.0f, 0.0f, 0.0f), ImColor(1.0f, 1.0f, 0.0f), ImColor(0.0f, 1.0f, 0.0), ImColor(0.0f, 0.376f, 1.0f)};

	auto* const drawList = ImGui::GetBackgroundDrawList();
	drawList->AddCircleFilled(ImVec2(pos.x(), pos.y()), 10, colors[lead], 0);
}