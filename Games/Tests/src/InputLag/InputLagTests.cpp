#include "InputLagTests.h"

namespace
{
	void DrawLatencyModeSelection()
	{
		ImGui::Text("NVIDIA Reflex:");
		if(!TRAP::Graphics::RendererAPI::GPUSettings.ReflexSupported)
			ImGui::Text("NVIDIA Reflex is not supported on this GPU!");
		else
		{
			static constexpr std::array<TRAP::Graphics::NVIDIAReflexLatencyMode, 3u> latencyModes
			{
				TRAP::Graphics::NVIDIAReflexLatencyMode::Disabled,
				TRAP::Graphics::NVIDIAReflexLatencyMode::Enabled,
				TRAP::Graphics::NVIDIAReflexLatencyMode::EnabledBoost
			};
			const TRAP::Graphics::NVIDIAReflexLatencyMode currentLatencyMode = TRAP::Graphics::RenderCommand::GetReflexLatencyMode();

			ImGui::Text("Current Latency Mode: %s", fmt::format("{}", currentLatencyMode).c_str());

			if(ImGui::BeginCombo("Latency Mode", fmt::format("{}", currentLatencyMode).c_str()))
			{
				for(const auto latencyMode : latencyModes)
				{
					const bool isSelected = latencyMode == currentLatencyMode;
					if(ImGui::Selectable(fmt::format("{}", latencyMode).c_str(), isSelected))
						TRAP::Graphics::RenderCommand::SetReflexLatencyMode(latencyMode);

					if(isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		}
	}

	void DrawNVIDIAReflexStats([[maybe_unused]] const InputLagTests::ReflexData& reflexData)
	{
	if(TRAP::Graphics::RendererAPI::GPUSettings.ReflexSupported)
	{
		ImGui::Begin("NVIDIA Reflex Latency", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
					                                   ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::PlotLines("Total Game to Render Latency", reflexData.m_totalHistory.data(), NumericCast<i32>(reflexData.m_totalHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("Simulation Delta", reflexData.m_simulationDeltaHistory.data(), NumericCast<i32>(reflexData.m_simulationDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("Render Delta", reflexData.m_renderDeltaHistory.data(), NumericCast<i32>(reflexData.m_renderDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("Present Delta", reflexData.m_presentDeltaHistory.data(), NumericCast<i32>(reflexData.m_presentDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("Driver Delta", reflexData.m_driverDeltaHistory.data(), NumericCast<i32>(reflexData.m_driverDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("OS Render Queue Delta", reflexData.m_OSRenderQueueDeltaHistory.data(), NumericCast<i32>(reflexData.m_OSRenderQueueDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::PlotLines("GPU Render Delta", reflexData.m_GPURenderDeltaHistory.data(), NumericCast<i32>(reflexData.m_GPURenderDeltaHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::End();
	}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

InputLagTests::InputLagTests()
	: Layer("InputLag")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("InputLag");
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
	for(u32 lead = 0u; lead <= 3u; ++lead)
	{
		if(lead == 0u)
			ImGui::Text("Red circle - Current cursor position (no input lag)");
		else if(lead == 1u)
			ImGui::Text("Yellow circle - %d-frame forecast (input lag is %d frame)", lead, lead);
		else if(lead == 2u)
			ImGui::Text("Green circle  - %d-frame forecast (input lag is %d frame)", lead, lead);
		else if(lead == 3u)
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
	DrawLatencyModeSelection();
	ImGui::End();

	DrawNVIDIAReflexStats(m_reflexData);
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	SampleInput();

	//Update latency history
	if (TRAP::Graphics::RendererAPI::GPUSettings.ReflexSupported && m_updateLatencyTimer.Elapsed() >= 0.025f)
	{
		const auto latencyData = TRAP::Graphics::RendererAPI::GetRenderer()->ReflexGetLatency(1, *TRAP::Application::GetWindow());
		if(latencyData.empty())
			return;

		const auto& curr = latencyData.back();
		const f32 totalGameToRenderLatencyMs = NumericCast<f32>(curr.gpuRenderEndTimeUs - curr.simStartTimeUs) / 1000.0f;
		const f32 simulationDeltaMs = NumericCast<f32>(curr.simEndTimeUs - curr.simStartTimeUs) / 1000.0f;
		const f32 renderDeltaMs = NumericCast<f32>(curr.renderSubmitEndTimeUs - curr.renderSubmitStartTimeUs) / 1000.0f;
		const f32 presentDeltaMs = NumericCast<f32>(curr.presentEndTimeUs - curr.presentStartTimeUs) / 1000.0f;
		const f32 driverDeltaMs = NumericCast<f32>(curr.driverEndTimeUs - curr.driverStartTimeUs) / 1000.0f;
		const f32 OSRenderQueueDeltaMs = NumericCast<f32>(curr.osRenderQueueEndTimeUs - curr.osRenderQueueStartTimeUs) / 1000.0f;
		const f32 GPURenderDeltaMs = NumericCast<f32>(curr.gpuRenderEndTimeUs - curr.gpuRenderStartTimeUs) / 1000.0f;

		constinit static usize frameTimeIndex = 0u;
		m_updateLatencyTimer.Reset();
		if (frameTimeIndex < m_reflexData.m_totalHistory.size() - 1u)
		{
			if(curr.gpuRenderEndTimeUs != 0)
			{
				m_reflexData.m_totalHistory[frameTimeIndex] = totalGameToRenderLatencyMs;
				m_reflexData.m_simulationDeltaHistory[frameTimeIndex] = simulationDeltaMs;
				m_reflexData.m_renderDeltaHistory[frameTimeIndex] = renderDeltaMs;
				m_reflexData.m_presentDeltaHistory[frameTimeIndex] = presentDeltaMs;
				m_reflexData.m_driverDeltaHistory[frameTimeIndex] = driverDeltaMs;
				m_reflexData.m_OSRenderQueueDeltaHistory[frameTimeIndex] = OSRenderQueueDeltaMs;
				m_reflexData.m_GPURenderDeltaHistory[frameTimeIndex] = GPURenderDeltaMs;
			}
			++frameTimeIndex;
		}
		else
		{
			if(curr.gpuRenderEndTimeUs != 0)
			{
				std::shift_left(m_reflexData.m_totalHistory.begin(), m_reflexData.m_totalHistory.end(), 1);
				m_reflexData.m_totalHistory.back() = totalGameToRenderLatencyMs;
				std::shift_left(m_reflexData.m_simulationDeltaHistory.begin(), m_reflexData.m_simulationDeltaHistory.end(), 1);
				m_reflexData.m_simulationDeltaHistory.back() = simulationDeltaMs;
				std::shift_left(m_reflexData.m_renderDeltaHistory.begin(), m_reflexData.m_renderDeltaHistory.end(), 1);
				m_reflexData.m_renderDeltaHistory.back() = renderDeltaMs;
				std::shift_left(m_reflexData.m_presentDeltaHistory.begin(), m_reflexData.m_presentDeltaHistory.end(), 1);
				m_reflexData.m_presentDeltaHistory.back() = presentDeltaMs;
				std::shift_left(m_reflexData.m_driverDeltaHistory.begin(), m_reflexData.m_driverDeltaHistory.end(), 1);
				m_reflexData.m_driverDeltaHistory.back() = driverDeltaMs;
				std::shift_left(m_reflexData.m_OSRenderQueueDeltaHistory.begin(), m_reflexData.m_OSRenderQueueDeltaHistory.end(), 1);
				m_reflexData.m_OSRenderQueueDeltaHistory.back() = OSRenderQueueDeltaMs;
				std::shift_left(m_reflexData.m_GPURenderDeltaHistory.begin(), m_reflexData.m_GPURenderDeltaHistory.end(), 1);
				m_reflexData.m_GPURenderDeltaHistory.back() = GPURenderDeltaMs;
			}
		}
	}
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

void InputLagTests::SampleInput()
{
	static constexpr f32 a = 0.25f; //Exponential smoothing factor

	if(m_cursorMethod == CursorMethod::SyncQuery)
		m_cursorNew = TRAP::Input::GetMousePosition();

	m_cursorVelocity = (m_cursorNew - m_cursorPos) * a + m_cursorVelocity * (1.0f - a);
	m_cursorPos = m_cursorNew;
}

//-------------------------------------------------------------------------------------------------------------------//

void InputLagTests::DrawMarker(const u32 lead, TRAP::Math::Vec2 pos)
{
	static constexpr std::array<ImColor, 4u> colors{ImColor(1.0f, 0.0f, 0.0f), ImColor(1.0f, 1.0f, 0.0f), ImColor(0.0f, 1.0f, 0.0), ImColor(0.0f, 0.376f, 1.0f)};

	auto* const drawList = ImGui::GetBackgroundDrawList();
	drawList->AddCircleFilled(ImVec2(pos.x(), pos.y()), 10.0f, colors[lead], 0);
}
