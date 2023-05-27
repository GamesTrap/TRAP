#include "TRAPPCH.h"
#include "FileWatcher.h"

#include "FileSystem.h"
#include "Events/FileEvent.h"
#include "Utils/String/String.h"
#include "Utils/Utils.h"

TRAP::FileSystem::FileWatcher::FileWatcher(std::string name, const bool recursive)
    : m_recursive(recursive), m_run(false), m_name(std::move(name))
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!m_name.empty(), "FileWatcher(): Name can not be empty!");

    if(m_name.empty())
    {
        TP_ERROR(Log::FileWatcherPrefix, "Name can not be empty!");
        return;
    }

    Init();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileSystem::FileWatcher::~FileWatcher()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::FileWatcher::SetEventCallback(const EventCallbackFn& callback) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    m_callback = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::FileSystem::FileWatcher::EventCallbackFn TRAP::FileSystem::FileWatcher::GetEventCallback() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    return m_callback;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::FileWatcher::AddFolder(const std::filesystem::path& path)
{
    //TODO Make this work without stopping the watcher thread
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!path.empty(), "FileWatcher::AddFolder(): Path can not be empty!");

    if(path.empty())
    {
        TP_ERROR(Log::FileWatcherPrefix, "AddFolder(): Path can not be empty!");
        return;
    }

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
    //TODO Make this work without stopping the watcher thread
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!paths.empty(), "FileWatcher::AddFolders(): Paths can not be empty!");

    if(paths.empty())
    {
        TP_ERROR(Log::FileWatcherPrefix, "AddFolders(): Paths can not be empty!");
        return;
    }

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
    //TODO Make this work without stopping the watcher thread
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

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
    //TODO Make this work without stopping the watcher thread
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!paths.empty(), "FileWatcher::RemoveFolders(): Paths can not be empty!");

    if(paths.empty())
    {
        TP_ERROR(Log::FileWatcherPrefix, "RemoveFolders(): Paths can not be empty!");
        return;
    }

    Shutdown();

    for(const auto& path : paths)
    {
        //Always use absolute paths
        const auto& absPath = FileSystem::ToAbsolutePath(path);
        if(!absPath) //Skip empty path
            continue;

        m_paths.erase(std::remove(m_paths.begin(), m_paths.end(), *absPath),
                      m_paths.end());
    }

    Init();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<std::filesystem::path> TRAP::FileSystem::FileWatcher::GetFolders() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem) || (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    return m_paths;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::FileWatcher::Init()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    if(m_paths.empty())
        return;

    m_run = true;

#ifdef TRAP_PLATFORM_WINDOWS
    m_killEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if(!m_killEvent)
        TP_ERROR(Log::FileWatcherWindowsPrefix, "Failed to create kill event (", Utils::String::GetStrError(), ")!");
#elif defined(TRAP_PLATFORM_LINUX)
    m_killEvent = eventfd(0, 0);
    if(m_killEvent < 0)
        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to create kill event (", Utils::String::GetStrError(), ")!");
#endif
    m_thread = std::thread(&TRAP::FileSystem::FileWatcher::Watch, this);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::FileWatcher::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    if(!m_run)
        return;

    m_run = false;

#ifdef TRAP_PLATFORM_WINDOWS
    if(!SetEvent(m_killEvent))
    {
        TP_ERROR(Log::FileWatcherWindowsPrefix, "Failed to set kill event (", Utils::String::GetStrError(), ")!");
        return;
    }
#elif defined(TRAP_PLATFORM_LINUX)
    const uint64_t value = 1;
    ssize_t toSend = sizeof(value);
    do
    {
        const ssize_t res = write(m_killEvent, &value, sizeof(value));
        if(res < 0)
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Error writing to eventfd (", Utils::String::GetStrError(), ")");
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
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

#ifdef TRACY_ENABLE
	//Set Thread name for profiler
    if(m_name.empty())
	    tracy::SetThreadName("FileWatcher");
    else
	    tracy::SetThreadName((m_name + " (FileWatcher)").c_str());
#endif /*TRACY_ENABLE*/

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
            TP_ERROR(Log::FileWatcherWindowsPrefix, "Failed to open directory: ", path, " (", Utils::String::GetStrError(), ")");
            continue;
        }

        dirHandles.push_back(reinterpret_cast<uintptr_t>(handle));
    }

    OVERLAPPED pollingOverlap;
    pollingOverlap.OffsetHigh = 0;
    pollingOverlap.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if(!pollingOverlap.hEvent)
    {
        TP_ERROR(Log::FileWatcherWindowsPrefix, "Failed to create polling overlap event (", Utils::String::GetStrError(), ")!");
        m_run = false; //Fatal error, thread wont wait for the event so just let it cleanup
    }

    std::vector<std::array<char, 2048>> bufs(dirHandles.size());
    std::vector<DWORD> bytesReturned(dirHandles.size());

    //Thread work loop
    while(m_run)
    {
        for(std::size_t i = 0; i < bufs.size(); ++i)
        {
            const BOOL result = ReadDirectoryChangesW(reinterpret_cast<HANDLE>(dirHandles[i]), bufs[i].data(), NumericCast<DWORD>(bufs[i].size()),
                                                      m_recursive, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                                                      FILE_NOTIFY_CHANGE_SIZE, &bytesReturned[i], &pollingOverlap,
                                                      nullptr);
            if(!result)
            {
                TP_ERROR(Log::FileWatcherWindowsPrefix, "Failed to read directory changes (", Utils::String::GetStrError(), ")");
                for(uintptr_t& handle : dirHandles)
                {
                    if(!CloseHandle(reinterpret_cast<HANDLE>(handle)))
                        TP_ERROR(Log::FileWatcherWindowsPrefix, "Failed to close directory handle (", Utils::String::GetStrError(), ")");
                }
                return;
            }
        }

        const std::array<HANDLE, 2> handles = {pollingOverlap.hEvent, m_killEvent};
        const DWORD res = WaitForMultipleObjects(NumericCast<DWORD>(handles.size()), handles.data(), FALSE, INFINITE);

        if (res == WAIT_OBJECT_0 + 1) //Exit work loop
            break;
        else if(res == WAIT_FAILED)
            TP_ERROR(Log::FileWatcherWindowsPrefix, "Failed to wait for multiple objects (", Utils::String::GetStrError(), ")");

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
                std::optional<std::filesystem::path> oldFileName = std::nullopt;

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
    {
        if(!CloseHandle(reinterpret_cast<HANDLE>(handle)))
            TP_ERROR(Log::FileWatcherWindowsPrefix, "Failed to close directory handle (", Utils::String::GetStrError(), ")");
    }
}

