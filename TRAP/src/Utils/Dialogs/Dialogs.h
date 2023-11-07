#ifndef TRAP_DIALOGS_H
#define TRAP_DIALOGS_H

#include <ModernDialogs.h>

namespace TRAP::Utils::Dialogs
{
	/// @brief Open a save file dialog.
	/// @param title Title for the dialog.
	/// @param defaultPathAndFile Sets a default path and file.
	/// @param filterPatterns File filters (separate multiple extensions for the same filter with a ';'.
	/// Example: {"Test File", "*.Test;*.TS"}.
	/// @param allFiles Whether to add a filter for "All Files (*.*)" or not.
	/// @return Physical path of the Dialog or empty string.
	[[nodiscard]] std::string SaveFile(const std::string& title,
						               const std::string& defaultPathAndFile = "",
						               const std::vector<std::pair<std::string, std::string>>& filterPatterns = {},
						               bool allFiles = true);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Opens an open file dialog.
	/// @param title Title for the dialog.
	/// @param defaultPathAndFile Sets a default path and file.
	/// @param filterPatterns File filters (separate multiple extensions for the same filter with a ';'.
	/// Example: {"Test File", "*.Test;*.TS"}.
	/// @param allowMultipleSelects Whether to allow multiple file selections or not.
	/// @param allFiles Whether to add a filter for "All Files (*.*)" or not.
	/// @return Physical path(s) of the dialog or empty string or vector.
	[[nodiscard]] std::vector<std::string> OpenFile(const std::string& title,
									                const std::string& defaultPathAndFile = "",
									                const std::vector<std::pair<std::string, std::string>>& filterPatterns = {},
									                bool allowMultipleSelects = false,
									                bool allFiles = true);

	/// @brief Opens an open file dialog for a single file.
	/// Alias for OpenFile();
	/// @param title Title for the dialog.
	/// @param defaultPathAndFile Sets a default path and file.
	/// @param filterPatterns File filters (separate multiple extensions for the same filter with a ';'.
	/// Example: {"Test File", "*.Test;*.TS"}.
	/// @param allFiles Whether to add a filter for "All Files (*.*)" or not.
	/// @return Physical path of the dialog or empty string.
	[[nodiscard]] std::string OpenSingleFile(const std::string& title,
							                 const std::string& defaultPathAndFile = "",
							                 const std::vector<std::pair<std::string, std::string>>& filterPatterns = {},
							                 bool allFiles = true);

	/// @brief Opens an open file dialog for multiple files.
	/// Alias for OpenFile();
	/// @param title Title for the dialog.
	/// @param defaultPathAndFile Sets a default path and file.
	/// @param filterPatterns File filters (separate multiple extensions for the same filter with a ';'.
	/// Example: {"Test File", "*.Test;*.TS"}.
	/// @param allFiles Whether to add a filter for "All Files (*.*)" or not.
	/// @return Physical path(s) of the dialog or empty vector.
	[[nodiscard]] std::vector<std::string> OpenMultipleFiles(const std::string& title,
											                 const std::string& defaultPathAndFile = "",
											                 const std::vector<std::pair<std::string, std::string>>& filterPatterns = {},
											                 bool allFiles = true);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Opens an select folder dialog.
	/// @param title Title for the dialog.
	/// @param defaultPath Sets a default path and file.
	/// @return Physical path of the select folder dialog or empty string.
	[[nodiscard]] std::string SelectFolder(const std::string& title, const std::string& defaultPath = "");

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Styles for message boxes
	using Style = MD::Style;

	/// @brief Buttons for message boxes
	using Buttons = MD::Buttons;

	/// @brief Possible responses from a message box.
	/// "None" signifies that no option was selected.
	/// "Error" signifies that an error was encountered while creating the message box.
	using Selection = MD::Selection;

	/// @brief Blocking call to create a modal message box with the given title, message, style and buttons.
    /// @param title Title for the message box.
    /// @param message Message for the message box.
    /// @param style Style for the message box.
    /// @param buttons Button(s) for the message box.
    /// @return Selection made by the user.
    Selection ShowMsgBox(const std::string& title, const std::string& message, Style style, Buttons buttons);

    /// @brief Convenience function to call ShowMsgBox() with default button.
    /// @param title Title for the message box.
    /// @param message Message for the message box.
    /// @param style Style for the message box.
    /// @return Selection made by the user.
    Selection ShowMsgBox(const std::string& title, const std::string& message, Style style);

    /// @brief Convenience function to call ShowMsgBox() with default style.
    /// @param title Title for the message box.
    /// @param message Message for the message box.
    /// @param buttons Button(s) for the message box.
    /// @return Selection made by the user.
    Selection ShowMsgBox(const std::string& title, const std::string& message, Buttons buttons);

    /// @brief Convenience function to call ShowMsgBox() with default style and button.
    /// @param title Title for the message box.
    /// @param message Message for the message box.
    /// @return Selection made by the user.
    Selection ShowMsgBox(const std::string& title, const std::string& message);
}

#endif /*TRAP_DIALOGS_H*/