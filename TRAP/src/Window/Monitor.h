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
	/// @brief Class representing a physical monitor.
	/// @remark @headless This class is not available in headless mode.
	class Monitor
	{
	public:
		/// @brief Struct representing a video mode used by TRAP::Monitors.
		/// @remark @headless This struct is not available in headless mode.
		struct VideoMode
		{
			/// @brief Constructor.
			/// @param width Width.
			/// @param height Height.
			/// @param refreshRate Refresh rate.
			constexpr VideoMode(u32 width, u32 height, f64 refreshRate) noexcept;

			u32 Width = 0u;
			u32 Height = 0u;
			f64 RefreshRate = 0.0;

			[[nodiscard]] constexpr auto operator<=>(const VideoMode& mode) const noexcept = default;
		};


		/// @brief Destructor.
		constexpr ~Monitor() = default;
		/// @brief Copy constructor.
		constexpr Monitor(const Monitor&) noexcept = default;
		/// @brief Copy assignment operator.
		constexpr Monitor& operator=(const Monitor&) noexcept = default;
		/// @brief Move constructor.
		constexpr Monitor(Monitor&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr Monitor& operator=(Monitor&&) noexcept = default;

		/// @brief Retrieve the name of a monitor.
		/// @return Name of the monitor.
		[[nodiscard]] constexpr std::string GetName() const noexcept;
		/// @brief Retrieve a list of available video modes for the monitor.
		/// @return Vector of video modes.
		[[nodiscard]] std::vector<VideoMode> GetVideoModes() const;
		/// @brief Retrieve the native video mode.
		/// @return Native video mode on success, empty optional otherwise.
		[[nodiscard]] std::optional<TRAP::Monitor::VideoMode> GetNativeVideoMode() const noexcept;
		/// @brief Retrieve the current video mode.
		/// @return Current video mode on success, empty optional otherwise.
		[[nodiscard]] std::optional<TRAP::Monitor::VideoMode> GetCurrentVideoMode() const noexcept;
		/// @brief Retrieve the monitors content scale.
		/// @return Vec2 containing the content scale.
		[[nodiscard]] Math::Vec2 GetContentScale() const;
		/// @brief Retrieve the monitors X content scale.
		/// @return Monitors X content scale.
		[[nodiscard]] f32 GetContentScaleX() const;
		/// @brief Retrieve the monitors Y content scale.
		/// @return Monitors Y content scale.
		[[nodiscard]] f32 GetContentScaleY() const;
		/// @brief Retrieve the monitors viewport position.
		/// @return Vec2i containing the monitors viewport position.
		[[nodiscard]] Math::Vec2i GetPosition() const;
		/// @brief Retrieve the monitors X viewport position.
		/// @return Monitors X viewport position.
		[[nodiscard]] i32 GetPositionX() const;
		/// @brief Retrieve the monitors Y viewport position.
		/// @return Monitors Y viewport position.
		[[nodiscard]] i32 GetPositionY() const;
		/// @brief Retrieve the monitors work area.
		///
		/// Work area is the area of a monitor without task bars or menu bars.
		/// @return Vec4i containing the monitors work area.
		[[nodiscard]] Math::Vec4i GetWorkArea() const;
		/// @brief Retrieve the monitors X work area.
		/// @return Monitors X work area.
		[[nodiscard]] i32 GetWorkAreaX() const;
		/// @brief Retrieve the monitors Y work area.
		/// @return Monitors Y work area.
		[[nodiscard]] i32 GetWorkAreaY() const;
		/// @brief Retrieve the monitors work area width.
		/// @return Monitors work area width.
		[[nodiscard]] i32 GetWorkAreaWidth() const;
		/// @brief Retrieve the monitors work area height.
		/// @return Monitors work area height.
		[[nodiscard]] i32 GetWorkAreaHeight() const;
		/// @brief Check whether the monitor is currently used by a window.
		/// @return True if monitor is currently used, false otherwise.
		[[nodiscard]] constexpr bool IsInUse() const noexcept;
		/// @brief Retrieve the monitors ID.
		/// @return Monitors ID.
		/// @note ID 0 is always the primary monitor!
		[[nodiscard]] u32 GetID() const;

		/// @brief Retrieve a pointer to the internal monitor.
		/// @return Pointer to the internal monitor of the Monitor.
		[[nodiscard]] constexpr void* GetInternalMonitor() const noexcept;

		/// @brief Retrieve all currently connected monitors.
		/// @return Vector with all monitors.
		[[nodiscard]] static std::vector<Monitor> GetAllMonitors();
		/// @brief Retrieve the primary monitor.
		/// @return Primary monitor.
		[[nodiscard]] static Monitor GetPrimaryMonitor();

	private:
		/// @brief Constructor.
		/// @param monitor Pointer to the InternalMonitor handle.
		constexpr explicit Monitor(INTERNAL::WindowingAPI::InternalMonitor* monitor);

		INTERNAL::WindowingAPI::InternalMonitor* m_handle{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Monitor::VideoMode::VideoMode(const u32 width, const u32 height, const f64 refreshRate) noexcept
	: Width(width), Height(height), RefreshRate(refreshRate)
{}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Monitor::GetName() const noexcept
{
	return m_handle->Name;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Monitor::IsInUse() const noexcept
{
	return m_handle->Window != nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr void* TRAP::Monitor::GetInternalMonitor() const noexcept
{
	return m_handle;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Monitor::Monitor(INTERNAL::WindowingAPI::InternalMonitor* const monitor)
	: m_handle(monitor)
{
	TRAP_ASSERT(m_handle, "Monitor::Monitor(): monitor is nullptr!");
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_MONITOR_H*/
