#ifndef _TRAP_FILEDIALOGS_H_
#define _TRAP_FILEDIALOGS_H_

namespace TRAP::Utils::Dialogs
{
	/// <summary>
	/// Open a Save File Dialog.
	/// </summary>
	/// <param name="title">Title for the Dialog.</param>
	/// <param name="defaultPathAndFile">Sets a default path and file.</param>
	/// <param name="filterPatterns">File filters (Separate multiple extensions for the same filter with a ';'. Example: {"Test File", "*.Test;*.TS"}.</param>
	/// <param name="allFiles">Whether to add a filter for "All Files (*.*)" or not.</param>
	/// <returns>Physical Path of the Dialog or empty string.</returns>
	std::string SaveFile(const std::string& title,
						 const std::string& defaultPathAndFile = "",
						 const std::vector<std::pair<std::string, std::string>>& filterPatterns = {},
						 bool allFiles = true);

	//-------------------------------------------------------------------------------------------------------------------//
	
	/// <summary>
	/// Opens an Open File Dialog.
	/// </summary>
	/// <param name="title">Title for the Dialog.</param>
	/// <param name="defaultPathAndFile">Sets a default path and file.</param>
	/// <param name="filterPatterns">File filters (Separate multiple extensions for the same filter with a ';'. Example: {"Test File", "*.Test;*.TS"}.</param>
	/// <param name="allowMultipleSelects">Whether to allow multiple file selections or not.</param>
	/// <param name="allFiles">Whether to add a filter for "All Files (*.*)" or not.</param>
	/// <returns>Physical Path(s) of the Dialog or empty string or vector.</returns>
	std::vector<std::string> OpenFile(const std::string& title,
									  const std::string& defaultPathAndFile = "",
									  const std::vector<std::pair<std::string, std::string>>& filterPatterns = {},
									  bool allowMultipleSelects = false,
									  bool allFiles = true);

	/// <summary>
	/// Opens an Open File Dialog for a single file.<br>
	/// Alias for OpenFile();
	/// </summary>
	/// <param name="title">Title for the Dialog.</param>
	/// <param name="defaultPathAndFile">Sets a default path and file.</param>
	/// <param name="filterPatterns">File filters (Separate multiple extensions for the same filter with a ';'. Example: {"Test File", "*.Test;*.TS"}.</param>
	/// <param name="allFiles">Whether to add a filter for "All Files (*.*)" or not.</param>
	/// <returns>Physical Path of the Dialog or empty string.</returns>
	std::string OpenSingleFile(const std::string& title,
							   const std::string& defaultPathAndFile = "",
							   const std::vector<std::pair<std::string, std::string>>& filterPatterns = {},
							   bool allFiles = true);

	/// <summary>
	/// Opens an Open File Dialog for multiple files.<br>
	/// Alias for OpenFile();
	/// </summary>
	/// <param name="title">Title for the Dialog.</param>
	/// <param name="defaultPathAndFile">Sets a default path and file.</param>
	/// <param name="filterPatterns">File filters (Separate multiple extensions for the same filter with a ';'. Example: {"Test File", "*.Test;*.TS"}.</param>
	/// <param name="allFiles">Whether to add a filter for "All Files (*.*)" or not.</param>
	/// <returns>Physical Paths of the Dialog or empty vector.</returns>
	std::vector<std::string> OpenMultipleFiles(const std::string& title,
											   const std::string& defaultPathAndFile = "",
											   const std::vector<std::pair<std::string, std::string>>& filterPatterns = {},
											   bool allFiles = true);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Opens an Select Folder Dialog.
	/// </summary>
	/// <param name="title">Title for the Dialog.</param>
	/// <param name="defaultPath">Sets a default path and file.</param>
	/// <returns>Physical Path of the Select Folder Dialog or empty string.</returns>
	std::string SelectFolder(const std::string& title, const std::string& defaultPath = "");
}

#endif /*_TRAP_FILEDIALOGS_H_*/