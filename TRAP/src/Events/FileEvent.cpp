#include "TRAPPCH.h"
#include "FileEvent.h"

#include "FS/FileWatcher.h"

TRAP::Events::FileChangeEvent::FileChangeEvent(TRAP::FS::FileStatus status, std::filesystem::path path,
                                               std::filesystem::path oldName)
    : m_status(status), m_path(std::move(path)), m_oldName(std::move(oldName))
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FS::FileStatus TRAP::Events::FileChangeEvent::GetStatus() const
{
    return m_status;
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::Events::FileChangeEvent::GetPath() const
{
    return m_path;
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::Events::FileChangeEvent::GetOldName() const
{
    return m_oldName;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::FileChangeEvent::ToString() const
{
    return "FileChangeEvent: Path: " + m_path.generic_u8string() + " Status: " +
           FileStatusToString(m_status) + (m_oldName.empty() ? "" : " OldName: " + m_oldName.generic_u8string());
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::FileChangeEvent::GetStaticType()
{
	return EventType::FileChange;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::FileChangeEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Events::FileChangeEvent::GetName() const
{
	return "FileChange";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::FileChangeEvent::GetCategoryFlags() const
{
	return EventCategory::FileChange;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::FileChangeEvent::FileStatusToString(TRAP::FS::FileStatus status)
{
    switch(status)
    {
    case TRAP::FS::FileStatus::Created:
        return "Created";

    case TRAP::FS::FileStatus::Renamed:
        return "Renamed";

    case TRAP::FS::FileStatus::Modified:
        return "Modified";

    case TRAP::FS::FileStatus::Erased:
        return "Erased";

    default:
        return "";
    }
}