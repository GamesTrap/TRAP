#include "TRAPPCH.h"
#include "Dialogs.h"

[[nodiscard]] std::string TRAP::Utils::Dialogs::SaveFile(const std::string& title,
                                                         const std::string& defaultPathAndFile,
                                                         const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                                                         const bool allFiles)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    return MD::SaveFile(title, defaultPathAndFile, filterPatterns, allFiles);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<std::string> TRAP::Utils::Dialogs::OpenFile(const std::string& title,
                                                                      const std::string& defaultPathAndFile,
                                                                      const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                                                                      const bool allowMultipleSelects,
                                                                      const bool allFiles)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    return MD::OpenFile(title, defaultPathAndFile, filterPatterns, allowMultipleSelects, allFiles);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Utils::Dialogs::OpenSingleFile(const std::string& title,
                                                               const std::string& defaultPathAndFile,
                                                               const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                                                               const bool allFiles)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    return MD::OpenSingleFile(title, defaultPathAndFile, filterPatterns, allFiles);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<std::string> TRAP::Utils::Dialogs::OpenMultipleFiles(const std::string& title,
                                                                               const std::string& defaultPathAndFile,
                                                                               const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                                                                               const bool allFiles)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    return MD::OpenMultipleFiles(title, defaultPathAndFile, filterPatterns, allFiles);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Utils::Dialogs::SelectFolder(const std::string& title, const std::string& defaultPath)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    return MD::SelectFolder(title, defaultPath);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Utils::Dialogs::Selection TRAP::Utils::Dialogs::ShowMsgBox(const std::string& title,
                                                                               const std::string& message,
    const Style style, const Buttons buttons)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    return MD::ShowMsgBox(title, message, style, buttons);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Utils::Dialogs::Selection TRAP::Utils::Dialogs::ShowMsgBox(const std::string& title,
                                                                               const std::string& message,
    const Style style)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    return MD::ShowMsgBox(title, message, style);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Utils::Dialogs::Selection TRAP::Utils::Dialogs::ShowMsgBox(const std::string& title,
                                                                               const std::string& message,
    const Buttons buttons)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    return MD::ShowMsgBox(title, message, buttons);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Utils::Dialogs::Selection TRAP::Utils::Dialogs::ShowMsgBox(const std::string& title,
                                                                               const std::string& message)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    return MD::ShowMsgBox(title, message);
}
