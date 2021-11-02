#ifndef TRAP_FILEWATCHER_H
#define TRAP_FILEWATCHER_H

#include "Utils/Time/Timer.h"

namespace TRAP
{
	class FileWatcher final
	{
	public:
		/// <summary>
		/// Specifies the status of a file.
		/// </summary>
		enum class FileStatus
		{
			/// <summary>
			/// Indicates a file creation.
			/// </summary>
			Created,
			/// <summary>
			/// Indicates a file modification.
			/// </summary>
			Modified,
			/// <summary>
			/// Indicates a file deletion.
			/// </summary>
			Erased
		};

		/// <summary>
		/// Keeps a record of files from every physicalPath inside the virtualPath and their last modification time.
		/// </summary>
		/// <param name="virtualPath">Path to track.</param>
		/// <param name="updateTimeInMilliseconds">
		/// Time in Milliseconds between file checks.<br>
		/// Minimum is 50ms (this prevents issues with programs that recreate files instead of updating them).
		/// </param>
		/// <returns>A New TRAP::FileWatcher instance.</returns>
		explicit FileWatcher(const std::string& virtualPath, float updateTimeInMilliseconds = 50.0f);

		/// <summary>
		/// Default Destructor.
		/// </summary>
		~FileWatcher() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		FileWatcher(const FileWatcher&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		FileWatcher(FileWatcher&&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		FileWatcher& operator=(const FileWatcher&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		FileWatcher& operator=(FileWatcher&&) = default;

		/// <summary>
		/// Check every file mounted to the virtualPath for changes and if so call the given action function.
		/// </summary>
		/// <param name="action">Function to be called each time when a file has been changed.</param>
		void Check(const std::function<void(std::filesystem::path, std::string, FileStatus)>& action);

	private:
		/// <summary>
		/// Convert a Physical file path to a virtual file path.<br>
		/// <br>
		/// Example return: "/foo/bar/FooBar.BAr".
		/// </summary>
		/// <param name="virtualPath">Virtual folder path.</param>
		/// <param name="physicalFilePath">Physical file path.</param>
		/// <returns>Virtual file path to the physical file path.</returns>
		std::string VirtualFilePathFormatter(const std::string& virtualPath,
		                                     const std::filesystem::directory_entry& physicalFilePath) const;

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

#endif /*TRAP_FILEWATCHER_H*/