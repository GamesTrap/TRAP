#include "TRAPPCH.h"
#include "FileEvent.h"

TRAP::Events::FileSystemChangeEvent::FileSystemChangeEvent(TRAP::FileSystem::FileSystemStatus status, std::filesystem::path path,
                                               const TRAP::Optional<std::filesystem::path>& oldPath) noexcept
    : m_status(status), m_path(std::move(path)), m_oldPath(oldPath)
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (GetTRAPProfileSystems() & ProfileSystems::Events) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::filesystem::path TRAP::Events::FileSystemChangeEvent::GetPath() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (GetTRAPProfileSystems() & ProfileSystems::Events) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    return m_path;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::filesystem::path> TRAP::Events::FileSystemChangeEvent::GetOldPath() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (GetTRAPProfileSystems() & ProfileSystems::Events) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    return m_oldPath;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::FileSystemChangeEvent::ToString() const
{
	return fmt::format("FileSystemChangeEvent: Path: {} Status: {}{}", m_path, m_status,
	                   (!m_oldPath ? "" : " OldPath: " + m_oldPath->string()));
}
