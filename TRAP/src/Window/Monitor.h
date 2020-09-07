#ifndef _TRAP_MONITOR_H_
#define _TRAP_MONITOR_H_

#include "WindowingAPI.h"

namespace TRAP
{	
	class Monitor
	{
	public:
		struct VideoMode
		{
			int32_t Width = 0;
			int32_t Height = 0;
			int32_t RefreshRate = 0;
		};
		
		explicit Monitor(uint32_t monitor);
		
		~Monitor() = default;
		Monitor(const Monitor&) = default;
		Monitor(Monitor&&) = default;
		Monitor& operator=(const Monitor&) = default;
		Monitor& operator=(Monitor&&) = default;

		const std::string& GetName() const;
		std::vector<VideoMode> GetVideoModes() const;
		VideoMode GetCurrentVideoMode() const;
		Math::Vec2 GetContentScale() const;
		float GetContentScaleX() const;
		float GetContentScaleY() const;
		Math::Vec2i GetPosition() const;
		int32_t GetPositionX() const;
		int32_t GetPositionY() const;
		Math::Vec4i GetWorkArea() const;
		int32_t GetWorkAreaX() const;
		int32_t GetWorkAreaY() const;
		int32_t GetWorkAreaWidth() const;
		int32_t GetWorkAreaHeight() const;
		bool IsInUse() const;
		uint32_t GetID() const;
		void* GetInternalMonitor() const;

		static std::vector<Monitor> GetAllMonitors();
		static Monitor GetPrimaryMonitor();		

	private:
		INTERNAL::WindowingAPI::InternalMonitor* m_handle{};
	};
}

#endif /*_TRAP_MONITOR_H_*/