#ifndef TRAP_FILEWATCHER_H
#define TRAP_FILEWATCHER_H

#include <filesystem>
#include <functional>
#include <thread>
#include <atomic>

namespace TRAP::Events
{
    class Event;
}

namespace TRAP::FileSystem
{
    /// <summary>
    /// Specifies the status of a file.
    /// </summary>
    enum class FileStatus
    {
        Created,
        Renamed,
        Modified,
        Erased
    };

    class FileWatcher
    {
    public:
        /// <summary>
		/// Describes a callback function which gets called when an input event occurs.
		/// </summary>
		using EventCallbackFn = std::function<void(Events::Event&)>;

        /// <summary>
        /// Keeps track of the statuses of all files inside the specified paths.
        ///
        /// Note: This class starts a new thread.
        ///
        /// Windows: Event-based via ReadDirectoryChangesW.
        /// Linux: Event-based via inotify & eventfd.
        /// </summary>
        /// <param name="name">Name for the file watcher.</param>
        /// <param name="paths">Folder paths to watch over.</param>
        /// <param name="recursive">Whether to also include sub-folders inside the given paths.</param>
        explicit FileWatcher(std::string name, const std::vector<std::filesystem::path>& paths, bool recursive = true);
        /// <summary>
        /// Keeps track of the statuses of all files inside the specified path.
        ///
        /// Note: This class starts a new thread.
        ///
        /// Windows: Event-based via ReadDirectoryChangesW.
        /// Linux: Event-based via inotify & eventfd.
        /// </summary>
        /// <param name="name">Name for the file watcher.</param>
        /// <param name="path">Folder path to watch over.</param>
        /// <param name="recursive">Whether to also include sub-folders inside the given paths.</param>
        explicit FileWatcher(std::string name, const std::filesystem::path& path, bool recursive = true);

        /// <summary>
		/// Destructor.
		/// </summary>
		~FileWatcher();
		/// <summary>
		/// Copy constructor.
		/// </summary>
		FileWatcher(const FileWatcher&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		FileWatcher(FileWatcher&&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		FileWatcher& operator=(const FileWatcher&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		FileWatcher& operator=(FileWatcher&&) = delete;

        /// <summary>
        /// Sets the callback function that is called when a file event occurs.
        /// </summary>
        /// <param name="callback">Callback function used to report events to.</param>
        void SetEventCallback(const EventCallbackFn& callback);
		/// <summary>
		/// Get the function to call when an file event occurred.
		/// </summary>
		/// <returns>EventCallbackFn.</returns>
		EventCallbackFn GetEventCallback() const;

        /// <summary>Adds a new folder path to the tracked paths.</summary>
        /// <param name="path">Folder path to track.</param>
        void AddFolder(const std::filesystem::path& path);
        /// <summary>Adds new folder paths to the tracked paths.</summary>
        /// <param name="paths">Folder paths to track.</param>
        void AddFolders(const std::vector<std::filesystem::path>& paths);

        /// <summary>Removes a folder path from the tracked paths.</summary>
        /// <param name="path">Folder path to untrack.</param>
        void RemoveFolder(const std::filesystem::path& path);
        /// <summary>Removes folder paths from the tracked paths.</summary>
        /// <param name="paths">Folder paths to untrack.</param>
        void RemoveFolders(const std::vector<std::filesystem::path>& paths);

        /// <summary>
        /// Returns the paths that are being watched.
        /// </summary>
        /// <returns>The paths that are being watched.</returns>
        std::vector<std::filesystem::path> GetFolders() const;

    private:
        /// <summary>
        /// Initialize FileWatcher.
        /// </summary>
        void Init();
        /// <summary>
        /// Shutdown FileWatcher.
        /// </summary>
        void Shutdown();

        /// <summary>
        /// Watch over files.
        /// </summary>
        void Watch();

        std::thread m_thread;
        EventCallbackFn m_callback;
        std::vector<std::filesystem::path> m_paths; //No synchronization needed since it's only changed when m_thread is not running.
        bool m_recursive;
        std::atomic<bool> m_run;
        std::string m_name; //Doesn't need to be synced because it won't change after construction

#ifdef TRAP_PLATFORM_WINDOWS
        HANDLE m_killEvent = nullptr;
#elif defined(TRAP_PLATFORM_LINUX)
        int32_t m_killEvent = 0;
#endif
    };
}

#endif /*TRAP_FILEWATCHER_H*/
