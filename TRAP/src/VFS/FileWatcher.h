#ifndef _TRAP_FILEWATCHER_H_
#define _TRAP_FILEWATCHER_H_

#include "Utils/Time/Timer.h"

namespace TRAP
{
	class FileWatcher final
	{
	public:
		/// <summary>
		/// Specifies the status of a file
		/// </summary>
		enum class FileStatus
		{
			Created,
			Modified,
			Erased
		};
		
		/// <summary>
		/// Keeps a record of files from every physicalPath inside the virtualPath and their last modification time
		/// </summary>
		/// <param name="virtualPath">Path to track</param>
		/// <param name="updateTimeInMilliseconds">Time in Milliseconds between file checks. Minimum is 15ms (this prevents issues with programs that recreate files instead of updating them
		/// </param>
		/// <returns>New TRAP::FileWatcher instance</returns>
		FileWatcher(std::string virtualPath, float updateTimeInMilliseconds);

		/// <summary>
		/// Check every file mounted to the virtualPath for changes and if so call the given action function
		/// TODO Updating of physical (sub)folders that are mounted to the virtualPath
		/// </summary>
		/// <param name="action">Function to be called each time when a file has been changed</param>
		void Check(const std::function<void(std::filesystem::path, std::string, FileStatus)>& action);

	private:
		//Virtual folder to monitor
		std::string m_virtualPathToWatch;
		//All Physical paths corresponding to the virtualPath that should be monitored
		std::vector<std::filesystem::path> m_physicalPathsToWatch;

		//Physical paths bound to the Virtual path and the last written file time
		std::unordered_map<std::string, std::filesystem::file_time_type> m_physicalPaths;
		//Physical file list with corresponding virtual file
		std::unordered_map<std::string, std::string> m_virtualPaths;

		//Used for delaying next check
		float m_delay;
		Utils::Timer m_timer;
	};
}

#endif /*_TRAP_FILEWATCHER_H_*/