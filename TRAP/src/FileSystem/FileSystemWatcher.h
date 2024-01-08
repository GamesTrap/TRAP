#ifndef TRAP_FILESYSTEMWATCHER_H
#define TRAP_FILESYSTEMWATCHER_H

#include <filesystem>
#include <functional>
#include <future>
#include <thread>
#include <atomic>
#include <shared_mutex>

#include <fmt/core.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
//Tracy - Profiler
#include <tracy/Tracy.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#include "TRAP_Assert.h"

#include "Utils/Optional.h"
#include "Utils/UniqueResource.h"

namespace TRAP::Events
{
    class Event;
}

namespace TRAP::FileSystem
{
    /// @brief Specifies the status of a file or folder.
    enum class FileSystemStatus
    {
        Created,
        Renamed,
        Modified,
        Erased
    };

    class FileSystemWatcher
    {
    public:
        /// @brief Describes a callback function which gets called when a event occurs.
		using EventCallbackFn = std::function<void(Events::Event&)>;

#ifdef TRAP_PLATFORM_WINDOWS
        using KillEvent = TRAP::UniqueResource<HANDLE, void(*)(HANDLE)>;
#elif defined(TRAP_PLATFORM_LINUX)
        using KillEvent = TRAP::UniqueResource<i32, void(*)(i32)>;
#endif

        /// @brief Keeps track of the status of all files and folders inside the added folders.
        /// @param recursive Whether to also include sub-folders recursively.
        /// @param debugName Name for the file watcher.
        /// @note This class uses an extra thread for tracking files and folders.
        /// @remark @win32 Event based via ReadDirectoryChangesW.
        /// @remark @linux Event-based via inotify and eventfd.
        explicit FileSystemWatcher(bool recursive = true, std::string debugName = "");

        /// @brief Destructor.
		~FileSystemWatcher() = default;
		/// @brief Copy constructor.
		consteval FileSystemWatcher(const FileSystemWatcher&) noexcept = delete;
		/// @brief Move constructor.
		consteval FileSystemWatcher(FileSystemWatcher&&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval FileSystemWatcher& operator=(const FileSystemWatcher&) noexcept = delete;
		/// @brief Move assignment operator.
		consteval FileSystemWatcher& operator=(FileSystemWatcher&&) noexcept = delete;

        /// @brief Sets the callback function that is called when a filesystem change event occurs.
        /// @param callback Callback function used to report events to.
        /// @note The callback must be threadsafe
        /// @threadsafe
        void SetEventCallback(const EventCallbackFn& callback);
        /// @brief Get the function to call when an filesystem change event occurred.
        /// @return EventCallbackFn.
        /// @threadsafe
		[[nodiscard]] EventCallbackFn GetEventCallback() const;

        /// @brief Adds a new folder path to the tracked paths.
        /// @param path Folder path to track.
        /// @return Future which can be used to wait for the changes to take effect.
        /// @threadsafe
        std::future<void> AddFolder(const std::filesystem::path& path);
        /// @brief Adds new folder paths to the tracked paths.
        /// @param paths Folder paths to track.
        /// @return Future which can be used to wait for the changes to take effect.
        /// @threadsafe
        std::future<void> AddFolders(std::vector<std::filesystem::path> paths);

        /// @brief Removes a folder path from the tracked paths.
        /// @param path Folder path to untrack.
        /// @threadsafe
        void RemoveFolder(const std::filesystem::path& path);
        /// @brief Removes folder paths from the tracked paths.
        /// @param paths Folder paths to untrack.
        /// @threadsafe
        void RemoveFolders(std::span<const std::filesystem::path> paths);

        /// @brief Returns the paths that are being watched.
        /// @return The paths that are being watched.
        /// @note If recursive watching is enabled then there may be additional
        ///       paths being tracked which aren't part of this list.
        /// @threadsafe
        [[nodiscard]] constexpr std::vector<std::filesystem::path> GetFolders() const noexcept;

    private:
        /// @brief Initialize FileWatcher.
        /// @param optPromise Optional promise to set after watcher thread initialization has finished
        void Init(TRAP::Optional<std::promise<void>> optPromise = TRAP::NullOpt);
        /// @brief Shutdown FileWatcher.
        void Shutdown();

        /// @brief Watch over files.
        /// @param stopToken Token to use for stop request.
        /// @param pathsToWatch Folders to watch.
        /// @param optPromise Optional promise to signal after watcher thread initialization has finished
        void Watch(const std::stop_token& stopToken, std::vector<std::filesystem::path> pathsToWatch,
                   TRAP::Optional<std::promise<void>> optPromise);

        /// @brief Callback to stop the running file watcher thread.
        void StopCallback() const;

        mutable TracySharedLockable(std::shared_mutex, m_mtx);

        //Thread unsafe members
        EventCallbackFn m_callback = nullptr;

        //Thread safe members
        std::vector<std::filesystem::path> m_paths{};
        bool m_recursive = true; //Safe, only set on construction, read-only afterwards
        std::string m_debugName; //Safe, only set on construction, read-only afterwards
        KillEvent m_killEvent; //Safe to use while watcher thread isnt running

        std::jthread m_thread{}; //Stay at the bottom so stop requests can join thread without hitting exceptions or deadlock
    };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::filesystem::path> TRAP::FileSystem::FileSystemWatcher::GetFolders() const noexcept
{
    return m_paths;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::FileSystem::FileSystemStatus>
{
    static constexpr auto parse(fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::FileSystem::FileSystemStatus& status, fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(status)
        {
        case TRAP::FileSystem::FileSystemStatus::Created:
            enumStr = "Created";
            break;
        case TRAP::FileSystem::FileSystemStatus::Renamed:
            enumStr = "Renamed";
            break;
        case TRAP::FileSystem::FileSystemStatus::Modified:
            enumStr = "Modified";
            break;
        case TRAP::FileSystem::FileSystemStatus::Erased:
            enumStr = "Erased";
            break;

        default:
            TRAP_ASSERT(false, "fmt::formatter<TRAP::FileSystem::FileSystemStatus>: Missing enum value!");
            enumStr = "<MISSING ENUM VALUE>";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

#endif /*TRAP_FILESYSTEMWATCHER_H*/
