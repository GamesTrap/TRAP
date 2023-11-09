#include "TRAPPCH.h"
#include "FileEvent.h"

#include "FileSystem/FileWatcher.h"
#include "Utils/String/String.h"

TRAP::Events::FileChangeEvent::FileChangeEvent(TRAP::FileSystem::FileStatus status, std::filesystem::path path,
                                               const std::optional<std::filesystem::path>& oldName) noexcept
    : m_status(status), m_path(std::move(path)), m_oldName(oldName)
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::filesystem::path TRAP::Events::FileChangeEvent::GetPath() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    return m_path;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<std::filesystem::path> TRAP::Events::FileChangeEvent::GetOldName() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    return m_oldName;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::FileChangeEvent::ToString() const
{
	return fmt::format("FileChangeEvent: Path: {} Status: {}{}", m_path, m_status,
							(!m_oldName ? "" : " OldName: " + m_oldName->string()));
}
