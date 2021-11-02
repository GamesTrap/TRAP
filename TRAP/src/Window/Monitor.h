#ifndef TRAP_MONITOR_H
#define TRAP_MONITOR_H

#include "Window.h"
#include "WindowingAPI.h"

namespace TRAP
{
	/// <summary>
	/// Interface representing a physical Monitor.
	/// </summary>
	class Monitor
	{
	public:
		/// <summary>
		/// Interface representing a VideoMode used by TRAP::Monitors.
		/// </summary>
		struct VideoMode
		{
			/// <summary>
			/// VideoMode Constructor.
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
		/// Default Destructor.
		/// </summary>
		~Monitor() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		Monitor(const Monitor&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		Monitor(Monitor&&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		Monitor& operator=(const Monitor&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		Monitor& operator=(Monitor&&) = default;

		/// <summary>
		/// Retrieve the name of a TRAP::Monitor.
		/// </summary>
		/// <returns>Name of the TRAP::Monitor.</returns>
		const std::string& GetName() const;
		/// <summary>
		/// Retrieve a list of available TRAP::Monitor::VideoModes for the TRAP::Monitor.
		/// </summary>
		/// <returns>Vector of TRAP::Monitor::VideoModes.</returns>
		std::vector<VideoMode> GetVideoModes() const;
		/// <summary>
		/// Retrieve the TRAP::Monitors TRAP::Monitor::VideoMode.
		/// </summary>
		/// <returns>Current TRAP::Monitor::VideoMode</returns>
		VideoMode GetCurrentVideoMode() const;
		/// <summary>
		/// Retrieve the TRAP::Monitors content scale.
		/// </summary>
		/// <returns>TRAP::Math::Vec2 containing the content scale.</returns>
		Math::Vec2 GetContentScale() const;
		/// <summary>
		/// Retrieve the TRAP::Monitors X content scale.
		/// </summary>
		/// <returns>TRAP::Monitors X content scale.</returns>
		float GetContentScaleX() const;
		/// <summary>
		/// Retrieve the TRAP::Monitors Y content scale.
		/// </summary>
		/// <returns>TRAP::Monitors Y content scale.</returns>
		float GetContentScaleY() const;
		/// <summary>
		/// Retrieve the TRAP::Monitors Viewport position.
		/// </summary>
		/// <returns>TRAP::Math::Vec2i containing the TRAP::Monitors Viewport position.</returns>
		Math::Vec2i GetPosition() const;
		/// <summary>
		/// Retrieve the TRAP::Monitors X Viewport position.
		/// </summary>
		/// <returns>TRAP::Monitors X Viewport position.</returns>
		int32_t GetPositionX() const;
		/// <summary>
		/// Retrieve the TRAP::Monitors Y Viewport position.
		/// </summary>
		/// <returns>TRAP::Monitors Y Viewport position.</returns>
		int32_t GetPositionY() const;
		/// <summary>
		/// Retrieve the TRAP::Monitors work area.<br>
		/// <br>
		/// Work area is the area of a Monitor without task bars or menu bars.
		/// </summary>
		/// <returns>TRAP::Math::Vec4i containing the TRAP::Monitors work area.</returns>
		Math::Vec4i GetWorkArea() const;
		/// <summary>
		/// Retrieve the TRAP::Monitors X work area.
		/// </summary>
		/// <returns>TRAP::Monitors X work area.</returns>
		int32_t GetWorkAreaX() const;
		/// <summary>
		/// Retrieve the TRAP::Monitors Y work area.
		/// </summary>
		/// <returns>TRAP::Monitors Y work area.</returns>
		int32_t GetWorkAreaY() const;
		/// <summary>
		/// Retrieve the TRAP::Monitors work area width.
		/// </summary>
		/// <returns>TRAP::Monitors work area width.</returns>
		int32_t GetWorkAreaWidth() const;
		/// <summary>
		/// Retrieve the TRAP::Monitors work area height.
		/// </summary>
		/// <returns>TRAP::Monitors work area height.</returns>
		int32_t GetWorkAreaHeight() const;
		/// <summary>
		/// Check whether the TRAP::Monitor is currently used by a TRAP::Window.
		/// </summary>
		/// <returns>True if it is currently used, false otherwise.</returns>
		bool IsInUse() const;
		/// <summary>
		/// Retrieve the TRAP::Monitors ID.<br>
		/// <br>
		/// Note: ID 0 is always the primary monitor!
		/// </summary>
		/// <returns>TRAP::Monitors ID.</returns>
		uint32_t GetID() const;

		/// <summary>
		/// Retrieve a pointer to the InternalMonitor.
		/// </summary>
		/// <returns>Pointer to TRAP::INTERNAL::WindowingAPI::InternalMonitor of the TRAP::Monitor.</returns>
		void* GetInternalMonitor() const;

		/// <summary>
		/// Retrieve all currently connected Monitors.
		/// </summary>
		/// <returns>Vector with all TRAP::Monitors.</returns>
		static std::vector<Monitor> GetAllMonitors();
		/// <summary>
		/// Retrieve the primary Monitor.
		/// </summary>
		/// <returns>Primary TRAP::Monitor.</returns>
		static Monitor GetPrimaryMonitor();

	private:
		/// <summary>
		/// Private Constructor.
		/// </summary>
		/// <param name="monitor">Monitor ID.</param>
		explicit Monitor(uint32_t monitor);

		INTERNAL::WindowingAPI::InternalMonitor* m_handle{};

		friend TRAP::Monitor TRAP::Window::GetMonitor() const;
	};
}

#endif /*TRAP_MONITOR_H*/