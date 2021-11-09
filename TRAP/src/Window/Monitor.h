#ifndef TRAP_MONITOR_H
#define TRAP_MONITOR_H

#include "Window.h"
#include "WindowingAPI.h"

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
			VideoMode(int32_t width, int32_t height, int32_t refreshRate);

			int32_t Width = 0;
			int32_t Height = 0;
			int32_t RefreshRate = 0;
		};

		/// <summary>
		/// Destructor.
		/// </summary>
		~Monitor() = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		Monitor(const Monitor&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Monitor(Monitor&&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Monitor& operator=(const Monitor&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Monitor& operator=(Monitor&&) = default;

		/// <summary>
		/// Retrieve the name of a monitor.
		/// </summary>
		/// <returns>Name of the monitor.</returns>
		const std::string& GetName() const;
		/// <summary>
		/// Retrieve a list of available video modes for the monitor.
		/// </summary>
		/// <returns>Vector of video modes.</returns>
		std::vector<VideoMode> GetVideoModes() const;
		/// <summary>
		/// Retrieve the current video mode.
		/// </summary>
		/// <returns>Current video mode</returns>
		VideoMode GetCurrentVideoMode() const;
		/// <summary>
		/// Retrieve the monitors content scale.
		/// </summary>
		/// <returns>Vec2 containing the content scale.</returns>
		Math::Vec2 GetContentScale() const;
		/// <summary>
		/// Retrieve the monitors X content scale.
		/// </summary>
		/// <returns>Monitors X content scale.</returns>
		float GetContentScaleX() const;
		/// <summary>
		/// Retrieve the monitors Y content scale.
		/// </summary>
		/// <returns>Monitors Y content scale.</returns>
		float GetContentScaleY() const;
		/// <summary>
		/// Retrieve the monitors viewport position.
		/// </summary>
		/// <returns>Vec2i containing the monitors viewport position.</returns>
		Math::Vec2i GetPosition() const;
		/// <summary>
		/// Retrieve the monitors X viewport position.
		/// </summary>
		/// <returns>Monitors X viewport position.</returns>
		int32_t GetPositionX() const;
		/// <summary>
		/// Retrieve the monitors Y viewport position.
		/// </summary>
		/// <returns>Monitors Y viewport position.</returns>
		int32_t GetPositionY() const;
		/// <summary>
		/// Retrieve the monitors work area.
		///
		/// Work area is the area of a monitor without task bars or menu bars.
		/// </summary>
		/// <returns>Vec4i containing the monitors work area.</returns>
		Math::Vec4i GetWorkArea() const;
		/// <summary>
		/// Retrieve the monitors X work area.
		/// </summary>
		/// <returns>Monitors X work area.</returns>
		int32_t GetWorkAreaX() const;
		/// <summary>
		/// Retrieve the monitors Y work area.
		/// </summary>
		/// <returns>Monitors Y work area.</returns>
		int32_t GetWorkAreaY() const;
		/// <summary>
		/// Retrieve the monitors work area width.
		/// </summary>
		/// <returns>Monitors work area width.</returns>
		int32_t GetWorkAreaWidth() const;
		/// <summary>
		/// Retrieve the monitors work area height.
		/// </summary>
		/// <returns>Monitors work area height.</returns>
		int32_t GetWorkAreaHeight() const;
		/// <summary>
		/// Check whether the monitor is currently used by a window.
		/// </summary>
		/// <returns>True if monitor is currently used, false otherwise.</returns>
		bool IsInUse() const;
		/// <summary>
		/// Retrieve the monitors ID.
		///
		/// Note: ID 0 is always the primary monitor!
		/// </summary>
		/// <returns>Monitors ID.</returns>
		uint32_t GetID() const;

		/// <summary>
		/// Retrieve a pointer to the internal monitor.
		/// </summary>
		/// <returns>Pointer to the internal monitor of the Monitor.</returns>
		void* GetInternalMonitor() const;

		/// <summary>
		/// Retrieve all currently connected monitors.
		/// </summary>
		/// <returns>Vector with all monitors.</returns>
		static std::vector<Monitor> GetAllMonitors();
		/// <summary>
		/// Retrieve the primary monitor.
		/// </summary>
		/// <returns>Primary monitor.</returns>
		static Monitor GetPrimaryMonitor();

	private:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="monitor">Monitor ID.</param>
		explicit Monitor(uint32_t monitor);

		INTERNAL::WindowingAPI::InternalMonitor* m_handle{};

		friend TRAP::Monitor TRAP::Window::GetMonitor() const;
	};
}

#endif /*TRAP_MONITOR_H*/