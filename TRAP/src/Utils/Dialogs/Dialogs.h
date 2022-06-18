#ifndef TRAP_DIALOGS_H
#define TRAP_DIALOGS_H

#include <ModernDialogs.h>

namespace TRAP::Utils::Dialogs
{
	/// <summary>
	/// Open a save file dialog.
	/// </summary>
	/// <param name="title">Title for the dialog.</param>
	/// <param name="defaultPathAndFile">Sets a default path and file.</param>
	/// <param name="filterPatterns">
	/// File filters (separate multiple extensions for the same filter with a ';'.
	/// Example: {"Test File", "*.Test;*.TS"}.
	/// </param>
	/// <param name="allFiles">Whether to add a filter for "All Files (*.*)" or not.</param>
	/// <returns>Physical path of the Dialog or empty string.</returns>
	std::string SaveFile(const std::string& title,
						 const std::string& defaultPathAndFile = "",
						 const std::vector<std::pair<std::string, std::string>>& filterPatterns = {},
						 bool allFiles = true);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Opens an open file dialog.
	/// </summary>
	/// <param name="title">Title for the dialog.</param>
	/// <param name="defaultPathAndFile">Sets a default path and file.</param>
	/// <param name="filterPatterns">
	/// File filters (separate multiple extensions for the same filter with a ';'.
	/// Example: {"Test File", "*.Test;*.TS"}.
	/// </param>
	/// <param name="allowMultipleSelects">Whether to allow multiple file selections or not.</param>
	/// <param name="allFiles">Whether to add a filter for "All Files (*.*)" or not.</param>
	/// <returns>Physical path(s) of the dialog or empty string or vector.</returns>
	std::vector<std::string> OpenFile(const std::string& title,
									  const std::string& defaultPathAndFile = "",
									  const std::vector<std::pair<std::string, std::string>>& filterPatterns = {},
									  bool allowMultipleSelects = false,
									  bool allFiles = true);

	/// <summary>
	/// Opens an open file dialog for a single file.
	/// Alias for OpenFile();
	/// </summary>
	/// <param name="title">Title for the dialog.</param>
	/// <param name="defaultPathAndFile">Sets a default path and file.</param>
	/// <param name="filterPatterns">
	/// File filters (separate multiple extensions for the same filter with a ';'.
	/// Example: {"Test File", "*.Test;*.TS"}.
	/// </param>
	/// <param name="allFiles">Whether to add a filter for "All Files (*.*)" or not.</param>
	/// <returns>Physical path of the dialog or empty string.</returns>
	std::string OpenSingleFile(const std::string& title,
							   const std::string& defaultPathAndFile = "",
							   const std::vector<std::pair<std::string, std::string>>& filterPatterns = {},
							   bool allFiles = true);

	/// <summary>
	/// Opens an open file dialog for multiple files.
	/// Alias for OpenFile();
	/// </summary>
	/// <param name="title">Title for the dialog.</param>
	/// <param name="defaultPathAndFile">Sets a default path and file.</param>
	/// <param name="filterPatterns">
	/// File filters (separate multiple extensions for the same filter with a ';'.
	/// Example: {"Test File", "*.Test;*.TS"}.
	/// </param>
	/// <param name="allFiles">Whether to add a filter for "All Files (*.*)" or not.</param>
	/// <returns>Physical path(s) of the dialog or empty vector.</returns>
	std::vector<std::string> OpenMultipleFiles(const std::string& title,
											   const std::string& defaultPathAndFile = "",
											   const std::vector<std::pair<std::string, std::string>>& filterPatterns = {},
											   bool allFiles = true);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Opens an select folder dialog.
	/// </summary>
	/// <param name="title">Title for the dialog.</param>
	/// <param name="defaultPath">Sets a default path and file.</param>
	/// <returns>Physical path of the select folder dialog or empty string.</returns>
	std::string SelectFolder(const std::string& title, const std::string& defaultPath = "");

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Styles for message boxes
	/// </summary>
	using Style = MD::Style;

	/// <summary>
	/// Buttons for message boxes
	/// </summary>
	using Buttons = MD::Buttons;

	/// <summary>
	/// Possible responses from a message box.
	/// "None" signifies that no option was selected.
	/// "Error" signifies that an error was encountered while creating the message box.
	/// </summary>
	using Selection = MD::Selection;

	/// <summary>
    /// Blocking call to create a modal message box with the given title, message, style and buttons.
    /// </summary>
    /// <param name="title">Title for the message box.</param>
    /// <param name="message">Message for the message box.</param>
    /// <param name="style">Style for the message box.</param>
    /// <param name="buttons">Button(s) for the message box.</param>
    /// <returns>Selection made by the user.</returns>
    Selection ShowMsgBox(const std::string& title, const std::string& message, Style style, Buttons buttons);

    /// <summary>
    /// Convenience function to call ShowMsgBox() with default button.
    /// </summary>
    /// <param name="title">Title for the message box.</param>
    /// <param name="message">Message for the message box.</param>
    /// <param name="style">Style for the message box.</param>
    /// <returns>Selection made by the user.</returns>
    Selection ShowMsgBox(const std::string& title, const std::string& message, Style style);

    /// <summary>
    /// Convenience function to call ShowMsgBox() with default style.
    /// </summary>
    /// <param name="title">Title for the message box.</param>
    /// <param name="message">Message for the message box.</param>
    /// <param name="buttons">Button(s) for the message box.</param>
    /// <returns>Selection made by the user.</returns>
    Selection ShowMsgBox(const std::string& title, const std::string& message, Buttons buttons);

    /// <summary>
    /// Convenience function to call ShowMsgBox() with default style and button.
    /// </summary>
    /// <param name="title">Title for the message box.</param>
    /// <param name="message">Message for the message box.</param>
    /// <returns>Selection made by the user.</returns>
    Selection ShowMsgBox(const std::string& title, const std::string& message);
}

#endif /*TRAP_DIALOGS_H*/