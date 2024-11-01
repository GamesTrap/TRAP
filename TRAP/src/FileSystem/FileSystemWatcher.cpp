#include "TRAPPCH.h"
#include "FileSystemWatcher.h"

#include "FileSystem.h"
#include "Events/FileEvent.h"
#include "Utils/String/String.h"
#include "Utils/UniqueResource.h"

TRAP::FileSystem::FileSystemWatcher::FileSystemWatcher(const bool recursive, std::string debugName)
    : m_recursive(recursive), m_debugName(std::move(debugName))
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::FileSystemWatcher::SetEventCallback(const EventCallbackFn& callback)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    *m_callback.WriteLock() = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::FileSystem::FileSystemWatcher::EventCallbackFn TRAP::FileSystem::FileSystemWatcher::GetEventCallback() const
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    return *m_callback.ReadLock();
}

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
    [[nodiscard]] bool IsSubPath(const std::filesystem::path& destination, const std::filesystem::path& base)
    {
        const std::string rel = std::filesystem::relative(destination, base).string();
        return rel.size() == 1 || (rel.size() > 1 && rel[0] != '.' && rel[1] != '.');
    }

    void EraseSubDirs(std::vector<std::filesystem::path>& paths)
    {
        std::erase_if(paths, [&paths](const std::filesystem::path& path)
        {
            return std::ranges::any_of(paths, [&path](const std::filesystem::path& other)
            {
                return (path != other) && IsSubPath(path, other);
            });
        });
    }
}

std::future<void> TRAP::FileSystem::FileSystemWatcher::AddFolder(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!path.empty(), "FileWatcher::AddFolder(): Path can not be empty!");

    std::promise<void> promise{};
    std::future<void> fut = promise.get_future();

    if(path.empty())
    {
        TP_ERROR(Log::FileWatcherPrefix, "AddFolder(): Path can not be empty!");
        promise.set_value();
        return fut;
    }

    if(!TRAP::FileSystem::IsFolder(path))
    {
        TP_ERROR(Log::FileWatcherPrefix, "AddFolder(): Path does not lead to a folder!");
        promise.set_value();
        return fut;
    }

    //Always use absolute paths
    const auto absPath = FileSystem::ToAbsolutePath(path);
    if(!absPath)
    {
        promise.set_value();
        return fut;
    }

    //Only add if not already in list
    if(!std::ranges::contains(m_paths, *absPath))
    {
        m_paths.push_back(*absPath);
        if(m_recursive)
            EraseSubDirs(m_paths);

        Shutdown();
        Init(std::move(promise));
    }

    return fut;
}

//-------------------------------------------------------------------------------------------------------------------//

