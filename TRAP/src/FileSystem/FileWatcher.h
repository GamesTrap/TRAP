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
    /// @brief Specifies the status of a file.
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
        /// @brief Describes a callback function which gets called when a event occurs.
		using EventCallbackFn = std::function<void(Events::Event&)>;

        /// @brief Keeps track of the status of all files inside the added folders.
        /// @param name Name for the file watcher.
        /// @param recursive Whether to also include sub-folders.
        /// @note This class uses an extra thread for tracking files and folders.
        /// @remark @win32 Event based via ReadDirectoryChangesW.
        /// @remark @linux Event-based via inotify and eventfd.
        explicit FileWatcher(std::string name, bool recursive = true);

        /// @brief Destructor.
		~FileWatcher() = default;
		/// @brief Copy constructor.
		consteval FileWatcher(const FileWatcher&) = delete;
		/// @brief Move constructor.
		FileWatcher(FileWatcher&&) noexcept = default;
		/// @brief Copy assignment operator.
		consteval FileWatcher& operator=(const FileWatcher&) = delete;
		/// @brief Move assignment operator.
		FileWatcher& operator=(FileWatcher&&) noexcept = default;

        /// @brief Sets the callback function that is called when a file event occurs.
        /// @param callback Callback function used to report events to.
        void SetEventCallback(const EventCallbackFn& callback) noexcept;
        /// @brief Get the function to call when an file event occurred.
        /// @return EventCallbackFn.
		[[nodiscard]] EventCallbackFn GetEventCallback() const noexcept;

        /// @brief Adds a new folder path to the tracked paths.
        /// @param path Folder path to track.
        void AddFolder(const std::filesystem::path& path);
        /// @brief Adds new folder paths to the tracked paths.
        /// @param paths Folder paths to track.
        void AddFolders(const std::vector<std::filesystem::path>& paths);

        /// @brief Removes a folder path from the tracked paths.
        /// @param path Folder path to untrack.
        void RemoveFolder(const std::filesystem::path& path);
        /// @brief Removes folder paths from the tracked paths.
        /// @param paths Folder paths to untrack.
        void RemoveFolders(const std::vector<std::filesystem::path>& paths);

        /// @brief Returns the paths that are being watched.
        /// @return The paths that are being watched.
        [[nodiscard]] constexpr const std::vector<std::filesystem::path>& GetFolders() const noexcept;

    private:
        /// @brief Initialize FileWatcher.
        void Init();
        /// @brief Shutdown FileWatcher.
        void Shutdown();

        /// @brief Watch over files.
        /// @param stopToken Token to use for stop request.
        void Watch(const std::stop_token& stopToken);

        /// @brief Callback to stop the running file watcher thread.
        void StopCallback() const;

        EventCallbackFn m_callback = nullptr;
        std::vector<std::filesystem::path> m_paths{}; //No synchronization needed since it's only changed when m_thread is not running.
        bool m_recursive = true;
        std::string m_name; //Doesn't need to be synced because it won't change after construction

#ifdef TRAP_PLATFORM_WINDOWS
        HANDLE m_killEvent = nullptr;
#elif defined(TRAP_PLATFORM_LINUX)
        i32 m_killEvent = 0;
#endif

        std::jthread m_thread{}; //Stay at the bottom so stop requests can join thread without hitting exceptions or deadlock
    };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<std::filesystem::path>& TRAP::FileSystem::FileWatcher::GetFolders() const noexcept
{
    return m_paths;
}

#endif /*TRAP_FILEWATCHER_H*/
