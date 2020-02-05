#ifndef _TRAP_MONITOR_H_
#define _TRAP_MONITOR_H_

#include "WindowingAPI.h"
#include "VideoMode.h"

namespace TRAP
{	
	class Monitor
	{
	public:
		explicit Monitor(INTERNAL::WindowingAPI::InternalMonitor* monitor);
		explicit Monitor(uint32_t monitor);
		
		std::string GetName() const;
		std::vector<VideoMode> GetVideoModes() const;
		VideoMode GetCurrentMode() const;
		bool InUse() const;
		uint32_t GetID() const;
		void* GetInternalMonitor() const;

		static std::vector<Monitor> GetAllMonitors();
		static Monitor GetPrimaryMonitor();

	private:
		INTERNAL::WindowingAPI::InternalMonitor* m_handle{};
	};
}

#endif /*_TRAP_MONITOR_H_*/