std::future<void> TRAP::FileSystem::FileSystemWatcher::AddFolders(std::vector<std::filesystem::path> paths)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!paths.empty(), "FileWatcher::AddFolders(): Paths can not be empty!");

    std::promise<void> promise{};

    std::future<void> fut = promise.get_future();

    if(paths.empty())
    {
        promise.set_value();
        return fut;
    }

    ContainerAppendRange(m_paths, paths |
                                  std::views::filter(FileSystem::IsFolder) |
                                  std::views::transform(FileSystem::ToAbsolutePath) |
                                  std::views::filter([this](const auto& p){return p.HasValue() && !std::ranges::contains(m_paths, *p);}) |
                                  std::views::transform([](const auto& p){return *p;}));
    if(m_recursive)
        EraseSubDirs(m_paths);
    Shutdown();
    Init(std::move(promise));

    return fut;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::FileSystemWatcher::RemoveFolder(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!path.empty(), "FileWatcher::RemoveFolder(): Path can not be empty!");

    if(path.empty())
    {
        TP_ERROR(Log::FileWatcherPrefix, "RemoveFolder(): Path can not be empty!");
        return;
    }

    //Always use absolute paths
    const auto absPath = FileSystem::ToAbsolutePath(path);
    if(!absPath)
        return;

    //Check if folder is in the list
    if(std::ranges::contains(m_paths, *absPath))
    {
        std::erase(m_paths, *absPath);
        Shutdown();
        Init();
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::FileSystemWatcher::RemoveFolders(const std::span<const std::filesystem::path> paths)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!paths.empty(), "FileWatcher::RemoveFolders(): Paths can not be empty!");

    if(paths.empty())
        return;

    for(const std::filesystem::path& path : paths |
                                            std::views::transform(FileSystem::ToAbsolutePath) |
                                            std::views::filter([](const auto& p){return p.HasValue();}) |
                                            std::views::transform([](const auto& p){return *p;}))
    {
        std::erase(m_paths, path);
    }
    Shutdown();
    Init();
}

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
#ifdef TRAP_PLATFORM_WINDOWS
    void KillEventDeleter(HANDLE event)
    {
        if(!CloseHandle(event))
            TP_ERROR(TRAP::Log::FileWatcherWindowsPrefix, "Failed to close kill event (", TRAP::Utils::String::GetStrError(), ")");
    }

    [[nodiscard]] bool CreateKillEvent(TRAP::FileSystem::FileSystemWatcher::KillEvent& event)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        event = TRAP::MakeUniqueResourceChecked(CreateEvent(nullptr, FALSE, FALSE, nullptr), nullptr, KillEventDeleter);
        if(!event.Get())
        {
            TP_ERROR(TRAP::Log::FileWatcherWindowsPrefix, "Failed to create kill event (", TRAP::Utils::String::GetStrError(), ")!");
            return false;
        }

        return true;
    }
#elif defined(TRAP_PLATFORM_LINUX)
    void KillEventDeleter(const i32 event)
    {
        if(close(event) < 0)
            TP_ERROR(TRAP::Log::FileWatcherLinuxPrefix, "Failed to close eventfd (", TRAP::Utils::String::GetStrError(), ")");
    }

    [[nodiscard]] bool CreateKillEvent(TRAP::FileSystem::FileSystemWatcher::KillEvent& event)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        event = TRAP::MakeUniqueResourceChecked(eventfd(0, 0), -1, KillEventDeleter);

        if(event.Get() < 0)
        {
            TP_ERROR(TRAP::Log::FileWatcherLinuxPrefix, "Failed to create kill event (", TRAP::Utils::String::GetStrError(), ")!");
            return false;
        }

        return true;
    }
#endif
}

void TRAP::FileSystem::FileSystemWatcher::Init(TRAP::Optional<std::promise<void>> optPromise)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    //Remove all paths which dont exist on disc
    std::erase_if(m_paths, std::not_fn(FileSystem::Exists));

    //Remove all duplicate paths
    std::ranges::sort(m_paths);
    const auto last = std::ranges::unique(m_paths, FileSystem::IsEquivalent);
    m_paths.erase(last.begin(), m_paths.end());

    if(m_paths.empty())
    {
        if(optPromise)
            optPromise->set_value();
        return;
    }

    if(!CreateKillEvent(m_killEvent))
    {
        if(optPromise)
            optPromise->set_value();
        TP_ERROR(Log::FileWatcherLinuxPrefix, "Aborting start of FileWatcher!");
        return;
    }

    m_thread = std::jthread(std::bind_front(&TRAP::FileSystem::FileSystemWatcher::Watch, this), m_paths, std::move(optPromise));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::FileSystemWatcher::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    m_thread.request_stop();
    if (m_thread.joinable())
    {
        m_thread.join();
        m_killEvent.Reset();
    }
}

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
#ifdef TRAP_PLATFORM_LINUX
    constexpr u64 KILLTHREAD = 1;
    void SetKillEvent(const i32 killEvent)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        isize toSend = sizeof(KILLTHREAD);
        while (toSend > 0)
        {
            const isize res = write(killEvent, &KILLTHREAD, toSend);
            if(res < 0)
            {
                const std::string errStr = TRAP::Utils::String::GetStrError();
                TP_ERROR(TRAP::Log::FileWatcherLinuxPrefix, "Error writing to eventfd (", errStr, ")");
                throw std::runtime_error(fmt::format("FileWatcher::StopCallback(): Failed to set kill event ({}), aborting!", errStr));
            }
            toSend -= res;
        }
    }
