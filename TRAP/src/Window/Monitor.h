#ifndef TRAP_MONITOR_H
#define TRAP_MONITOR_H

#ifndef TRAP_HEADLESS_MODE

#include <cstdint>
#include <string>
#include <vector>

#include "Maths/Math.h"
#include "Window/WindowingAPI.h"

namespace TRAP
{
	/// <summary>
	/// Class representing a physical monitor.
	/// </summary>
	class Monitor
	{
	public:
		/// <summary>
		/// Struct representing a video mode used by TRAP::Monitors.
		/// </summary>
		struct VideoMode
		{
			/// <summary>
			/// Constructor.
			/// </summary>
			/// <param name="width">Width.</param>
			/// <param name="height">Height.</param>
			/// <param name="refreshRate">Refresh rate.</param>
			constexpr VideoMode(uint32_t width, uint32_t height, double refreshRate) noexcept;

			uint32_t Width = 0;
			uint32_t Height = 0;
			double RefreshRate = 0;

			[[nodiscard]] constexpr auto operator<=>(const VideoMode& mode) const noexcept = default;
		};


		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~Monitor() = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr Monitor(const Monitor&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Monitor& operator=(const Monitor&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr Monitor(Monitor&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Monitor& operator=(Monitor&&) noexcept = default;

		/// <summary>
		/// Retrieve the name of a monitor.
		/// </summary>
		/// <returns>Name of the monitor.</returns>
		[[nodiscard]] constexpr std::string GetName() const noexcept;
		/// <summary>
		/// Retrieve a list of available video modes for the monitor.
		/// </summary>
		/// <returns>Vector of video modes.</returns>
		[[nodiscard]] std::vector<VideoMode> GetVideoModes() const;
		/// <summary>
		/// Retrieve the native video mode.
		/// </summary>
		/// <returns>Native video mode on success, empty optional otherwise.</returns>
		[[nodiscard]] std::optional<TRAP::Monitor::VideoMode> GetNativeVideoMode() const noexcept;
		/// <summary>
		/// Retrieve the current video mode.
		/// </summary>
		/// <returns>Current video mode on success, empty optional otherwise.</returns>
		[[nodiscard]] std::optional<TRAP::Monitor::VideoMode> GetCurrentVideoMode() const noexcept;
		/// <summary>
		/// Retrieve the monitors content scale.
		/// </summary>
		/// <returns>Vec2 containing the content scale.</returns>
		[[nodiscard]] Math::Vec2 GetContentScale() const;
		/// <summary>
		/// Retrieve the monitors X content scale.
		/// </summary>
		/// <returns>Monitors X content scale.</returns>
		[[nodiscard]] float GetContentScaleX() const;
		/// <summary>
		/// Retrieve the monitors Y content scale.
		/// </summary>
		/// <returns>Monitors Y content scale.</returns>
		[[nodiscard]] float GetContentScaleY() const;
		/// <summary>
		/// Retrieve the monitors viewport position.
		/// </summary>
		/// <returns>Vec2i containing the monitors viewport position.</returns>
		[[nodiscard]] Math::Vec2i GetPosition() const;
		/// <summary>
		/// Retrieve the monitors X viewport position.
		/// </summary>
		/// <returns>Monitors X viewport position.</returns>
		[[nodiscard]] int32_t GetPositionX() const;
		/// <summary>
		/// Retrieve the monitors Y viewport position.
		/// </summary>
		/// <returns>Monitors Y viewport position.</returns>
		[[nodiscard]] int32_t GetPositionY() const;
		/// <summary>
		/// Retrieve the monitors work area.
		///
		/// Work area is the area of a monitor without task bars or menu bars.
		/// </summary>
		/// <returns>Vec4i containing the monitors work area.</returns>
		[[nodiscard]] Math::Vec4i GetWorkArea() const;
		/// <summary>
		/// Retrieve the monitors X work area.
		/// </summary>
		/// <returns>Monitors X work area.</returns>
		[[nodiscard]] int32_t GetWorkAreaX() const;
		/// <summary>
		/// Retrieve the monitors Y work area.
		/// </summary>
		/// <returns>Monitors Y work area.</returns>
		[[nodiscard]] int32_t GetWorkAreaY() const;
		/// <summary>
		/// Retrieve the monitors work area width.
		/// </summary>
		/// <returns>Monitors work area width.</returns>
		[[nodiscard]] int32_t GetWorkAreaWidth() const;
		/// <summary>
		/// Retrieve the monitors work area height.
		/// </summary>
		/// <returns>Monitors work area height.</returns>
		[[nodiscard]] int32_t GetWorkAreaHeight() const;
		/// <summary>
		/// Check whether the monitor is currently used by a window.
		/// </summary>
		/// <returns>True if monitor is currently used, false otherwise.</returns>
		[[nodiscard]] constexpr bool IsInUse() const noexcept;
		/// <summary>
		/// Retrieve the monitors ID.
		///
		/// Note: ID 0 is always the primary monitor!
		/// </summary>
		/// <returns>Monitors ID.</returns>
		[[nodiscard]] uint32_t GetID() const;

		/// <summary>
		/// Retrieve a pointer to the internal monitor.
		/// </summary>
		/// <returns>Pointer to the internal monitor of the Monitor.</returns>
		[[nodiscard]] constexpr void* GetInternalMonitor() const noexcept;

		/// <summary>
		/// Retrieve all currently connected monitors.
		/// </summary>
		/// <returns>Vector with all monitors.</returns>
		[[nodiscard]] static std::vector<Monitor> GetAllMonitors();
		/// <summary>
		/// Retrieve the primary monitor.
		/// </summary>
		/// <returns>Primary monitor.</returns>
		[[nodiscard]] static Monitor GetPrimaryMonitor();

	private:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="monitor">Pointer to the InternalMonitor handle.</param>
		explicit Monitor(INTERNAL::WindowingAPI::InternalMonitor* monitor);

		INTERNAL::WindowingAPI::InternalMonitor* m_handle{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Monitor::VideoMode::VideoMode(const uint32_t width, const uint32_t height, const double refreshRate) noexcept
	: Width(width), Height(height), RefreshRate(refreshRate)
{}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline constexpr std::string TRAP::Monitor::GetName() const noexcept
{
	return m_handle->Name;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline constexpr bool TRAP::Monitor::IsInUse() const noexcept
{
	return m_handle->Window != nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline constexpr void* TRAP::Monitor::GetInternalMonitor() const noexcept
{
	return m_handle;
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_MONITOR_H*/