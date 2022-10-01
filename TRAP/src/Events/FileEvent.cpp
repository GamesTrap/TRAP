#include "TRAPPCH.h"
#include "FileEvent.h"

#include "FileSystem/FileWatcher.h"
#include <stdexcept>

TRAP::Events::FileChangeEvent::FileChangeEvent(TRAP::FileSystem::FileStatus status, std::filesystem::path path,
                                               std::filesystem::path oldName)
    : m_status(status), m_path(std::move(path)), m_oldName(std::move(oldName))
{
	ZoneScoped;
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::Events::FileChangeEvent::GetPath() const
{
	ZoneScoped;

    return m_path;
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::Events::FileChangeEvent::GetOldName() const
{
	ZoneScoped;

    return m_oldName;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::FileChangeEvent::ToString() const
{
	ZoneScoped;

    return "FileChangeEvent: Path: " + m_path.u8string() + " Status: " +
           FileStatusToString(m_status) + (m_oldName.empty() ? "" : " OldName: " + m_oldName.u8string());
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::FileChangeEvent::GetEventType() const
{
	ZoneScoped;

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::FileChangeEvent::GetName() const
{
	ZoneScoped;

	return "FileChange";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::FileChangeEvent::GetCategoryFlags() const
{
	ZoneScoped;

	return EventCategory::FileChange;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::FileChangeEvent::FileStatusToString(const TRAP::FileSystem::FileStatus status)
{
	ZoneScoped;

    switch(status)
    {
    case TRAP::FileSystem::FileStatus::Created:
        return "Created";

    case TRAP::FileSystem::FileStatus::Renamed:
        return "Renamed";

    case TRAP::FileSystem::FileStatus::Modified:
        return "Modified";

    case TRAP::FileSystem::FileStatus::Erased:
        return "Erased";

    default:
        throw std::invalid_argument("Unimplemented enum->string value for TRAP::FileSystem::FileStatus");
    }
}