#elif defined(TRAP_PLATFORM_WINDOWS)
    void SetKillEvent(HANDLE killEvent)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        if(!SetEvent(killEvent))
        {
            const std::string errStr = TRAP::Utils::String::GetStrError();
            TP_ERROR(TRAP::Log::FileWatcherWindowsPrefix, "Failed to set kill event (", errStr, ")!");
            throw std::runtime_error(fmt::format("FileWatcher::StopCallback(): Failed to set kill event ({}), aborting!", errStr));
        }
    }
#endif
}

void TRAP::FileSystem::FileSystemWatcher::StopCallback() const
{
    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    //Note: This deadlocks if m_killEvent can't be set, so throw if this happens

    SetKillEvent(m_killEvent.Get());
}

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
    constexpr void DeduplicateEvents(std::vector<TRAP::Events::FileSystemChangeEvent>& events)
    {
        const auto last = std::ranges::unique(events, [](const auto& lhs, const auto& rhs)
        {
            return lhs.GetStatus() == rhs.GetStatus() && lhs.GetPath() == rhs.GetPath() && lhs.GetOldPath() == rhs.GetOldPath();
        });

        events.erase(last.begin(), events.end());
    }

    void DispatchEvents(std::vector<TRAP::Events::FileSystemChangeEvent>& events,
                        const TRAP::FileSystem::FileSystemWatcher::EventCallbackFn& callback)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        if(callback)
        {
            for(TRAP::Events::FileSystemChangeEvent& e : events)
                callback(e);
        }
        events.clear();
    }
}

#ifdef TRAP_PLATFORM_WINDOWS

namespace
{
    void DirectoryHandleDeleter(uptr directoryHandle)
    {
        if(!CloseHandle(reinterpret_cast<HANDLE>(directoryHandle)))
            TP_ERROR(TRAP::Log::FileWatcherWindowsPrefix, "Failed to close directory handle (", TRAP::Utils::String::GetStrError(), ")");
    }

    using DirectoryHandle = TRAP::UniqueResource<uptr, decltype(&DirectoryHandleDeleter)>;

    [[nodiscard]] TRAP::Optional<DirectoryHandle> CreateDirectoryHandle(const std::filesystem::path& p)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        static constexpr DWORD desiredAccess = FILE_LIST_DIRECTORY;
        static constexpr DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
        static constexpr DWORD creationDisposition = OPEN_EXISTING;
        static constexpr DWORD flagsAndAttributes = FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED;
        const HANDLE rawDirHandle = CreateFile(p.c_str(), desiredAccess, shareMode, nullptr, creationDisposition, flagsAndAttributes, nullptr);

        if(rawDirHandle == INVALID_HANDLE_VALUE)
        {
            TP_ERROR(TRAP::Log::FileWatcherWindowsPrefix, "Failed to open directory: ", p, " (", TRAP::Utils::String::GetStrError(), ")");
            return TRAP::NullOpt;
        }

