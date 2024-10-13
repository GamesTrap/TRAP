#include "MonitorTests.h"

namespace
{
	[[nodiscard]] constexpr u32 Euclid(const u32 a, const u32 b)
	{
		return b != 0u ? Euclid(b, a % b) : a;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] std::string FormatMode(const TRAP::Monitor::VideoMode& mode)
	{
		const u32 gcd = Euclid(mode.Width, mode.Height);

		return fmt::format("{}x{}({}:{}) {}Hz", mode.Width, mode.Height, (mode.Width / gcd), (mode.Height / gcd),
						   mode.RefreshRate);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void MonitorTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Monitor");
}

//-------------------------------------------------------------------------------------------------------------------//

void MonitorTests::OnImGuiRender()
{
	const u32 primaryID = TRAP::Monitor::GetPrimaryMonitor().GetID();

	for(const TRAP::Monitor& monitor : TRAP::Monitor::GetAllMonitors())
	{
		const std::string name = monitor.GetName();
		const std::string uniqueName = fmt::format("{} ({})", name, monitor.GetID());
		const u32 ID = monitor.GetID();
		const auto nativeMode = monitor.GetNativeVideoMode();
		const auto currentMode = monitor.GetCurrentVideoMode();
		const auto position = monitor.GetPosition();
		const auto scale = monitor.GetContentScale();
		const auto workArea = monitor.GetWorkArea();
		const auto videoModes = monitor.GetVideoModes();

		ImGui::Begin(uniqueName.c_str());
		ImGui::Text("Name: %s", name.c_str());
		ImGui::Text("Primary: %s", (ID == primaryID ? "True" : "False"));
		ImGui::Text("Native VideoMode: %s", nativeMode ? FormatMode(*nativeMode).c_str() : "Unknown");
		ImGui::Text("Current VideoMode: %s", currentMode ? FormatMode(*currentMode).c_str() : "Unknown");
		ImGui::Text("Virtual Position: %i %i", position.x(), position.y());
		ImGui::Text("Content Scale: %f %f", scale.x(), scale.y());
		ImGui::Text("Monitor Work Area: %i %i Starting @ %i %i", workArea.x(), workArea.y(),
		            workArea.z(), workArea.w());
		ImGui::Separator();
		ImGui::Text("Supported VideoModes:");
		for(const TRAP::Monitor::VideoMode& videoMode : videoModes)
		{
			std::string tmp = FormatMode(videoMode);

			if (currentMode && *currentMode == videoMode)
				tmp += " (current)";
			if (nativeMode && *nativeMode == videoMode)
				tmp += " (native)";

			ImGui::Text("%s", tmp.c_str());
		}
		ImGui::End();
	}
}
