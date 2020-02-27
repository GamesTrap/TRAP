#ifndef _TRAP_FILEWATCHER_H_
#define _TRAP_FILEWATCHER_H_

#include "Utils/Time/Timer.h"

namespace TRAP
{
	//Define available file changes
	enum class FileStatus
	{
		Created,
		Modified,
		Erased
	};

	class FileWatcher final
	{
	public:
		//Keep a record of files from every physicalPath inside virtualPath and their last modification time
		FileWatcher(const std::string& virtualPath, float updateTimeInMilliseconds);

		//Monitor physicalPaths inside virtualPath for changes and in case of a change execute the user supplied action function
		void Check(const std::function<void(std::filesystem::path, std::string, FileStatus)>& action);

	private:
		std::string m_virtualPathToWatch;
		std::vector<std::filesystem::path> m_physicalPathsToWatch;

		std::unordered_map<std::string, std::filesystem::file_time_type> m_physicalPaths;
		std::unordered_map<std::string, std::string> m_virtualPaths;

		float m_delay;
		Utils::Timer m_timer;
	};
}

#endif /*_TRAP_FILEWATCHER_H_*/