        return TRAP::MakeUniqueResourceChecked(reinterpret_cast<uptr>(rawDirHandle), reinterpret_cast<uptr>(INVALID_HANDLE_VALUE), DirectoryHandleDeleter);
    }

    [[nodiscard]] std::vector<std::pair<DirectoryHandle, std::filesystem::path>> CreateDirectoryHandles(const std::span<const std::filesystem::path> paths)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        std::vector<std::pair<DirectoryHandle, std::filesystem::path>> dirHandles{};
        dirHandles.reserve(paths.size());

        for(const std::filesystem::path& p : paths)
        {
            if(auto dirHandle = CreateDirectoryHandle(p))
                dirHandles.emplace_back(std::move(*dirHandle), p);
        }

        return dirHandles;
    }

    void OverlappedEventDeleter(uptr overlapped)
    {
        if(!CloseHandle(reinterpret_cast<HANDLE>(overlapped)))
            TP_ERROR(TRAP::Log::FileWatcherWindowsPrefix, "Failed to close polling overlap event handle (", TRAP::Utils::String::GetStrError(), ")");
    }

    using OverlappedEvent = TRAP::UniqueResource<uptr, decltype(&OverlappedEventDeleter)>;
    [[nodiscard]] TRAP::Optional<OverlappedEvent> CreateOverlappedEvent()
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        const HANDLE overlappedEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

        if(!overlappedEvent)
        {
            TP_ERROR(TRAP::Log::FileWatcherWindowsPrefix, "Failed to create polling overlap event (", TRAP::Utils::String::GetStrError(), ")!");
            return TRAP::NullOpt;
        }

        return TRAP::MakeUniqueResourceChecked(reinterpret_cast<uptr>(overlappedEvent), false, OverlappedEventDeleter);
    }

    [[nodiscard]] bool ReadAllDirectoryChangesAsync(const std::span<const std::pair<DirectoryHandle, std::filesystem::path>> dirHandles,
                                                    OVERLAPPED& pollingOverlap, const bool recursive,
                                                    std::vector<std::array<char, 2048>>& rawEventBuffers)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        for(usize i = 0; i < rawEventBuffers.size(); ++i)
        {
            static constexpr DWORD notifyFilter = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE;
            const BOOL result = ReadDirectoryChangesW(reinterpret_cast<HANDLE>(dirHandles[i].first.Get()), rawEventBuffers[i].data(),
                                                      NumericCast<DWORD>(rawEventBuffers[i].size()), recursive, notifyFilter, nullptr,
                                                      &pollingOverlap, nullptr);
            if(!result)
            {
                TP_ERROR(TRAP::Log::FileWatcherWindowsPrefix, "Failed to read directory changes (", TRAP::Utils::String::GetStrError(), ")");
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] TRAP::Optional<DWORD> WaitForFileEvents(OVERLAPPED& pollingOverlap,
                                                          const TRAP::FileSystem::FileSystemWatcher::KillEvent& killEvent)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        const std::array<HANDLE, 2> handles = {pollingOverlap.hEvent, killEvent.Get()};
        const DWORD res = WaitForMultipleObjects(NumericCast<DWORD>(handles.size()), handles.data(), FALSE, INFINITE);

        if(res == WAIT_FAILED)
        {
            TP_ERROR(TRAP::Log::FileWatcherWindowsPrefix, "Failed to wait for multiple objects (", TRAP::Utils::String::GetStrError(), ")");
            return TRAP::NullOpt;
        }

        return res;
    }

    [[nodiscard]] constexpr bool IsKillEventTriggered(const DWORD waitResult)
    {
        return waitResult == (WAIT_OBJECT_0 + 1);
    }

    [[nodiscard]] constexpr bool IsFileEventTriggered(const DWORD waitResult)
    {
        return waitResult == (WAIT_OBJECT_0);
    }

    void ProcessFileEvent(std::array<char, 2048>& rawEventData,
                          const std::pair<DirectoryHandle, std::filesystem::path>& dirHandle,
                          OVERLAPPED& pollingOverlap,
                          std::vector<TRAP::Events::FileSystemChangeEvent>& events)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        DWORD bytesTransferred{};
        GetOverlappedResult(reinterpret_cast<HANDLE>(dirHandle.first.Get()),
                            &pollingOverlap, &bytesTransferred, FALSE);

        u32 offset = 0;
        const FILE_NOTIFY_INFORMATION* notify;

        std::filesystem::path oldFilePath = "";
        do
        {
            notify = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(rawEventData.data() + offset);

            const usize filenameLength = notify->FileNameLength / sizeof(wchar_t);
            if (filenameLength == 0)
            {
                if(notify->NextEntryOffset == 0)
                    break;

                offset += notify->NextEntryOffset;
                continue;
            }

            const std::filesystem::path filePath = (dirHandle.second / std::wstring(notify->FileName, filenameLength));
            TRAP::FileSystem::FileSystemStatus status;

            switch(notify->Action)
            {
            case FILE_ACTION_ADDED:
                status = TRAP::FileSystem::FileSystemStatus::Created;
                break;

            case FILE_ACTION_REMOVED:
                status = TRAP::FileSystem::FileSystemStatus::Erased;
                break;

            case FILE_ACTION_MODIFIED:
                status = TRAP::FileSystem::FileSystemStatus::Modified;
                break;

            case FILE_ACTION_RENAMED_OLD_NAME:
                oldFilePath = filePath;
                break;

            case FILE_ACTION_RENAMED_NEW_NAME:
                status = TRAP::FileSystem::FileSystemStatus::Renamed;
                break;

            default:
                break;
            }

            if (notify->Action == FILE_ACTION_RENAMED_NEW_NAME)
            {
                events.emplace_back(status, filePath, oldFilePath);
                oldFilePath.clear();
            }
            else if(notify->Action != FILE_ACTION_RENAMED_OLD_NAME)
                events.emplace_back(status, filePath, TRAP::NullOpt);

            offset += notify->NextEntryOffset;
        } while(notify->NextEntryOffset);

        //Clear buffer
        std::ranges::fill(rawEventData, '\0');
    }

    void ProcessFileEvents(std::vector<std::array<char, 2048>>& rawEventBuffers,
                           std::span<const std::pair<DirectoryHandle, std::filesystem::path>> dirHandles,
                           OVERLAPPED& pollingOverlap, std::vector<TRAP::Events::FileSystemChangeEvent>& events)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        std::wstring oldName;
        for(usize i = 0; i < rawEventBuffers.size(); ++i)
            ProcessFileEvent(rawEventBuffers[i], dirHandles[i], pollingOverlap, events);
    }
}