//-------------------------------------------------------------------------------------------------------------------//

#elif defined(TRAP_PLATFORM_LINUX)
void TRAP::FileSystem::FileWatcher::Watch()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

#ifdef TRACY_ENABLE
	//Set Thread name for profiler
    if(m_name.empty())
	    tracy::SetThreadName("FileWatcher");
    else
	    tracy::SetThreadName((m_name + " (FileWatcher)").c_str());
#endif /*TRACY_ENABLE*/

    //Thread init
    std::vector<Events::FileChangeEvent> events;

    std::array<pollfd, 2> fileDescriptors{};
    std::get<1>(fileDescriptors).events = POLLIN;
    std::get<1>(fileDescriptors).fd = m_killEvent;

    std::get<0>(fileDescriptors).events = POLLIN;
    std::get<0>(fileDescriptors).fd = inotify_init();
    if(std::get<0>(fileDescriptors).fd < 0)
    {
        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to initialize inotify (", Utils::String::GetStrError(), ")");
        if(close(m_killEvent) < 0)
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to close kill event (", Utils::String::GetStrError(), ")");
        return;
    }

    //Storing watch descriptor and the according path
    std::unordered_map<int32_t, std::filesystem::path> watchDescriptors;

    //Lambda function for shutting down when an error has occurred
    const auto ErrorShutdown = [this, watchDescriptors, fileDescriptors]()
    {
        for(const auto& [wDesc, path] : watchDescriptors)
        {
            if(inotify_rm_watch(std::get<0>(fileDescriptors).fd, wDesc) < 0)
                TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to remove watch descriptor of path: ", path, " (", Utils::String::GetStrError(), ")");
        }
        if(close(std::get<0>(fileDescriptors).fd) < 0)
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to close inotify file descriptor (", Utils::String::GetStrError(), ")");
        if(close(m_killEvent) < 0)
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to close kill event (", Utils::String::GetStrError(), ")");
    };

    for(const auto& path : m_paths)
    {
        const int32_t wd = inotify_add_watch(std::get<0>(fileDescriptors).fd, path.string().c_str(),
                                             IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);

        if(wd < 0)
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to add watch for path: ", path, " (", Utils::String::GetStrError(), ")");
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
                const int32_t wd1 = inotify_add_watch(std::get<0>(fileDescriptors).fd, p.path().string().c_str(),
                                                      IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);

                if(wd1 < 0)
                {
                    TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to add watch for path: ", path, " (", Utils::String::GetStrError(), ")");
                    continue; //Skip failed entry
                }

                watchDescriptors[wd1] = p.path();
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
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to poll events (", Utils::String::GetStrError(), ")");
            ErrorShutdown();
            return;
        }

        if((std::get<0>(fileDescriptors).revents != 0 && (((std::get<0>(fileDescriptors).revents & POLLHUP) != 0) || ((std::get<0>(fileDescriptors).revents & POLLERR) != 0))) ||
           (std::get<1>(fileDescriptors).revents != 0 && (((std::get<1>(fileDescriptors).revents & POLLHUP) != 0) || ((std::get<1>(fileDescriptors).revents & POLLERR) != 0))))
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Polling error");
            ErrorShutdown();
            return;
        }
        if((std::get<1>(fileDescriptors).revents & POLLIN) != 0)
        {
            uint64_t value = 0;
            const ssize_t len = read(std::get<1>(fileDescriptors).fd, &value, sizeof(value));
            if(len < 0)
            {
                TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to read kill event (", Utils::String::GetStrError(), ")");
                ErrorShutdown();
                return;
            }

            if(value == 1)
            {
                ErrorShutdown();
                return;
            }
        }

        const ssize_t len = read(std::get<0>(fileDescriptors).fd, buf.data(), buf.size());
        if(len == 0)
        {
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to read inotify events (", Utils::String::GetStrError(), ")");
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

            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to read inotify events (", Utils::String::GetStrError(), ")");
            ErrorShutdown();
            return;
        }

        std::size_t offset = 0;
        while(offset < NumericCast<std::size_t>(len)) //Process events
        {
            const inotify_event* const event = reinterpret_cast<const inotify_event*>(buf.data() + offset); //Must use reinterpret_cast because of flexible array member
            if(event->len == 0u)
            {
                offset += sizeof(inotify_event) + event->len;
                continue;
            }

            FileStatus status = FileStatus::Created;
            const std::filesystem::path filePath = watchDescriptors[event->wd] / std::filesystem::path(event->name);
            const bool isDir = std::filesystem::is_directory(filePath);
            std::optional<std::filesystem::path> oldFileName = std::nullopt;

            if((event->mask & IN_CREATE) != 0u)
            {
                if(isDir && m_recursive) //Add to tracking list
                {
                    const int32_t wd = inotify_add_watch(std::get<0>(fileDescriptors).fd, filePath.string().c_str(),
                                                            IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);

                    if(wd < 0)
                    {
                        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to add watch for path: ", filePath, " (", Utils::String::GetStrError(), ")");
                        ErrorShutdown();
                        return;
                    }

                    watchDescriptors[wd] = filePath;
                }

                status = FileStatus::Created;
            }
            else if((event->mask & IN_DELETE) != 0u)
            {
                if(isDir && m_recursive) //Remove from tracking list
                {
                    if(inotify_rm_watch(std::get<0>(fileDescriptors).fd, event->wd) < 0)
                        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to remove watch of path: ", filePath, " (", Utils::String::GetStrError(), ")");
                    watchDescriptors.erase(event->wd);
                }

                status = FileStatus::Erased;
            }
            else if((event->mask & IN_MODIFY) != 0u)
                status = FileStatus::Modified;
            else if((event->mask & IN_MOVED_FROM) != 0u)
            {
                oldName = filePath;
                status = FileStatus::Renamed;
            }
            else if((event->mask & IN_MOVED_TO) != 0u) //BUG Some distros (like Ubuntu) seem to not report IN_MOVED_TO on deletion
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

            if((event->mask & IN_MOVED_FROM) == 0u)
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
        if(inotify_rm_watch(std::get<0>(fileDescriptors).fd, wd) < 0)
            TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to remove inotify watch of path: ", path, " (", Utils::String::GetStrError(), ")");
    }

    if(close(std::get<0>(fileDescriptors).fd) < 0)
        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to close inotify (", Utils::String::GetStrError(), ")");

    if(close(m_killEvent) < 0)
        TP_ERROR(Log::FileWatcherLinuxPrefix, "Failed to close eventfd (", Utils::String::GetStrError(), ")");
}
#endif
