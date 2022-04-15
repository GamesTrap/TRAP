#ifndef TRAP_FILEWATCHER_H
#define TRAP_FILEWATCHER_H

namespace TRAP::Events
{
    class Event;
}

namespace TRAP::FS
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
        /// <param name="paths">Folder paths to watch over.</param>
        /// <param name="recursive">Whether to also include sub-folders inside the given paths.</param>
        explicit FileWatcher(const std::vector<std::filesystem::path>& paths, bool recursive = true);
        /// <summary>
        /// Keeps track of the statuses of all files inside the specified path.
        ///
        /// Note: This class starts a new thread.
        ///
        /// Windows: Event-based via ReadDirectoryChangesW.
        /// Linux: Event-based via inotify & eventfd.
        /// </summary>
        /// <param name="path">Folder path to watch over.</param>
        /// <param name="recursive">Whether to also include sub-folders inside the given paths.</param>
        explicit FileWatcher(const std::filesystem::path& path, bool recursive = true);

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
		FileWatcher(FileWatcher&&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		FileWatcher& operator=(const FileWatcher&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		FileWatcher& operator=(FileWatcher&&) = default;

        /// <summary>
        /// Skip the next file event.
        /// </summary>
        void SkipNextFileChange();

        /// <summary>
        /// Sets the callback function that is called when a file event occurs.
        /// </summary>
        /// <param name="callback">Callback function used to report events to.</param>
        void SetEventCallback(const EventCallbackFn& callback);
		/// <summary>
		/// Get the function to call when an file event occurred.
		/// </summary>
		/// <returns>EventCallbackFn.</returns>
		EventCallbackFn GetEventCallback();

        /// <summary>Adds a new folder path to the tracked paths.</summary>
        /// <param name="path">Folder path to track.</param>
        void AddFolder(std::filesystem::path path);
        /// <summary>Adds new folder paths to the tracked paths.</summary>
        /// <param name="paths">Folder paths to track.</param>
        void AddFolders(const std::vector<std::filesystem::path>& paths);

        /// <summary>Removes a folder path from the tracked paths.</summary>
        /// <param name="path">Folder path to untrack.</param>
        void RemoveFolder(std::filesystem::path path);
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
        /// Used by Windows.
        /// </summary>
        void WatchWindows();
        /// <summary>
        /// Watch over files.
        /// Used by Linux.
        /// </summary>
        void WatchLinux();

        std::thread m_thread;
        EventCallbackFn m_callback;
        std::vector<std::filesystem::path> m_paths;
        bool m_recursive;
        bool m_run;
        bool m_skipNextFileChange;

#ifdef TRAP_PLATFORM_WINDOWS
        HANDLE m_killEvent = nullptr;
#elif defined(TRAP_PLATFORM_LINUX)
        int32_t m_killEvent = 0;
#endif
    };
}

#endif /*TRAP_FILEWATCHER_H*/