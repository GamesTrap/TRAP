#include "MonitorTests.h"

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

	TRAP::Application::Shutdown();
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
}