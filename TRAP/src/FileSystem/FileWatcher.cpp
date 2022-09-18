#include "TRAPPCH.h"
#include "FileWatcher.h"

#include "FileSystem.h"
#include "Events/FileEvent.h"
#include "Utils/Utils.h"

TRAP::FileSystem::FileWatcher::FileWatcher(const std::vector<std::filesystem::path>& paths, const bool recursive)
    : m_recursive(recursive), m_run(true)
{
    if(paths.empty())
        return;

    AddFolders(paths);
    Init();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileSystem::FileWatcher::FileWatcher(const std::filesystem::path& path, const bool recursive)
    : m_recursive(recursive), m_run(true)
{
    if(path.empty())
        return;

    AddFolder(path);
    Init();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileSystem::FileWatcher::~FileWatcher()
{
    Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::FileWatcher::SetEventCallback(const EventCallbackFn& callback)
{
    m_callback = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileSystem::FileWatcher::EventCallbackFn TRAP::FileSystem::FileWatcher::GetEventCallback() const
{
    return m_callback;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::FileWatcher::AddFolder(const std::filesystem::path& path)
{
    if(path.empty())
        return;

    //Always use absolute paths
    const auto absPath = FileSystem::ToAbsolutePath(path);
    if(!absPath)
        return;

    //Only add if not already in list
    if(std::find(m_paths.begin(), m_paths.end(), *absPath) == m_paths.end())
    {
        Shutdown();
        m_paths.push_back(*absPath);
        Init();
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::FileWatcher::AddFolders(const std::vector<std::filesystem::path>& paths)
{
    if(paths.empty())
        return;

    Shutdown();

    for (const auto& path : paths)
    {
        //Always use absolute paths
        const auto absPath = FileSystem::ToAbsolutePath(path);
        if(!absPath) //Skip invalid paths
            continue;

        //Only add if not already in list
        if(std::find(m_paths.begin(), m_paths.end(), *absPath) == m_paths.end())
            m_paths.push_back(*absPath);
    }

    Init();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::FileWatcher::RemoveFolder(const std::filesystem::path& path)
{
    //Always use absolute paths
    const auto absPath = FileSystem::ToAbsolutePath(path);
    if(!absPath)
        return;

    //Check if folder is in the list
    if(std::find(m_paths.begin(), m_paths.end(), *absPath) != m_paths.end())
    {
        Shutdown();
        m_paths.erase(std::remove(m_paths.begin(), m_paths.end(), (*absPath)),
                        m_paths.end());
        Init();
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::FileWatcher::RemoveFolders(const std::vector<std::filesystem::path>& paths)
{
    if(paths.empty())
        return;

    Shutdown();

    for(const auto& path : paths)
    {
        //Always use absolute paths
        const auto& absPath = FileSystem::ToAbsolutePath(path);
        if(!absPath) //Skip empty path
            continue;

        m_paths.erase(std::remove(m_paths.begin(), m_paths.end(), (*absPath).u8string()),
                      m_paths.end());
    }

    Init();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::filesystem::path> TRAP::FileSystem::FileWatcher::GetFolders() const
{
    return m_paths;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::FileWatcher::Init()
{
    if(m_paths.empty())
        return;

    m_run = true;

#ifdef TRAP_PLATFORM_WINDOWS
    m_killEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
#elif defined(TRAP_PLATFORM_LINUX)
    m_killEvent = eventfd(0, 0);
#endif
    m_thread = std::thread(&TRAP::FileSystem::FileWatcher::Watch, this);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::FileWatcher::Shutdown()
{
    if(!m_run)
        return;

    m_run = false;

#ifdef TRAP_PLATFORM_WINDOWS
    SetEvent(m_killEvent);
#elif defined(TRAP_PLATFORM_LINUX)
    const uint64_t value = 1;
    ssize_t toSend = sizeof(value);
    do
    {
        const ssize_t res = write(m_killEvent, &value, sizeof(value));
        if(res == -1)
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Error writing to eventfd");
            m_thread.detach();
            return;
        }
        toSend -= res;
    } while (toSend > 0);
#endif

    if (m_thread.joinable())
        m_thread.join();
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_WINDOWS
void TRAP::FileSystem::FileWatcher::Watch()
{
    //Thread init
    std::vector<Events::FileChangeEvent> events;

    std::vector<uintptr_t> dirHandles{}; //Don't use void* as type for vector.
    dirHandles.reserve(m_paths.size());
    for(const auto& path : m_paths)
    {
        const HANDLE handle = CreateFile(path.generic_wstring().c_str(), GENERIC_READ | FILE_LIST_DIRECTORY,
                                         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
                                         FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);

        if (handle == INVALID_HANDLE_VALUE)
        {
            TP_ERROR(Log::FileWatcherWindowsPrefix, "Failed to open directory: ", path);
            continue;
        }

        dirHandles.push_back(reinterpret_cast<uintptr_t>(handle));
    }

    OVERLAPPED pollingOverlap;
    pollingOverlap.OffsetHigh = 0;
    pollingOverlap.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    std::vector<std::array<char, 2048>> bufs(dirHandles.size());
    std::vector<DWORD> bytesReturned(dirHandles.size());

    //Thread work loop
    while(m_run)
    {
        for(std::size_t i = 0; i < bufs.size(); ++i)
        {
            const BOOL result = ReadDirectoryChangesW(reinterpret_cast<HANDLE>(dirHandles[i]), bufs[i].data(), static_cast<DWORD>(bufs[i].size()),
                                                      m_recursive, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                                                      FILE_NOTIFY_CHANGE_SIZE, &bytesReturned[i], &pollingOverlap,
                                                      nullptr);
            if(!result)
            {
                TP_ERROR(Log::FileWatcherWindowsPrefix, "Failed to read directory changes: ", GetLastError());
                for(uintptr_t& handle : dirHandles)
                    CloseHandle(reinterpret_cast<HANDLE>(handle));
                return;
            }
        }

        const std::array<HANDLE, 2> handles = {pollingOverlap.hEvent, m_killEvent};
        const DWORD res = WaitForMultipleObjects(static_cast<DWORD>(handles.size()), handles.data(), FALSE, INFINITE);

        if (res == WAIT_OBJECT_0 + 1)
            break;

        std::wstring oldName;

        for(std::size_t i = 0; i < bufs.size(); ++i)
        {
            uint32_t offset = 0;
            const FILE_NOTIFY_INFORMATION* notify;

            do
            {
                notify = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(bufs[i].data() + offset);
                if (notify->FileNameLength == 0)
                {
                    offset += notify->NextEntryOffset;
                    continue;
                }
                const std::size_t filenameLength = notify->FileNameLength / sizeof(wchar_t);

                const std::filesystem::path filePath = (m_paths[i] / std::filesystem::path(std::wstring(notify->FileName, filenameLength)));
                FileStatus status;
                std::filesystem::path oldFileName = "";

                switch(notify->Action)
                {
                case FILE_ACTION_ADDED:
                    status = FileStatus::Created;
                    break;

                case FILE_ACTION_REMOVED:
                    status = FileStatus::Erased;
                    break;

                case FILE_ACTION_MODIFIED:
                    status = FileStatus::Modified;
                    break;

                case FILE_ACTION_RENAMED_OLD_NAME:
                    oldName = filePath.filename();
                    break;

                case FILE_ACTION_RENAMED_NEW_NAME:
                    status = FileStatus::Renamed;
                    oldFileName = oldName;
                    break;

                default:
                    break;
                }

                if(notify->Action != FILE_ACTION_RENAMED_OLD_NAME)
                    events.emplace_back(status, filePath, oldFileName);

                offset += notify->NextEntryOffset;
            } while(notify->NextEntryOffset);

            //Clear buffer
            std::fill(bufs[i].begin(), bufs[i].end(), '\0');
        }

        if(!events.empty())
        {
            if(m_callback)
            {
                for(Events::FileChangeEvent& e : events)
                    m_callback(e);
            }
            events.clear();
        }
    }

    //Thread cleanup
    for(uintptr_t& handle : dirHandles)
        CloseHandle(reinterpret_cast<HANDLE>(handle));
}

//-------------------------------------------------------------------------------------------------------------------//

#elif defined(TRAP_PLATFORM_LINUX)
void TRAP::FileSystem::FileWatcher::Watch()
{
    //Thread init
    std::vector<Events::FileChangeEvent> events;

    std::array<pollfd, 2> fileDescriptors{};
    fileDescriptors[1].events = POLLIN;
    fileDescriptors[1].fd = m_killEvent;

    fileDescriptors[0].events = POLLIN;
    fileDescriptors[0].fd = inotify_init();
    if(fileDescriptors[0].fd < 0)
    {
        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to initialize inotify: ", Utils::String::GetStrError());
        close(m_killEvent);
        return;
    }

    //Storing watch descriptor and the according path
    std::unordered_map<int32_t, std::filesystem::path> watchDescriptors;

    //Lambda function for shutting down when an error has occurred
    const auto ErrorShutdown = [this, watchDescriptors, fileDescriptors]()
    {
        for(auto& [wDesc, path] : watchDescriptors)
            inotify_rm_watch(fileDescriptors[0].fd, wDesc);
        close(fileDescriptors[0].fd);
        close(m_killEvent);
    };

    for(const auto& path : m_paths)
    {
        const int32_t wd = inotify_add_watch(fileDescriptors[0].fd, path.u8string().c_str(),
                                             IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);

        if(wd < 0)
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to add watch: ", Utils::String::GetStrError());
            continue; //Skip failed entry
        }

        watchDescriptors[wd] = path;

        //Also add sub dirs if recursive
        if(m_recursive)
        {
            std::error_code ec; //Using the non-throwing version of recursive_directory_iterator
            const std::filesystem::recursive_directory_iterator it(path, ec);
            if(ec) //Skip adding sub dirs on failure
                continue;

            for(const auto& p : it)
            {
                const int32_t wd = inotify_add_watch(fileDescriptors[0].fd, p.path().u8string().c_str(),
                                                    IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);

                if(wd < 0)
                {
                    TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to add watch: ", Utils::String::GetStrError());
                    continue; //Skip failed entry
                }

                watchDescriptors[wd] = p.path();
            }
        }
    }

    std::array<char, 2048> buf{}; //Can't be a inotify_event array because it includes a flexible array member.
    std::filesystem::path oldName;

    //Thread work loop
    while(m_run)
    {
        const int32_t ready = poll(fileDescriptors.data(), fileDescriptors.size(), -1);
        if(ready < 0)
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to poll events: ", Utils::String::GetStrError());
            ErrorShutdown();
            return;
        }

        if((fileDescriptors[0].revents != 0 && (fileDescriptors[0].revents & POLLHUP || fileDescriptors[0].revents & POLLERR)) ||
           (fileDescriptors[1].revents != 0 && (fileDescriptors[1].revents & POLLHUP || fileDescriptors[1].revents & POLLERR)))
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Polling error");
            ErrorShutdown();
            return;
        }
        if(fileDescriptors[1].revents & POLLIN)
        {
            uint64_t value = 0;
            const ssize_t len = read(fileDescriptors[1].fd, &value, sizeof(value));
            if(len < 0)
            {
                TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to read kill event: ", Utils::String::GetStrError());
                ErrorShutdown();
                return;
            }

            if(value == 1)
            {
                ErrorShutdown();
                return;
            }
        }

        const ssize_t len = read(fileDescriptors[0].fd, buf.data(), buf.size());
        if(len == 0)
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to read inotify events: ", Utils::String::GetStrError());
            ErrorShutdown();
            return;
        }
        if(len < 0)
        {
            if(errno == EAGAIN)
            {
                std::this_thread::sleep_for(std::chrono::seconds(2));
                continue;
            }

            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to read inotify events: ", Utils::String::GetStrError());
            ErrorShutdown();
            return;
        }

        std::size_t offset = 0;
        while(offset < static_cast<std::size_t>(len)) //Process events
        {
            const inotify_event* const event = reinterpret_cast<const inotify_event*>(buf.data() + offset); //Must use reinterpret_cast because of flexible array member
            if(!event->len)
            {
                offset += sizeof(inotify_event) + event->len;
                continue;
            }

            FileStatus status = FileStatus::Created;
            const std::filesystem::path filePath = watchDescriptors[event->wd] / std::filesystem::path(event->name);
            const bool isDir = std::filesystem::is_directory(filePath);
            std::filesystem::path oldFileName = "";

            if(event->mask & IN_CREATE)
            {
                if(isDir && m_recursive) //Add to tracking list
                {
                    const int32_t wd = inotify_add_watch(fileDescriptors[0].fd, filePath.u8string().c_str(),
                                                            IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);

                    if(wd < 0)
                    {
                        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to add watch: ", Utils::String::GetStrError());
                        ErrorShutdown();
                        return;
                    }

                    watchDescriptors[wd] = filePath;
                }

                status = FileStatus::Created;
            }
            else if(event->mask & IN_DELETE)
            {
                if(isDir && m_recursive) //Remove from tracking list
                {
                    inotify_rm_watch(fileDescriptors[0].fd, event->wd);
                    watchDescriptors.erase(event->wd);
                }

                status = FileStatus::Erased;
            }
            else if(event->mask & IN_MODIFY)
                status = FileStatus::Modified;
            else if(event->mask & IN_MOVED_FROM)
            {
                oldName = filePath;
                status = FileStatus::Renamed;
            }
            else if(event->mask & IN_MOVED_TO) //BUG Some distros (like Ubuntu) seem to not report IN_MOVED_TO on deletion
            {
                if(filePath.parent_path() == oldName.parent_path())
                {
                    status = FileStatus::Renamed;
                    oldFileName = oldName.filename();
                }
                else
                {
                    //Erase event
                    events.emplace_back(FileStatus::Erased, oldName);

                    status = FileStatus::Created;
                }
            }

            if(!(event->mask & IN_MOVED_FROM))
                events.emplace_back(status, filePath, oldFileName);

            offset += sizeof(inotify_event) + event->len;
        }

        std::fill(buf.begin(), buf.end(), 0);

        if(!events.empty())
        {
            if(m_callback)
            {
                for(Events::FileChangeEvent& e : events)
                    m_callback(e);
            }
            events.clear();
        }
    }

    //Thread cleanup
    for(auto& [wd, path] : watchDescriptors)
    {
        if(inotify_rm_watch(fileDescriptors[0].fd, wd) < 0)
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to remove inotify watch: ", Utils::String::GetStrError());
    }

    if(close(fileDescriptors[0].fd) < 0)
        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to close inotify: ", Utils::String::GetStrError());

    if(close(m_killEvent) < 0)
        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to close eventfd: ", Utils::String::GetStrError());
}
#endif
