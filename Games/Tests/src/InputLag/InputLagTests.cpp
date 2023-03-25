#include "InputLagTests.h"

InputLagTests::InputLagTests()
	: Layer("InputLag"), m_cursorMethod(CursorMethod::SyncQuery),
	  m_vsync(TRAP::Application::GetConfig().Get<bool>("VSync")),
	  m_cursorNew(), m_cursorPos(), m_cursorVelocity(), m_showForecasts(true), m_latencyMode()
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
	for(int32_t lead = m_showForecasts ? 3 : 0; lead >= 0; --lead)
		DrawMarker(lead, TRAP::Math::Vec2(m_cursorPos + m_cursorVelocity * static_cast<float>(lead)));
	//Draw instructions
	ImGui::Text("Move mouse uniformly and check marker under cursor:");
	for(uint32_t lead = 0; lead <= 3; ++lead)
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
		ImGui::Text("Current Latency Mode: %s", TRAP::Utils::String::ConvertToString(m_latencyMode).c_str());

		static constexpr std::array<const char*, 3> latencyModes{"Disabled", "Enabled", "Enabled+Boost"};
		static int32_t currentLatencyMode = static_cast<int32_t>(m_latencyMode);
		if(ImGui::Combo("Latency Mode", &currentLatencyMode, latencyModes.data(), latencyModes.size()))
		{
			if(static_cast<int32_t>(m_latencyMode) != currentLatencyMode)
			{
				m_latencyMode = static_cast<TRAP::Graphics::LatencyMode>(currentLatencyMode);
				TRAP::Graphics::RenderCommand::SetLatencyMode(m_latencyMode);
				m_latencyMode = TRAP::Graphics::RenderCommand::GetLatencyMode();
			}
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
		ImGui::PlotLines("Total Game to Render Latency", m_totalHistory.data(), static_cast<int>(m_totalHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("Simulation Delta", m_simulationDeltaHistory.data(), static_cast<int>(m_simulationDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("Render Delta", m_renderDeltaHistory.data(), static_cast<int>(m_renderDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("Present Delta", m_presentDeltaHistory.data(), static_cast<int>(m_presentDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("Driver Delta", m_driverDeltaHistory.data(), static_cast<int>(m_driverDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("OS Render Queue Delta", m_OSRenderQueueDeltaHistory.data(), static_cast<int>(m_OSRenderQueueDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("GPU Render Delta", m_GPURenderDeltaHistory.data(), static_cast<int>(m_GPURenderDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::End();
	}
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	SampleInput();

#ifdef NVIDIA_REFLEX_AVAILABLE
	//Update latency history
	if (m_updateLatencyTimer.Elapsed() >= 0.025f)
	{
		const auto latencyData = TRAP::Graphics::RendererAPI::GetRenderer()->ReflexGetLatency();
		const auto& curr = latencyData.frameReport[63];
		const float totalGameToRenderLatencyMs = (curr.gpuRenderEndTime - curr.simStartTime) / 1000.0f;
		const float simulationDeltaMs = (curr.simEndTime - curr.simStartTime) / 1000.0f;
		const float renderDeltaMs = (curr.renderSubmitEndTime - curr.renderSubmitStartTime) / 1000.0f;
		const float presentDeltaMs = (curr.presentEndTime - curr.presentStartTime) / 1000.0f;
		const float driverDeltaMs = (curr.driverEndTime - curr.driverStartTime) / 1000.0f;
		const float OSRenderQueueDeltaMs = (curr.osRenderQueueEndTime - curr.osRenderQueueStartTime) / 1000.0f;
		const float GPURenderDeltaMs = (curr.gpuRenderEndTime - curr.gpuRenderStartTime) / 1000.0f;

		static int frameTimeIndex = 0;
		m_updateLatencyTimer.Reset();
		if (frameTimeIndex < static_cast<int32_t>(m_totalHistory.size() - 1))
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
				std::move(m_totalHistory.begin() + 1, m_totalHistory.end(), m_totalHistory.begin());
				m_totalHistory[m_totalHistory.size() - 1] = totalGameToRenderLatencyMs;
				std::move(m_simulationDeltaHistory.begin() + 1, m_simulationDeltaHistory.end(), m_simulationDeltaHistory.begin());
				m_simulationDeltaHistory[m_simulationDeltaHistory.size() - 1] = simulationDeltaMs;
				std::move(m_renderDeltaHistory.begin() + 1, m_renderDeltaHistory.end(), m_renderDeltaHistory.begin());
				m_renderDeltaHistory[m_renderDeltaHistory.size() - 1] = renderDeltaMs;
				std::move(m_presentDeltaHistory.begin() + 1, m_presentDeltaHistory.end(), m_presentDeltaHistory.begin());
				m_presentDeltaHistory[m_presentDeltaHistory.size() - 1] = presentDeltaMs;
				std::move(m_driverDeltaHistory.begin() + 1, m_driverDeltaHistory.end(), m_driverDeltaHistory.begin());
				m_driverDeltaHistory[m_driverDeltaHistory.size() - 1] = driverDeltaMs;
				std::move(m_OSRenderQueueDeltaHistory.begin() + 1, m_OSRenderQueueDeltaHistory.end(), m_OSRenderQueueDeltaHistory.begin());
				m_OSRenderQueueDeltaHistory[m_OSRenderQueueDeltaHistory.size() - 1] = OSRenderQueueDeltaMs;
				std::move(m_GPURenderDeltaHistory.begin() + 1, m_GPURenderDeltaHistory.end(), m_GPURenderDeltaHistory.begin());
				m_GPURenderDeltaHistory[m_GPURenderDeltaHistory.size() - 1] = GPURenderDeltaMs;
			}
		}
	}
#endif /*NVIDIA_REFLEX_AVAILABLE*/
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e)
	{
		return OnKeyPress(e);
	});
	dispatcher.Dispatch<TRAP::Events::MouseMoveEvent>([this](TRAP::Events::MouseMoveEvent& e)
	{
		return OnMouseMove(e);
	});
}

//-------------------------------------------------------------------------------------------------------------------//

bool InputLagTests::OnKeyPress(TRAP::Events::KeyPressEvent& event)
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
		event.GetWindow()->SetVSync(m_vsync);
		break;
	}

	default:
		break;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool InputLagTests::OnMouseMove(TRAP::Events::MouseMoveEvent& event)
{
	if(m_cursorMethod == CursorMethod::InputMessage)
	{
		m_cursorNew = event.GetPosition();
		m_cursorNew += event.GetWindow()->GetPosition();
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::SampleInput()
{
	static constexpr float a = 0.25f; //Exponential smoothing factor

	if(m_cursorMethod == CursorMethod::SyncQuery)
	{
		m_cursorNew = TRAP::Input::GetMousePosition();
		m_cursorNew += TRAP::Application::GetWindow()->GetPosition();
	}


	m_cursorVelocity = (m_cursorNew - m_cursorPos) * a + m_cursorVelocity * (1 - a);
	m_cursorPos = m_cursorNew;
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::DrawMarker(const int32_t lead, TRAP::Math::Vec2 pos)
{
	static const std::array<ImColor, 4> colors{ImColor(255, 0, 0), ImColor(255, 255, 0), ImColor(0, 255, 0), ImColor(0, 96, 255)};

	auto* const drawList = ImGui::GetBackgroundDrawList();
	drawList->AddCircleFilled(ImVec2(pos.x, pos.y), 10, colors[lead], 0);
}