void TRAP::FileSystem::FileSystemWatcher::Watch(const std::stop_token& stopToken, const std::vector<std::filesystem::path>& pathsToWatch,
                                                TRAP::Optional<std::promise<void>> optPromise)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    const std::stop_callback stopCallback(stopToken, [this] { StopCallback(); });
    TRAP::Utils::SetThreadName(m_debugName.empty() ? "FileSystemWatcher" : fmt::format("{} (FileSystemWatcher)", m_debugName));

    //Thread init
    std::vector<std::pair<DirectoryHandle, std::filesystem::path>> dirHandles = CreateDirectoryHandles(pathsToWatch);

    if(optPromise)
        optPromise->set_value();

    if(dirHandles.empty())
        return;

    const auto pollingOverlapEvent = CreateOverlappedEvent();
    if (!pollingOverlapEvent)
        return;

    OVERLAPPED pollingOverlap{};
    pollingOverlap.hEvent = reinterpret_cast<HANDLE>(pollingOverlapEvent->Get());

    std::vector<std::array<char, 2048>> rawEventBuffers(dirHandles.size()); //Can't be a FILE_NOTIFY_INFORMATION array because it includes a flexible array member.

    //Thread work loop
    std::vector<Events::FileSystemChangeEvent> events;
    while(!stopToken.stop_requested())
    {
        if(!ReadAllDirectoryChangesAsync(dirHandles, pollingOverlap, m_recursive, rawEventBuffers))
            return;

        const auto waitRes = WaitForFileEvents(pollingOverlap, m_killEvent);
        if(!waitRes || IsKillEventTriggered(*waitRes))
            return;
        if(!IsFileEventTriggered(*waitRes))
            continue;

        ProcessFileEvents(rawEventBuffers, dirHandles, pollingOverlap, events);

        if(!events.empty())
        {
            DeduplicateEvents(events);
            DispatchEvents(events, *m_callback.ReadLock());
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//

#elif defined(TRAP_PLATFORM_LINUX)

namespace
{
    using FileDescriptor = TRAP::UniqueResource<i32, void(*)(i32)>;
    constexpr i32 InvalidFileDescriptor = -1;

    void FileDescriptorDeleter(const i32 fd)
    {
        if(close(fd) < 0)
            TP_ERROR(TRAP::Log::FileWatcherLinuxPrefix, "Failed to close inotify (", TRAP::Utils::String::GetStrError(), ")");
    }

    TRAP::Optional<std::array<pollfd, 2>> CreateFileDescriptors(i32 killEventFD)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        const std::array<pollfd, 2> fileDescriptors
        {
            {
                {
                    .fd = inotify_init(),
                    .events = POLLIN,
                    .revents = 0
                },
                {
                    .fd = killEventFD,
                    .events = POLLIN,
                    .revents = 0
                }
            }
        };

        if(std::get<0>(fileDescriptors).fd < 0) //inotify_init() failed
        {
            TP_ERROR(TRAP::Log::FileWatcherLinuxPrefix, "Failed to initialize inotify (", TRAP::Utils::String::GetStrError(), ")");
            return TRAP::NullOpt;
        }

        return fileDescriptors;
    }

    void CreateInotifyWatch(const std::filesystem::path& pathToWatch, const FileDescriptor& inotifyFD,
                            std::unordered_map<i32, std::filesystem::path>& watchDescriptors,
                            const bool recursive)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        if(recursive)
        {
            std::error_code ec{};
            const std::filesystem::directory_iterator dirIt(pathToWatch, ec);
            if(!ec)
            {
                for(const auto& subPath : dirIt)
                {
                    if(!subPath.is_directory())
                        continue;

                    CreateInotifyWatch(subPath.path(), inotifyFD, watchDescriptors, recursive);
                }
            }
        }

        const i32 watchDesc = inotify_add_watch(inotifyFD.Get(), pathToWatch.string().c_str(), IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);

        if(watchDesc < 0)
        {
            TP_ERROR(TRAP::Log::FileWatcherLinuxPrefix, "Failed to add watch for path: ", pathToWatch, " (", TRAP::Utils::String::GetStrError(), ")");
            return;
        }

        watchDescriptors[watchDesc] = pathToWatch;
    }

    void CreateInotifyWatches(const std::span<const std::filesystem::path> pathsToWatch,
                              const FileDescriptor& inotifyFD,
                              std::unordered_map<i32, std::filesystem::path>& watchDescriptors,
                              const bool recursive)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        for(const auto& path : pathsToWatch)
            CreateInotifyWatch(path, inotifyFD, watchDescriptors, recursive);
    }

    [[nodiscard]] bool PollFileDescriptors(std::array<pollfd, 2>& fileDescriptors)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        static constexpr i32 InfiniteTimeout = -1;
        if(poll(fileDescriptors.data(), fileDescriptors.size(), InfiniteTimeout) < 0)
        {
            TP_ERROR(TRAP::Log::FileWatcherLinuxPrefix, "Failed to poll events (", TRAP::Utils::String::GetStrError(), ")");
            return false;
        }

        //Check inotify file descriptor
        const pollfd& inotifyFD = std::get<0>(fileDescriptors);
        if((inotifyFD.revents & POLLHUP) != 0 || (inotifyFD.revents & POLLERR) != 0)
        {
            TP_ERROR(TRAP::Log::FileWatcherLinuxPrefix, "Polling error");
            return false;
        }

        //Check kill event file descriptor
        const pollfd& killEventFD = std::get<1>(fileDescriptors);
        if((killEventFD.revents & POLLHUP) != 0 || (killEventFD.revents & POLLERR) != 0)
        {
            TP_ERROR(TRAP::Log::FileWatcherLinuxPrefix, "Polling error");
            return false;
        }

        return true;
    }

    [[nodiscard]] bool IsKillEventTriggered(const pollfd& killEventFD)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        if((killEventFD.revents & POLLIN) == 0)
            return false;

        u64 value = 0;
        if(const ssize_t len = read(killEventFD.fd, &value, sizeof(value)); len < 0)
        {
            TP_ERROR(TRAP::Log::FileWatcherLinuxPrefix, "Failed to read kill event (", TRAP::Utils::String::GetStrError(), ")");
            return true; //Treat this as kill was triggered
        }

        return value == KILLTHREAD;
    }

    [[nodiscard]] bool UpdateEventBufferSize(const FileDescriptor& inotifyFD, std::vector<char>& eventBuffer)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        u64 bytesAvailable = 0;
        if(ioctl(inotifyFD.Get(), FIONREAD, &bytesAvailable) < 0)
        {
            TP_ERROR(TRAP::Log::FileWatcherLinuxPrefix, "Failed to read number inotify events (", TRAP::Utils::String::GetStrError(), ")");
            return false;
        }

        if(eventBuffer.size() < bytesAvailable)
            eventBuffer.resize(bytesAvailable);

        return true;
    }

    [[nodiscard]] bool ReadInotifyEvents(const FileDescriptor& inotifyFD, std::vector<char>& eventData, isize& readBytes)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        readBytes = read(inotifyFD.Get(), eventData.data(), eventData.size());
        if(readBytes < 0)
        {
            TP_ERROR(TRAP::Log::FileWatcherLinuxPrefix, "Failed to read inotify events (", TRAP::Utils::String::GetStrError(), ")");
            return false;
        }

        return true;
    }

    void CreateFileSystemMoveEvent(const std::filesystem::path& from, const std::filesystem::path& to,
                                   std::vector<TRAP::Events::FileSystemChangeEvent>& events)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        using enum TRAP::FileSystem::FileSystemStatus;

        if(from.empty() || to.empty())
            return;

        if(to.parent_path() == from.parent_path())
            events.emplace_back(Renamed, to, from);
        else
        {
            events.emplace_back(Erased, from);
            events.emplace_back(Created, to);
        }
    }

    void CreateLeftoverFileSystemMoveEvents(const std::unordered_map<u32, std::pair<std::filesystem::path, std::filesystem::path>>& movedPaths,
                                            std::vector<TRAP::Events::FileSystemChangeEvent>& events)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        //Iterate movedPaths elements which only has either movedFrom or movedTo set but not both.
        for (const auto& [movedFrom, movedTo] : movedPaths | std::views::transform([](const auto& entry){return entry.second;})
                                                           | std::views::filter([](const auto& paths){return (paths.first.empty() && !paths.second.empty()) ||
                                                                                                             (!paths.first.empty() && paths.second.empty());}))
        {
            if(movedTo.empty()) //Erase
                events.emplace_back(TRAP::FileSystem::FileSystemStatus::Erased, movedFrom);
            else if(movedFrom.empty()) //Created
                events.emplace_back(TRAP::FileSystem::FileSystemStatus::Created, movedTo);
        }
    }

    void ProcessInotifyEvents(const isize readBytes, const std::vector<char>& eventDataBuffer,
                              std::unordered_map<i32, std::filesystem::path>& watchDescriptors,
                              const FileDescriptor& inotifyFD, const bool recursive,
                              std::vector<TRAP::Events::FileSystemChangeEvent>& events)
    {
	    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        //Key = Cookie, Value = {Moved from, Moved to}
        std::unordered_map<u32, std::pair<std::filesystem::path, std::filesystem::path>> movedPaths{};

        usize offset = 0;
        while(std::cmp_less(offset, readBytes)) //Process events
        {
            const inotify_event* const event = reinterpret_cast<const inotify_event*>(eventDataBuffer.data() + offset); //Must use reinterpret_cast because of flexible array member
            if(event->len == 0u)
            {
                offset += sizeof(inotify_event) + event->len;
                continue;
            }

            const std::string_view fileName(event->name);
            const std::filesystem::path filePath = watchDescriptors.at(event->wd) / std::filesystem::path(fileName);
            TRAP_ASSERT(!filePath.empty());

            if((event->mask & IN_DELETE) != 0u)
                events.emplace_back(TRAP::FileSystem::FileSystemStatus::Erased, filePath);

            if((event->mask & IN_CREATE) != 0u)
            {
                if(TRAP::FileSystem::IsFolder(filePath) && recursive) //Add to tracking list
                    CreateInotifyWatch(filePath, inotifyFD, watchDescriptors, false);

                events.emplace_back(TRAP::FileSystem::FileSystemStatus::Created, filePath);
            }

            if((event->mask & IN_MODIFY) != 0u)
                events.emplace_back(TRAP::FileSystem::FileSystemStatus::Modified, filePath);

            if((event->mask & IN_MOVED_FROM) != 0u)
            {
                auto& [movedFrom, movedTo] = movedPaths[event->cookie];
                movedFrom = filePath;

                CreateFileSystemMoveEvent(movedFrom, movedTo, events);
            }

            if((event->mask & IN_MOVED_TO) != 0u)
            {
                auto& [movedFrom, movedTo] = movedPaths[event->cookie];
                movedTo = filePath;

                CreateFileSystemMoveEvent(movedFrom, movedTo, events);
            }

            offset += sizeof(inotify_event) + event->len;
        }

        //Create events for the case where only movedTo or movedFrom path is known.
        CreateLeftoverFileSystemMoveEvents(movedPaths, events);
    }
}

