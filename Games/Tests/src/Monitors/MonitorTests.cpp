#include "MonitorTests.h"

std::vector<MonitorTests::MonitorInfo> MonitorTests::s_monitorInfos{};

//-------------------------------------------------------------------------------------------------------------------//

MonitorTests::MonitorTests()
	: Layer("Monitor")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void MonitorTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Monitor");

	std::vector<TRAP::Monitor> monitors = TRAP::Monitor::GetAllMonitors();

	for (const TRAP::Monitor& monitor : monitors)
		ListModes(monitor);
}

//-------------------------------------------------------------------------------------------------------------------//

void MonitorTests::OnImGuiRender()
{
	for(const MonitorInfo& monitor : s_monitorInfos)
	{
		ImGui::Begin(monitor.Name.c_str(), nullptr);
		ImGui::Text("Name: %s", monitor.Name.c_str());
		ImGui::Text("Primary: %s", (monitor.ID == TRAP::Monitor::GetPrimaryMonitor().GetID() ? std::string("True").c_str() : std::string("False").c_str()));
		ImGui::Text("Current VideoMode: %s", FormatMode(monitor.CurrentVideoMode).c_str());
		ImGui::Text("Virtual Position: %i %i", monitor.Position.x, monitor.Position.y);
		ImGui::Text("Content Scale: %f %f", monitor.Scale.x, monitor.Scale.y);
		ImGui::Text("Monitor Work Area: %i %i Starting @ %i %i", monitor.WorkArea.x, monitor.WorkArea.y, monitor.WorkArea.z, monitor.WorkArea.w);
		ImGui::Separator();
		ImGui::Text("Supported VideoModes:");
		for(const TRAP::Monitor::VideoMode& videoMode : monitor.VideoModes)
		{
			if (monitor.CurrentVideoMode.Width == videoMode.Width && monitor.CurrentVideoMode.Height == videoMode.Height && monitor.CurrentVideoMode.RefreshRate == videoMode.RefreshRate)
				ImGui::Text("%s (Current)", FormatMode(videoMode).c_str());
			else
				ImGui::Text("%s", FormatMode(videoMode).c_str());
		}
		ImGui::End();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void MonitorTests::OnUpdate(const TRAP::Utils::TimeStep&)
{
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::ClearBuffer::Color_Depth);
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t MonitorTests::Euclid(const int32_t a, const int32_t b)
{
	return b ? Euclid(b, a % b) : a;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string MonitorTests::FormatMode(const TRAP::Monitor::VideoMode& mode)
{
	const int32_t gcd = Euclid(mode.Width, mode.Height);

	std::string buffer = std::to_string(mode.Width) + "x" + std::to_string(mode.Height) + "(" +
		                 std::to_string(mode.Width / gcd) + ":" + std::to_string(mode.Height / gcd) + ") " + 
		                 std::to_string(mode.RefreshRate) + "Hz";

	return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

void MonitorTests::ListModes(const TRAP::Monitor& monitor)
{
	const TRAP::Monitor::VideoMode& mode = monitor.GetCurrentVideoMode();
	const std::vector<TRAP::Monitor::VideoMode>& modes = monitor.GetVideoModes();

	const TRAP::Math::Vec2i position = monitor.GetPosition();
	const TRAP::Math::Vec2 scale = monitor.GetContentScale();
	const TRAP::Math::Vec4i workArea = monitor.GetWorkArea();
	
	TP_TRACE("Name: ", monitor.GetName(), "(", (TRAP::Monitor::GetPrimaryMonitor().GetID() == monitor.GetID() ? "primary" : "secondary"), ")");
	TP_TRACE("Current mode: ", FormatMode(mode));
	TP_TRACE("Virtual position: ", position.x, ", ", position.y);
	TP_TRACE("Content scale: ", scale.x, "x", scale.y);
	TP_TRACE("Monitor work area: ", workArea.x, "x", workArea.y, " starting at ", workArea.z, ", ", workArea.w);

	TP_TRACE("Modes:");

	for(uint32_t i = 0; i < modes.size(); i++)
	{
		if (mode.Width == modes[i].Width && mode.Height == modes[i].Height && mode.RefreshRate == modes[i].RefreshRate)
			TP_TRACE(i, ": ", FormatMode(modes[i]), " (current mode)");
		else
			TP_TRACE(i, ": ", FormatMode(modes[i]));
	}

	MonitorInfo monitorInfo
	{
		monitor.GetName(),
		monitor.GetID(),
		mode,
		modes,
		position,
		scale,
		workArea
	};
	s_monitorInfos.emplace_back(monitorInfo);
}