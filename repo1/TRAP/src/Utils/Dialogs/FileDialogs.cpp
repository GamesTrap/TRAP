#include "TRAPPCH.h"
#include "FileDialogs.h"

#include <ModernFileDialogs.h>

std::string TRAP::Utils::Dialogs::SaveFile(const std::string& title,
                                           const std::string& defaultPathAndFile,
                                           const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                                           const bool allFiles)
{
    return ::SaveFile(title, defaultPathAndFile, filterPatterns, allFiles);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Utils::Dialogs::OpenFile(const std::string& title,
                                                        const std::string& defaultPathAndFile,
                                                        const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                                                        const bool allowMultipleSelects,
                                                        const bool allFiles)
{
    return ::OpenFile(title, defaultPathAndFile, filterPatterns, allowMultipleSelects, allFiles);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::Dialogs::OpenSingleFile(const std::string& title,
                                                 const std::string& defaultPathAndFile,
                                                 const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                                                 const bool allFiles)
{
    return ::OpenSingleFile(title, defaultPathAndFile, filterPatterns, allFiles);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Utils::Dialogs::OpenMultipleFiles(const std::string& title,
                                                                 const std::string& defaultPathAndFile,
                                                                 const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                                                                 const bool allFiles)
{
    return ::OpenMultipleFiles(title, defaultPathAndFile, filterPatterns, allFiles);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::Dialogs::SelectFolder(const std::string& title, const std::string& defaultPath)
{
    return ::SelectFolder(title, defaultPath);
}