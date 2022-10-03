#include "TRAPPCH.h"
#include "Dialogs.h"

std::string TRAP::Utils::Dialogs::SaveFile(const std::string& title,
                                           const std::string& defaultPathAndFile,
                                           const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                                           const bool allFiles)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

    return MD::SaveFile(title, defaultPathAndFile, filterPatterns, allFiles);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Utils::Dialogs::OpenFile(const std::string& title,
                                                        const std::string& defaultPathAndFile,
                                                        const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                                                        const bool allowMultipleSelects,
                                                        const bool allFiles)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

    return MD::OpenFile(title, defaultPathAndFile, filterPatterns, allowMultipleSelects, allFiles);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::Dialogs::OpenSingleFile(const std::string& title,
                                                 const std::string& defaultPathAndFile,
                                                 const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                                                 const bool allFiles)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

    return MD::OpenSingleFile(title, defaultPathAndFile, filterPatterns, allFiles);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Utils::Dialogs::OpenMultipleFiles(const std::string& title,
                                                                 const std::string& defaultPathAndFile,
                                                                 const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                                                                 const bool allFiles)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

    return MD::OpenMultipleFiles(title, defaultPathAndFile, filterPatterns, allFiles);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::Dialogs::SelectFolder(const std::string& title, const std::string& defaultPath)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

    return MD::SelectFolder(title, defaultPath);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Dialogs::Selection TRAP::Utils::Dialogs::ShowMsgBox(const std::string& title,
                                                                 const std::string& message,
    const Style style, const Buttons buttons)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

    return MD::ShowMsgBox(title, message, style, buttons);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Dialogs::Selection TRAP::Utils::Dialogs::ShowMsgBox(const std::string& title,
                                                                 const std::string& message,
    const Style style)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

    return MD::ShowMsgBox(title, message, style);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Dialogs::Selection TRAP::Utils::Dialogs::ShowMsgBox(const std::string& title,
                                                                 const std::string& message,
    const Buttons buttons)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

    return MD::ShowMsgBox(title, message, buttons);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Dialogs::Selection TRAP::Utils::Dialogs::ShowMsgBox(const std::string& title,
                                                                 const std::string& message)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

    return MD::ShowMsgBox(title, message);
}