void TRAP::FileSystem::FileSystemWatcher::Watch(const std::stop_token& stopToken, const std::vector<std::filesystem::path>& pathsToWatch,
                                                TRAP::Optional<std::promise<void>> optPromise)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    //Thread init
    const std::stop_callback stopCallback(stopToken, [this] { StopCallback(); });
    TRAP::Utils::SetThreadName(m_debugName.empty() ? "FileSystemWatcher" : fmt::format("{} (FileSystemWatcher)", m_debugName));

    auto fileDescriptors = CreateFileDescriptors(m_killEvent.Get());
    if(!fileDescriptors)
    {
        if(optPromise)
            optPromise->set_value();
        return;
    }
    const FileDescriptor inotifyFD = TRAP::MakeUniqueResourceChecked(std::get<0>(*fileDescriptors).fd, InvalidFileDescriptor, FileDescriptorDeleter);

    //Key = (raw) watch descriptor; Value = Watched path + RAII wrapper for the watch descriptor key
    std::unordered_map<i32, std::filesystem::path> watchDescriptors;

    CreateInotifyWatches(pathsToWatch, inotifyFD, watchDescriptors, m_recursive);

    if(optPromise)
        optPromise->set_value();

    std::vector<char> buf{}; //Can't be a inotify_event array because it includes a flexible array member.

    //Thread work loop
    std::vector<Events::FileSystemChangeEvent> events{};
    while(!stopToken.stop_requested())
    {
        if(!PollFileDescriptors(*fileDescriptors))
            return;

        if(IsKillEventTriggered(std::get<1>(*fileDescriptors)))
            break;

        if(!UpdateEventBufferSize(inotifyFD, buf))
            return;

        isize readBytes = 0;
        if(!ReadInotifyEvents(inotifyFD, buf, readBytes))
            return;

        if(readBytes == 0) //EOF
            continue;

        ProcessInotifyEvents(readBytes, buf, watchDescriptors, inotifyFD, m_recursive, events);

        std::ranges::fill(buf, '\0');

        if(!events.empty())
        {
            DeduplicateEvents(events);
            DispatchEvents(events, *m_callback.ReadLock());
        }
    }
}
#endif
