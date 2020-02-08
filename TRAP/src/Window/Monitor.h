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
		
		explicit Monitor(INTERNAL::WindowingAPI::InternalMonitor* monitor);
		explicit Monitor(uint32_t monitor);
		
		~Monitor() = default;
		Monitor(const Monitor&) = default;
		Monitor(Monitor&&) = default;
		Monitor& operator=(const Monitor&) = default;
		Monitor& operator=(Monitor&&) = default;

		std::string GetName() const;
		std::vector<VideoMode> GetVideoModes() const;
		VideoMode GetCurrentVideoMode() const;
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