#ifndef TRAP_FILEWATCHER_H_
#define TRAP_FILEWATCHER_H_

namespace TRAP //TODO Put into FS namespace
{
    class FileWatcher
    {
    public:
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

        struct FileChangedEvent
        {
            FileStatus Status{};
            std::filesystem::path FilePath = "";
            bool IsDirectory = false;

            //If this is a rename event the new name will be in the the filepath.
            std::filesystem::path OldName = "";
        };

        using FileChangedCallbackFn = std::function<void(const std::vector<FileChangedEvent>&)>;

        /// <summary>
        /// Keeps track of the statuses of all files inside the specified paths.
        ///
        /// Note: This class starts a new thread.
        ///
        /// Windows: Event-based via ReadDirectoryChangesW.
        /// Linux: Poll-based (every 2 seconds) via inotify.
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
        /// Linux: Poll-based (every 2 seconds) via inotify.
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
        /// Skip the next file change event.
        /// </summary>
        void SkipNextFileChange();

        /// <summary>
        /// Sets the callback function that is called when a file change event occurs.
        /// </summary>
        /// <param name="callback">Callback function used to report events to.</param>
        void SetChangeCallback(const FileChangedCallbackFn& callback);

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

    private:
        void Init();
        void Shutdown();

        void Watch();

        void WatchWindows();
        void WatchLinux();

        std::thread m_thread;
        FileChangedCallbackFn m_callback;
        std::vector<std::filesystem::path> m_paths;
        bool m_recursive;
        bool m_run = true;
        bool m_skipNextFileChange = false;

#ifdef TRAP_PLATFORM_WINDOWS
        HANDLE m_killEvent = nullptr;
#endif
    };
}

#endif /*TRAP_FILEWATCHER_H_*/