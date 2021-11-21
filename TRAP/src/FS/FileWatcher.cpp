#include "TRAPPCH.h"
#include "FileWatcher.h"

#include "Events/FileEvent.h"

TRAP::FileWatcher::FileWatcher(const std::vector<std::filesystem::path>& paths, const bool recursive)
    : m_recursive(recursive), m_run(true), m_skipNextFileChange(false)
{
    AddFolders(paths);
    Init();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileWatcher::FileWatcher(const std::filesystem::path& path, const bool recursive)
    : m_recursive(recursive), m_run(true), m_skipNextFileChange(false)
{
    AddFolder(path);
    Init();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileWatcher::~FileWatcher()
{
    Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileWatcher::SkipNextFileChange()
{
    m_skipNextFileChange = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileWatcher::SetEventCallback(const EventCallbackFn& callback)
{
    m_callback = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileWatcher::EventCallbackFn TRAP::FileWatcher::GetEventCallback()
{
    return m_callback;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileWatcher::AddFolder(const std::filesystem::path& path)
{
    AddFolders({path});
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileWatcher::AddFolders(const std::vector<std::filesystem::path>& paths)
{
    for (const auto& path : paths)
    {
        if(std::find(m_paths.begin(), m_paths.end(), path) == m_paths.end())
            m_paths.emplace_back(path);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileWatcher::RemoveFolder(const std::filesystem::path& path)
{
    RemoveFolders({path});
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileWatcher::RemoveFolders(const std::vector<std::filesystem::path>& paths)
{
    Shutdown();
    for(const auto& path : paths)
        m_paths.erase(std::remove(m_paths.begin(), m_paths.end(), path.generic_u8string()), m_paths.end());
    Init();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::filesystem::path> TRAP::FileWatcher::GetFolders() const
{
    return m_paths;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileWatcher::Init()
{
#ifdef TRAP_PLATFORM_WINDOWS
    m_killEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
#endif

    m_thread = std::thread(&TRAP::FileWatcher::Watch, this);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileWatcher::Shutdown()
{
    m_run = false;

#ifdef TRAP_PLATFORM_WINDOWS
    SetEvent(m_killEvent);
#endif

    if (m_thread.joinable())
        m_thread.join();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileWatcher::Watch()
{
#ifdef TRAP_PLATFORM_WINDOWS
    WatchWindows();
#elif defined(TRAP_PLATFORM_LINUX)
    WatchLinux();
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileWatcher::WatchWindows()
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

void TRAP::FileWatcher::WatchLinux()
{
#ifdef TRAP_PLATFORM_LINUX
    std::vector<Events::FileChangeEvent> events;
    events.reserve(10);

    int32_t fd = inotify_init1(IN_NONBLOCK);
    if(fd < 0)
    {
        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to initialize inotify: ", strerror(errno));
        return;
    }

    std::unordered_map<int32_t, std::filesystem::path> watchDescriptors;

    for(auto it = m_paths.begin(); it != m_paths.end(); ++it)
    {
        const int32_t wd = inotify_add_watch(fd, (*it).generic_u8string().c_str(),
                                             IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);

        if(wd < 0)
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to add watch: ", strerror(errno));
            return;
        }

        watchDescriptors[wd] = *it;

        //Also add sub dirs if recursive
        if(m_recursive)
        {
            for(auto& p : std::filesystem::recursive_directory_iterator(*it))
            {
                const int32_t wd = inotify_add_watch(fd, p.path().generic_u8string().c_str(),
                                                     IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);

                if(wd < 0)
                {
                    TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to add watch: ", strerror(errno));
                    return;
                }

                watchDescriptors[wd] = p.path().generic_u8string();
            }
        }
    }

    std::array<char, 2048> buf;
    std::filesystem::path oldName;

    while(m_run)
    {
        const ssize_t len = read(fd, buf.data(), buf.size()); //TODO To make this event based, we need a way to interrupt the read.
        if(len == 0)
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to read inotify events: ", strerror(errno));
            return;
        }
        else if(len < 0)
        {
            if(errno == EAGAIN)
            {
                std::this_thread::sleep_for(std::chrono::seconds(2));
                continue;
            }

            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to read inotify events: ", strerror(errno));
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
            inotify_event* event = reinterpret_cast<inotify_event*>(buf.data() + offset);
            if(event->len)
            {
                FileStatus status;
                std::filesystem::path filePath = watchDescriptors[event->wd] / std::filesystem::path(std::string(event->name)).generic_u8string();
                bool isDir = std::filesystem::is_directory(filePath);
                std::filesystem::path oldFileName = "";

                if(event->mask & IN_CREATE)
                {
                    if(isDir && m_recursive) //Add to tracking list
                    {
                        const int32_t wd = inotify_add_watch(fd, filePath.generic_u8string().c_str(),
                                                             IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);

                        if(wd < 0)
                        {
                            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to add watch: ", strerror(errno));
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
                        inotify_rm_watch(fd, event->wd);
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
                        events.emplace_back(FileStatus::Erased, oldName, std::filesystem::is_directory(oldName));

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
        int32_t res = inotify_rm_watch(fd, wd);
        if(res < 0)
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to remove inotify watch: ", strerror(errno));
            continue;
        }
    }

    int32_t res = close(fd);
    if(res < 0)
    {
        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to close inotify: ", strerror(errno));
        return;
    }
#endif
}
