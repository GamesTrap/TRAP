#include "TRAPPCH.h"
#include "FileWatcher.h"

#include "FS.h"
#include "Events/FileEvent.h"
#include "Utils/Utils.h"

TRAP::FS::FileWatcher::FileWatcher(const std::vector<std::filesystem::path>& paths, const bool recursive)
    : m_recursive(recursive), m_run(true), m_skipNextFileChange(false)
{
    if(paths.empty())
        return;

    AddFolders(paths);
    Init();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FS::FileWatcher::FileWatcher(const std::filesystem::path& path, const bool recursive)
    : m_recursive(recursive), m_run(true), m_skipNextFileChange(false)
{
    if(path.empty())
        return;

    AddFolder(path);
    Init();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FS::FileWatcher::~FileWatcher()
{
    Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FS::FileWatcher::SkipNextFileChange()
{
    m_skipNextFileChange = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FS::FileWatcher::SetEventCallback(const EventCallbackFn& callback)
{
    m_callback = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FS::FileWatcher::EventCallbackFn TRAP::FS::FileWatcher::GetEventCallback()
{
    return m_callback;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FS::FileWatcher::AddFolder(std::filesystem::path path)
{
    if(path.empty())
        return;

    // AddFolders({path});
    Shutdown();
    //Always use absolute paths
    path = FS::ToAbsolutePath(path);
    if(!path.empty())
    {
        if(std::find(m_paths.begin(), m_paths.end(), path) == m_paths.end())
            m_paths.emplace_back(path);
    }

    if(!m_paths.empty())
        Init();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FS::FileWatcher::AddFolders(const std::vector<std::filesystem::path>& paths)
{
    if(paths.empty())
        return;

    Shutdown();
    for (auto path : paths)
    {
        //Always use absolute paths
        path = FS::ToAbsolutePath(path);
        if(path.empty()) //Skip
            continue;

        if(std::find(m_paths.begin(), m_paths.end(), path) == m_paths.end())
            m_paths.emplace_back(path);
    }
    if(!m_paths.empty())
        Init();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FS::FileWatcher::RemoveFolder(std::filesystem::path path)
{
    if(path.empty())
        return;

    Shutdown();
    //Always use absolute paths
    path = FS::ToAbsolutePath(path);
    if(!path.empty())
        m_paths.erase(std::remove(m_paths.begin(), m_paths.end(), path.generic_u8string()), m_paths.end());
    if(!m_paths.empty())
        Init();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FS::FileWatcher::RemoveFolders(const std::vector<std::filesystem::path>& paths)
{
    if(paths.empty())
        return;

    Shutdown();
    for(auto path : paths)
    {
        //Always use absolute paths
        path = FS::ToAbsolutePath(path);
        if(path.empty()) //Skip
            continue;

        m_paths.erase(std::remove(m_paths.begin(), m_paths.end(), path.generic_u8string()), m_paths.end());
    }
    if(!m_paths.empty())
        Init();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::filesystem::path> TRAP::FS::FileWatcher::GetFolders() const
{
    return m_paths;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FS::FileWatcher::Init()
{
    if(m_paths.empty())
        return;

    m_run = true;

#ifdef TRAP_PLATFORM_WINDOWS
    m_killEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    m_thread = std::thread(&TRAP::FS::FileWatcher::WatchWindows, this);
#elif defined(TRAP_PLATFORM_LINUX)
    m_killEvent = eventfd(0, 0);
    m_thread = std::thread(&TRAP::FS::FileWatcher::WatchLinux, this);
#endif

}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FS::FileWatcher::Shutdown()
{
    if(!m_run)
        return;

    m_run = false;

#ifdef TRAP_PLATFORM_WINDOWS
    SetEvent(m_killEvent);
#elif defined(TRAP_PLATFORM_LINUX)
    uint64_t value = 1;
    ssize_t toSend = sizeof(value);
    do
    {
        ssize_t res = write(m_killEvent, &value, sizeof(value));
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

void TRAP::FS::FileWatcher::WatchWindows()
{
#ifdef TRAP_PLATFORM_WINDOWS
    std::vector<Events::FileChangeEvent> events;
    events.reserve(10);

    std::vector<HANDLE> dirHandles(m_paths.size());
    for(std::size_t i = 0; i < dirHandles.size(); ++i)
    {
        dirHandles[i] = CreateFile(m_paths[i].generic_wstring().c_str(), GENERIC_READ | FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);

        if (dirHandles[i] == INVALID_HANDLE_VALUE)
        {
            std::cout << "Failed to open directory : " << m_paths[i] << '\n';
            for(HANDLE& handle : dirHandles)
            {
                if(handle)
                    CloseHandle(handle);
            }
            return;
        }
    }

    OVERLAPPED pollingOverlap;
    pollingOverlap.OffsetHigh = 0;
    pollingOverlap.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    std::vector<std::array<char, 2048>> bufs;
    bufs.resize(dirHandles.size());
    std::vector<DWORD> bytesReturned(dirHandles.size());
    while(m_run)
    {
        for(std::size_t i = 0; i < bufs.size(); ++i)
        {
            const BOOL result = ReadDirectoryChangesW(dirHandles[i], bufs[i].data(), static_cast<DWORD>(bufs[i].size()),
                                                      m_recursive, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                                                      FILE_NOTIFY_CHANGE_SIZE, &bytesReturned[i], &pollingOverlap,
                                                      nullptr);
            if(!result)
            {
                TP_ERROR(Log::FileWatcherWindowsPrefix, "Failed to read directory changes: ", GetLastError());
                for(HANDLE& handle : dirHandles)
                    CloseHandle(handle);
                return;
            }
        }

        std::array<HANDLE, 2> handles = {pollingOverlap.hEvent, m_killEvent};
        const DWORD res = WaitForMultipleObjects(static_cast<DWORD>(handles.size()), handles.data(), FALSE, INFINITE);

        if (res == WAIT_OBJECT_0 + 1)
            break;

        if(m_skipNextFileChange)
        {
            m_skipNextFileChange = false;
            continue;
        }

        std::wstring oldName;

        for(std::size_t i = 0; i < bufs.size(); ++i)
        {
            uint32_t offset = 0;
            FILE_NOTIFY_INFORMATION* notify;

            do
            {
                notify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(bufs[i].data() + offset);
                if (notify->FileNameLength == 0)
                    continue;
               const std::size_t filenameLength = notify->FileNameLength / sizeof(wchar_t);

                std::filesystem::path filePath = (m_paths[i] / std::filesystem::path(std::wstring(notify->FileName, filenameLength))).generic_u8string();
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
            std::memset(bufs[i].data(), 0, bufs[i].size());
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

    for(HANDLE& handle : dirHandles)
        CloseHandle(handle);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FS::FileWatcher::WatchLinux()
{
#ifdef TRAP_PLATFORM_LINUX
    std::vector<Events::FileChangeEvent> events;
    events.reserve(10);

    std::array<pollfd, 2> fileDescriptors{};
    fileDescriptors[1].events = POLLIN;
    fileDescriptors[1].fd = m_killEvent;

    fileDescriptors[0].events = POLLIN;
    fileDescriptors[0].fd = inotify_init();
    if(fileDescriptors[0].fd < 0)
    {
        std::string error(1024, '\0');
        const char* unused = strerror_r(errno, error.data(), error.size());
        static_cast<void>(unused); //Avoid unused variable warning

        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to initialize inotify: ", Utils::GetStrError());
        close(m_killEvent);
        return;
    }

    std::unordered_map<int32_t, std::filesystem::path> watchDescriptors;

    for(const auto& path : m_paths)
    {
        const int32_t wd = inotify_add_watch(fileDescriptors[0].fd, path.generic_u8string().c_str(),
                                             IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);

        if(wd < 0)
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to add watch: ", Utils::GetStrError());
            for(auto& [wDesc, path] : watchDescriptors)
                inotify_rm_watch(fileDescriptors[0].fd, wDesc);
            close(fileDescriptors[0].fd);
            close(m_killEvent);
            return;
        }

        watchDescriptors[wd] = path;

        //Also add sub dirs if recursive
        if(m_recursive)
        {
            std::error_code ec;
            std::filesystem::recursive_directory_iterator it(path, ec);
            if(!ec)
            {
                for(const auto& p : it)
                {
                    const int32_t wd = inotify_add_watch(fileDescriptors[0].fd, p.path().generic_u8string().c_str(),
                                                        IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);

                    if(wd < 0)
                    {
                        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to add watch: ", Utils::GetStrError());
                        for(auto& [wDesc, path] : watchDescriptors)
                            inotify_rm_watch(fileDescriptors[0].fd, wDesc);
                        close(fileDescriptors[0].fd);
                        close(m_killEvent);
                        return;
                    }

                    watchDescriptors[wd] = p.path().generic_u8string();
                }
            }
        }
    }

    std::array<char, 2048> buf{};
    std::filesystem::path oldName;

    while(m_run)
    {
        int32_t ready = poll(fileDescriptors.data(), fileDescriptors.size(), -1);
        if(ready < 0)
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to poll events: ", Utils::GetStrError());
            for(auto& [wDesc, path] : watchDescriptors)
                inotify_rm_watch(fileDescriptors[0].fd, wDesc);
            close(fileDescriptors[0].fd);
            close(m_killEvent);
            return;
        }

        if(fileDescriptors[0].revents != 0 && (fileDescriptors[0].revents & POLLHUP || fileDescriptors[0].revents & POLLERR))
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Polling error");
            for(auto& [wDesc, path] : watchDescriptors)
                inotify_rm_watch(fileDescriptors[0].fd, wDesc);
            close(fileDescriptors[0].fd);
            close(m_killEvent);
            return;
        }
        if(fileDescriptors[1].revents != 0 && (fileDescriptors[1].revents & POLLHUP || fileDescriptors[1].revents & POLLERR))
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Polling error");
            for(auto& [wDesc, path] : watchDescriptors)
                inotify_rm_watch(fileDescriptors[0].fd, wDesc);
            close(fileDescriptors[0].fd);
            close(m_killEvent);
            return;
        }
        if(fileDescriptors[1].revents & POLLIN)
        {
            uint64_t value = 0;
            const ssize_t len = read(fileDescriptors[1].fd, &value, sizeof(value));
            if(len < 0)
            {
                TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to read kill event: ", Utils::GetStrError());
                for(auto& [wDesc, path] : watchDescriptors)
                    inotify_rm_watch(fileDescriptors[0].fd, wDesc);
                close(fileDescriptors[0].fd);
                close(m_killEvent);
                return;
            }

            if(value == 1)
            {
                for(auto& [wDesc, path] : watchDescriptors)
                    inotify_rm_watch(fileDescriptors[0].fd, wDesc);
                close(fileDescriptors[0].fd);
                close(m_killEvent);
                return;
            }
        }

        const ssize_t len = read(fileDescriptors[0].fd, buf.data(), buf.size());
        if(len == 0)
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to read inotify events: ", Utils::GetStrError());
            for(auto& [wDesc, path] : watchDescriptors)
                inotify_rm_watch(fileDescriptors[0].fd, wDesc);
            close(fileDescriptors[0].fd);
            close(m_killEvent);
            return;
        }
        if(len < 0)
        {
            if(errno == EAGAIN)
            {
                std::this_thread::sleep_for(std::chrono::seconds(2));
                continue;
            }

            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to read inotify events: ", Utils::GetStrError());
            for(auto& [wDesc, path] : watchDescriptors)
                inotify_rm_watch(fileDescriptors[0].fd, wDesc);
            close(fileDescriptors[0].fd);
            close(m_killEvent);
            return;
        }

        if(m_skipNextFileChange)
        {
            m_skipNextFileChange = false;
            continue;
        }

        std::size_t offset = 0;
        while(offset < static_cast<std::size_t>(len))
        {
            const inotify_event* event = reinterpret_cast<const inotify_event*>(buf.data() + offset);
            if(event->len)
            {
                FileStatus status = FileStatus::Created;
                std::filesystem::path filePath = watchDescriptors[event->wd] / std::filesystem::path(event->name).generic_u8string();
                bool isDir = std::filesystem::is_directory(filePath);
                std::filesystem::path oldFileName = "";

                if(event->mask & IN_CREATE)
                {
                    if(isDir && m_recursive) //Add to tracking list
                    {
                        const int32_t wd = inotify_add_watch(fileDescriptors[0].fd, filePath.generic_u8string().c_str(),
                                                             IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);

                        if(wd < 0)
                        {
                            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to add watch: ", Utils::GetStrError());
                            for(auto& [wDesc, path] : watchDescriptors)
                                inotify_rm_watch(fileDescriptors[0].fd, wDesc);
                            close(fileDescriptors[0].fd);
                            close(m_killEvent);
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
            }

            offset += sizeof(inotify_event) + event->len;
        }

        std::memset(buf.data(), 0, buf.size());

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

    for(auto& [wd, path] : watchDescriptors)
    {
        int32_t res = inotify_rm_watch(fileDescriptors[0].fd, wd);
        if(res < 0)
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to remove inotify watch: ", Utils::GetStrError());
            continue;
        }
    }

    int32_t res = close(fileDescriptors[0].fd);
    if(res < 0)
        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to close inotify: ", Utils::GetStrError());

    res = close(m_killEvent);
    if(res < 0)
        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to close eventfd: ", Utils::GetStrError());
#endif
}
