/*
MIT License

Copyright (c) 2020 Jan "GamesTrap" Schürkamp

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "ModernFileDialogs.h"

#include <algorithm>

#ifdef _WIN32
#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0500
#endif
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <conio.h>
#include <direct.h>
#else
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <termios.h>
#include <sys/utsname.h>
#include <signal.h>
#endif

//-------------------------------------------------------------------------------------------------------------------//

constexpr static int32_t MaxPathOrCMD = 1024;
constexpr static int32_t MaxMultipleFiles = 1024;

//-------------------------------------------------------------------------------------------------------------------//

//Windows land
#ifdef _WIN32

std::wstring GetPathWithoutFinalSlashW(const std::wstring& source)
{
	std::wstring result;
	
	if (!source.empty())
	{
		std::size_t index = source.find_last_of(L'/');
		if (index == std::wstring_view::npos)
			index = source.find_last_of(L'\\');
		if (index != std::string_view::npos)
			result = source.substr(0, index);
		else
			result = {};
	}
	else
		result = {};

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::wstring GetLastNameW(const std::wstring& source)
{
	std::wstring result;
	
	if (!source.empty())
	{
		std::size_t index = source.find_last_of(L'/');
		if (index == std::wstring_view::npos)
			index = source.find_last_of(L'\\');
		if (index != std::wstring_view::npos)
			result = source.substr(index + 1);
		else
			result = source;
	}
	else
		result = {};

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::wstring UTF8To16(const std::string& UTF8String)
{
	std::wstring result;

	const int32_t count = MultiByteToWideChar(CP_UTF8, 0, UTF8String.data(), -1, nullptr, 0);
	if (!count)
		return {};

	result.resize(count);

	if (!MultiByteToWideChar(CP_UTF8, 0, UTF8String.data(), -1, result.data(), static_cast<int32_t>(result.size())))
		return {};
	
	result.pop_back();

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string UTF16To8(const std::wstring& UTF16String)
{
	std::string result;

	const int32_t count = WideCharToMultiByte(CP_UTF8, 0, UTF16String.data(), -1, nullptr, 0, nullptr, nullptr);
	if (!count)
		return {};

	result.resize(count);

	if (!WideCharToMultiByte(CP_UTF8, 0, UTF16String.data(), -1, result.data(), static_cast<int32_t>(result.size()), nullptr, nullptr))
		return {};

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

bool DirExists(const std::string& dirPath)
{
	struct _stat info {};

	if (dirPath.empty())
		return false;
	const std::size_t dirLen = dirPath.length();
	if (!dirLen)
		return true;
	if ((dirLen == 2) && (dirPath[1] == ':'))
		return true;

	const std::wstring wStr = UTF8To16(dirPath);
	const int32_t statRet = _wstat(wStr.data(), &info);
	if (statRet != 0)
		return false;
	if (info.st_mode & S_IFDIR)
		return true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool FileExists(const std::string& filePathAndName)
{
	struct _stat info;

	if (filePathAndName.empty())
		return false;

	std::wstring temp = UTF8To16(filePathAndName);
	const int32_t statRet = _wstat(temp.data(), &info);
	
	if (statRet != 0)
		return false;
	
	if (info.st_mode & _S_IFREG)
		return true;
	
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

BOOL CALLBACK BrowseCallbackProcWEnum(HWND hwndChild, LPARAM lParam)
{
	std::wstring buffer;
	buffer.resize(255);
	GetClassNameW(hwndChild, buffer.data(), buffer.size());
	if(buffer == L"SysTreeView32")
	{
		HTREEITEM hNode = TreeView_GetSelection(hwndChild);
		TreeView_EnsureVisible(hwndChild, hNode);
		return FALSE;
	}
	
	return TRUE;
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t BrowseCallbackProcW(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTIONW, TRUE, static_cast<LPARAM>(pData));
		break;

	case BFFM_SELCHANGED:
		EnumChildWindows(hwnd, BrowseCallbackProcWEnum, 0);
		break;
		
	default:
		break;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

std::wstring SaveFileW(const std::wstring& title,
                       const std::wstring& defaultPathAndFile,
                       const std::vector<std::pair<std::wstring, std::wstring>>& filterPatterns,
                       const bool allFiles)
{
	static std::wstring buffer{};
	std::wstring defaultExtension{};
	std::wstring filterPatternsStr;
	std::wstring dirName;
	HRESULT hResult = CoInitializeEx(nullptr, 0);

	dirName = GetPathWithoutFinalSlashW(defaultPathAndFile);
	buffer = GetLastNameW(defaultPathAndFile);

	if (!filterPatterns.empty())
	{
		defaultExtension = filterPatterns[0].second;
		if (filterPatterns[0].first.empty())
		{
			filterPatternsStr += filterPatterns[0].second + L'\n';
			filterPatternsStr += filterPatterns[0].second;
		}
		else
			filterPatternsStr += filterPatterns[0].first + L"(" + filterPatterns[0].second + L")\n" + filterPatterns[0].second;
		for (uint32_t i = 1; i < filterPatterns.size(); i++)
		{
			if (filterPatterns[i].first.empty())
			{
				filterPatternsStr += L'\n' + filterPatterns[i].second;
				filterPatternsStr += L'\n' + filterPatterns[i].second;
			}
			else
				filterPatternsStr += L'\n' + filterPatterns[i].first + L"(" + filterPatterns[i].second + L")\n" + filterPatterns[i].second;
		}
		filterPatternsStr += L'\n';
		if (allFiles)
			filterPatternsStr += L"All Files\n*.*\n";
		else
			filterPatternsStr += L'\n';
	}
	else
	{
		defaultExtension = L"*.*";

		filterPatternsStr = L"All Files\n*.*\n";
	}

	std::replace(filterPatternsStr.begin(), filterPatternsStr.end(), L'\n', L'\0');

	OPENFILENAMEW ofn;
	std::memset(&ofn, 0, sizeof(OPENFILENAMEW));

	buffer.resize(MaxPathOrCMD);
	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = GetForegroundWindow();
	ofn.hInstance = nullptr;
	ofn.lpstrFilter = filterPatternsStr.empty() ? nullptr : filterPatternsStr.data();
	ofn.lpstrCustomFilter = nullptr;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = buffer.data();

	ofn.nMaxFile = MaxPathOrCMD;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = MaxPathOrCMD / 2;
	ofn.lpstrInitialDir = dirName.empty() ? nullptr : dirName.data();
	ofn.lpstrTitle = title.empty() ? nullptr : title.data();
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = defaultExtension.data();
	ofn.lCustData = 0L;
	ofn.lpfnHook = nullptr;
	ofn.lpTemplateName = nullptr;

	std::wstring retVal;
	if (GetSaveFileNameW(&ofn) == 0)
		retVal = {};
	else
		retVal = buffer;

	if (hResult == S_OK || hResult == S_FALSE)
		CoUninitialize();

	return retVal;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string SaveFileWinGUI(const std::string& title,
                           const std::string& defaultPathAndFile,
                           const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                           const bool allFiles)
{
	std::vector<std::pair<std::wstring, std::wstring>> wFilterPatterns(filterPatterns.size());
	for (uint32_t i = 0; i < wFilterPatterns.size(); i++)
		wFilterPatterns[i] = { UTF8To16(filterPatterns[i].first), UTF8To16(filterPatterns[i].second) };
	const std::wstring wTitle = UTF8To16(title);
	const std::wstring wDefaultPathAndFile = UTF8To16(defaultPathAndFile);

	const std::wstring wPath = SaveFileW(wTitle, wDefaultPathAndFile, wFilterPatterns, allFiles);

	if (wPath.empty())
		return {};

	std::string path = UTF16To8(wPath);
	UTF16To8({});
	
	return path;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::wstring> OpenFileW(const std::wstring& title,
                                    const std::wstring& defaultPathAndFile,
                                    const std::vector<std::pair<std::wstring, std::wstring>>& filterPatterns,
                                    const bool allowMultipleSelects,
                                    const bool allFiles)
{
	HRESULT hResult = CoInitializeEx(nullptr, 0);

	static std::wstring buffer;
	
	std::wstring dirName = GetPathWithoutFinalSlashW(defaultPathAndFile);
	buffer = GetLastNameW(defaultPathAndFile);

	if (allowMultipleSelects)
		buffer.resize(MaxMultipleFiles * MaxPathOrCMD + 1);
	else
		buffer.resize(MaxPathOrCMD + 1);
	
	std::wstring filterPatternsStr;
	if (!filterPatterns.empty())
	{
		if (filterPatterns[0].first.empty())
		{
			filterPatternsStr += filterPatterns[0].second + L'\n';
			filterPatternsStr += filterPatterns[0].second;
		}
		else
			filterPatternsStr += filterPatterns[0].first + L"(" + filterPatterns[0].second + L")\n" + filterPatterns[0].second;
		for (uint32_t i = 1; i < filterPatterns.size(); i++)
		{
			if (filterPatterns[i].first.empty())
			{
				filterPatternsStr += L'\n' + filterPatterns[i].second;
				filterPatternsStr += L'\n' + filterPatterns[i].second;
			}
			else
				filterPatternsStr += L'\n' + filterPatterns[i].first + L"(" + filterPatterns[i].second + L")\n" + filterPatterns[i].second;
		}
		filterPatternsStr += L'\n';
		if (allFiles)
			filterPatternsStr += L"All Files\n*.*\n";
		else
			filterPatternsStr += L'\n';
	}
	else
		filterPatternsStr = L"All Files\n*.*\n";

	std::replace(filterPatternsStr.begin(), filterPatternsStr.end(), L'\n', L'\0');
	
	OPENFILENAMEW ofn;
	std::memset(&ofn, 0, sizeof(OPENFILENAMEW));
	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = GetForegroundWindow();
	ofn.hInstance = nullptr;
	ofn.lpstrFilter = filterPatternsStr.empty() ? nullptr : filterPatternsStr.data();
	ofn.lpstrCustomFilter = nullptr;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = buffer.data();
	
	ofn.nMaxFile = static_cast<uint32_t>(buffer.size());
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = MaxPathOrCMD / 2;
	ofn.lpstrInitialDir = dirName.empty() ? nullptr : dirName.data();
	ofn.lpstrTitle = title.empty() ? nullptr : title.data();
	ofn.Flags = OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = nullptr;
	ofn.lCustData = 0L;
	ofn.lpfnHook = nullptr;
	ofn.lpTemplateName = nullptr;
	if (allowMultipleSelects)
		ofn.Flags |= OFN_ALLOWMULTISELECT;

	std::vector<std::wstring> paths{};
	if (!GetOpenFileNameW(&ofn))
		buffer = {};
	else
	{
		if (allowMultipleSelects)
		{
			std::wstring folder;
			do
			{
				auto x = buffer.find_first_of(L'\0');
				std::wstring s = buffer.substr(0, x);

				if (folder.empty())
					folder = s + L"\\";
				else
					paths.push_back(folder + s);

				buffer = buffer.substr(x + 1, buffer.size() - (x + 1));
			} while (buffer[0] != L'\0');
		}
		else
			paths.push_back(buffer);
	}

	if (hResult == S_OK || hResult == S_FALSE)
		CoUninitialize();

	return paths;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> OpenFileWinGUI(const std::string& title,
                                        const std::string& defaultPathAndFile,
                                        const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                                        const bool allowMultipleSelects,
                                        const bool allFiles)
{
	std::wstring wTitle;
	std::wstring wDefaultPathAndFile;
	
	std::vector<std::pair<std::wstring, std::wstring>> wFilterPatterns(filterPatterns.size());
	for(uint32_t i = 0; i < wFilterPatterns.size(); i++)
		wFilterPatterns[i] = { UTF8To16(filterPatterns[i].first), UTF8To16(filterPatterns[i].second) };

	if (!title.empty())
		wTitle = UTF8To16(title);
	if (!defaultPathAndFile.empty())
		wDefaultPathAndFile = UTF8To16(defaultPathAndFile);

	std::vector<std::wstring> wPaths = OpenFileW(wTitle, wDefaultPathAndFile, wFilterPatterns, allowMultipleSelects, allFiles);

	if (wPaths.empty())
		return {};

	std::vector<std::string> paths(wPaths.size());
	for (uint32_t i = 0; i < paths.size(); i++)
		paths[i] = UTF16To8(wPaths[i]);

	return paths;
}

//-------------------------------------------------------------------------------------------------------------------//

std::wstring SelectFolderW(const std::wstring& title, const std::wstring& defaultPath)
{
	static std::wstring buffer;
	buffer.resize(MaxPathOrCMD);

	HRESULT hResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	BROWSEINFOW info;
	std::memset(&info, 0, sizeof(BROWSEINFOW));

	info.hwndOwner = GetForegroundWindow();
	info.pidlRoot = nullptr;
	info.pszDisplayName = buffer.data();
	info.lpszTitle = title.empty() ? nullptr : title.data();
	if (hResult == S_OK || hResult == S_FALSE)
		info.ulFlags = BIF_USENEWUI;
	info.lpfn = BrowseCallbackProcW;
	info.lParam = reinterpret_cast<LPARAM>(defaultPath.data());
	info.iImage = -1;

	std::wstring retVal;
	LPITEMIDLIST lpItem = SHBrowseForFolderW(&info);
	if(lpItem)
	{
		SHGetPathFromIDListW(lpItem, buffer.data());
		retVal = buffer;
	}

	if (hResult == S_OK || hResult == S_FALSE)
		CoUninitialize();

	return retVal;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string SelectFolderWinGUI(const std::string& title, const std::string& defaultPath)
{
	std::wstring wTitle;
	std::wstring wDefaultPath;
	
	if (!title.empty())
		wTitle = UTF8To16(title);
	if (!defaultPath.empty())
		wDefaultPath = UTF8To16(defaultPath);

	const std::wstring wPath = SelectFolderW(wTitle, wDefaultPath);

	if (wPath.empty())
		return {};

	return UTF16To8(wPath);
}

//-------------------------------------------------------------------------------------------------------------------//

//Linux land
#else

std::string Python3Name;
	
bool DetectPresence(const std::string& executable)
{
	std::string buffer;
	buffer.resize(MaxPathOrCMD);
	FILE* in;
	
	std::string testedString = "which " + executable + " 2>/dev/null ";
	in = popen(testedString.data(), "r");
	if((fgets(buffer.data(), buffer.size(), in) != nullptr)
		&& (buffer.find_first_of(':') == std::string::npos) && (buffer.find("no ") == std::string::npos))
	{
		pclose(in);
		return true;
	}
	else
	{
		pclose(in);
		return false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool DirExists(const std::string& dirPath)
{
	DIR* dir;
	
	if (dirPath.empty())
		return false;
	
	dir = opendir(dirPath.data());
	
	if (!dir)
		return false;
	
	closedir(dir);
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool GetEnvDISPLAY()
{
	return std::getenv("DISPLAY");
}

//-------------------------------------------------------------------------------------------------------------------//

bool IsDarwin()
{
	static int32_t isDarwin = -1;
	struct utsname lUtsname;
	
	if (isDarwin < 0)
		isDarwin = !uname(&lUtsname) && std::string(lUtsname.sysname) == "Darwin";
	
	return isDarwin;
}

//-------------------------------------------------------------------------------------------------------------------//

bool GraphicMode()
{
	return (GetEnvDISPLAY() || (IsDarwin() && GetEnvDISPLAY()));
}

//-------------------------------------------------------------------------------------------------------------------//

bool XPropPresent()
{
	static int32_t xpropPresent = -1;
	
	if (xpropPresent < 0)
		xpropPresent = DetectPresence("xprop");

	return xpropPresent && GraphicMode();
}

//-------------------------------------------------------------------------------------------------------------------//

bool ZenityPresent()
{
	static int32_t zenityPresent = -1;
	
	if (zenityPresent < 0)
		zenityPresent = DetectPresence("zenity");

	return zenityPresent && GraphicMode();
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t Zenity3Present()
{
	static int32_t zenity3Present = -1;
	FILE* in;
	std::string buffer;
	buffer.resize(MaxPathOrCMD);
	
	if(zenity3Present < 0)
	{
		zenity3Present = 0;
		if(ZenityPresent())
		{
			in = popen("zenity --version", "r");
			if(fgets(buffer.data(), buffer.size(), in) != nullptr)
			{
				if(std::stoi(buffer) >= 3)
				{
					zenity3Present = 3;
					int32_t temp = std::stoi(buffer.substr(buffer.find_first_not_of('.') + 2));
					if(temp >= 18)
						zenity3Present = 5;
					else if(temp >= 10)
						zenity3Present = 4;
				}
				else if((std::stoi(buffer) == 2) && (std::stoi(buffer.substr(buffer.find_first_not_of('.') + 2)) >= 32))
					zenity3Present = 2;
			}
			pclose(in);
		}
	}
	
	return GraphicMode() ? zenity3Present : 0;
}

//-------------------------------------------------------------------------------------------------------------------//

bool MateDialogPresent()
{
	static int32_t matedialogPresent = -1;
	
	if(matedialogPresent < 0)
		matedialogPresent = DetectPresence("matedialog");
		
	return matedialogPresent && GraphicMode();
}

//-------------------------------------------------------------------------------------------------------------------//

bool ShellementaryPresent()
{
	static int32_t shellementaryPresent = -1;
	
	if(shellementaryPresent < 0)
		shellementaryPresent = DetectPresence("shellementary");
		
	return shellementaryPresent && GraphicMode();
}

//-------------------------------------------------------------------------------------------------------------------//

bool QarmaPresent()
{
	static int32_t qarmaPresent = -1;
	
	if(qarmaPresent < 0)
		qarmaPresent = DetectPresence("qarma");
		
	return qarmaPresent && GraphicMode();
}

//-------------------------------------------------------------------------------------------------------------------//

bool YadPresent()
{
	static int32_t yadPresent = -1;
	
	if(yadPresent < 0)
		yadPresent = DetectPresence("yad");
		
	return yadPresent && GraphicMode();
}

//-------------------------------------------------------------------------------------------------------------------//

bool Python3Present()
{
	static int32_t python3Present = -1;
	
	if(python3Present < 0)
	{
		python3Present = 0;
		Python3Name = "python3";
		if(DetectPresence(Python3Name))
			python3Present = 1;
		else
		{
			for(uint32_t i = 9; i >= 0; i--)
			{
				Python3Name = "python3." + std::to_string(i);
				if(DetectPresence(Python3Name))
				{
					python3Present = 1;
					break;
				}
			}
		}
	}
	
	return python3Present;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TryCommand(const std::string& command)
{
	std::string buffer;
	buffer.resize(MaxPathOrCMD);
	FILE* in;
	
	in = popen(command.data(), "r");
	if(fgets(buffer.data(), buffer.size(), in) == nullptr)
	{
		pclose(in);
		return true;
	}
	
	pclose(in);
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TKinter3Present()
{
	static int32_t tkinter3Present = -1;
	std::string pythonCommand;
	std::string pythonParams = "-S -c \"try:\n\timport tkinter;\nexcept:\n\tprint(0);\"";
	
	if(tkinter3Present < 0)
	{
		tkinter3Present = 0;
		if(Python3Present())
		{
			pythonCommand = Python3Name + " " + pythonParams;
			tkinter3Present = TryCommand(pythonCommand);
		}
	}
	
	return tkinter3Present && GraphicMode() && !IsDarwin();
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t KDialogPresent()
{
	static int32_t kdialogPresent = -1;
	std::string buffer;
	buffer.resize(MaxPathOrCMD);
	FILE* in;
	std::string desktop;
	
	if(kdialogPresent < 0)
	{
		if(ZenityPresent())
		{
			auto desktopEnv = std::getenv("XDG_SESSION_DESKTOP");
			if(!desktopEnv)
			{
				desktopEnv = std::getenv("XDG_CURRENT_DESKTOP");
				if(!desktopEnv)
				{
					desktopEnv = std::getenv("DESKTOP_SESSION");
					
					if(!desktopEnv)
					{
						kdialogPresent = 0;
						return kdialogPresent;
					}
				}
			}
			desktop = std::string(desktopEnv);
			if(desktop.empty() || ((desktop != "KDE" || desktop != "kde") && (desktop != "lxqt" || desktop != "LXQT")))
			{
				kdialogPresent = 0;
				return kdialogPresent;
			}
		}

		kdialogPresent = DetectPresence("kdialog");
		if(kdialogPresent)
		{
			in = popen("kdialog --attach 2>&1", "r");
			if(fgets(buffer.data(), buffer.size(), in) != nullptr)
			{
				if (buffer.find("Unknown") == std::string::npos)
					kdialogPresent = 2;
			}
			pclose(in);
		}
	}

	return GraphicMode() ? kdialogPresent : 0;
}

//-------------------------------------------------------------------------------------------------------------------//

bool FileExists(const std::string& filePathAndName)
{
	FILE* in;
	
	if(filePathAndName.empty())
		return false;
		
	in = fopen(filePathAndName.data(), "r");
	if(!in)
		return false;
		
	fclose(in);
	return true;
}

#endif

//-------------------------------------------------------------------------------------------------------------------//

std::string GetPathWithoutFinalSlash(const std::string& source)
{
	std::string result;

	if (!source.empty())
	{
		std::size_t index = source.find_last_of('/');
		if (index == std::string_view::npos)
			index = source.find_last_of('\\');
		if (index != std::string_view::npos)
			result = source.substr(0, index);
		else
			return {};
	}
	else
		return {};

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string GetLastName(const std::string& source)
{
	std::string result;
	
	if (!source.empty())
	{
		std::size_t index = source.find_last_of('/');
		if (index == std::string_view::npos)
			index = source.find_last_of('\\');
		if (index != std::string_view::npos)
			result = source.substr(index + 1);
		else
			return source;
	}
	else
		return {};

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

bool FilenameValid(const std::string_view filenameWithoutPath)
{
	if (filenameWithoutPath.empty())
		return false;

	for (const char& c : filenameWithoutPath)
	{
		if (c == '\\' || c == '/' || c == ':' || c == '*' || c == '?' ||
			c == '\"' || c == '<' || c == '>' || c == '|')
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool QuoteDetected(const std::string_view str)
{
	if (str.empty())
		return false;

	if (str.find_first_of('\'') != std::string_view::npos || str.find_first_of('\"') != std::string_view::npos)
		return true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string SaveFile(const std::string& title,
                     const std::string& defaultPathAndFile,
                     const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                     const bool allFiles)
{
	static std::string buffer{};

	if (QuoteDetected(title))
		return SaveFile("INVALID TITLE WITH QUOTES", defaultPathAndFile, filterPatterns, allFiles);
	if (QuoteDetected(defaultPathAndFile))
		return SaveFile(title, "INVALID DEFAULT_PATH WITH QUOTES", filterPatterns, allFiles);
	for(const auto& filterPattern : filterPatterns)
	{
		if (QuoteDetected(filterPattern.first) || QuoteDetected(filterPattern.second))
			return SaveFile("INVALID FILTER_PATTERN WITH QUOTES", defaultPathAndFile, {}, allFiles);
	}

	std::string path;
#ifdef _WIN32
	path = SaveFileWinGUI(title, defaultPathAndFile, filterPatterns, allFiles);
	buffer = path;
#else
	std::string dialogString;
	if(KDialogPresent())
	{
		dialogString = "kdialog";
		if (KDialogPresent() == 2 && XPropPresent())
			dialogString += " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)";
		dialogString += " --getsavefilename ";

		if (!defaultPathAndFile.empty())
		{
			if (defaultPathAndFile[0] != '/')
				dialogString += "$PWD/";
			dialogString += "\"" + defaultPathAndFile + "\"";
		}
		else
			dialogString += "$PWD/";

		if(!filterPatterns.empty())
		{
			dialogString += " \"";
			for(uint32_t i = 0; i < filterPatterns.size(); i++)
			{
				if(filterPatterns[i].second.find(';') == std::string::npos)
					dialogString += filterPatterns[i].first + " (" + filterPatterns[i].second + ")\n";
				else
				{
					std::string extensions = filterPatterns[i].second;
					std::replace(extensions.begin(), extensions.end(), ';', ' ');
					dialogString += filterPatterns[i].first + " (" + extensions + ")\n";
				}
			}
		}
		if(allFiles && filterPatterns.empty())
			dialogString += "\"All Files (*.*)\"";
		else if(allFiles && !filterPatterns.empty())
			dialogString += "All Files (*.*)\"";
		else if(!allFiles && !filterPatterns.empty())
		{
			dialogString.pop_back();
			dialogString += "\"";
		}
		if(!title.empty())
			dialogString += " --title \"" + title + "\"";
	}
	else if(ZenityPresent() || MateDialogPresent() || ShellementaryPresent() || QarmaPresent())
	{
		if(ZenityPresent())
		{
			dialogString = "zenity ";
			if(Zenity3Present() >= 4 && XPropPresent())
				dialogString += " --attach=$(sleep .01;xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)";
		}
		else if(MateDialogPresent())
			dialogString = "matedialog";
		else if(ShellementaryPresent())
			dialogString = "shellementary";
		else
		{
			dialogString = "qarma";
			if(XPropPresent())
				dialogString += " --attach$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)";
		}
		dialogString += " --file-selection --save --confirm-overwrite";
		
		if(!title.empty())
			dialogString += " --title=\"" + title + "\"";
		if(!defaultPathAndFile.empty())
			dialogString += " --filename=\"" + defaultPathAndFile + "\"";
		if(!filterPatterns.empty())
		{
			for(uint32_t i = 0; i < filterPatterns.size(); i++)
			{
				if(filterPatterns[i].second.find(';') == std::string::npos)
					dialogString += " --file-filter='" + filterPatterns[i].first + " | " + filterPatterns[i].second + "'";
				else
				{
					std::string extensions = filterPatterns[i].second;
					int32_t index = 0;
					while((index = extensions.find(';')) != std::string::npos)
						extensions.replace(index, 1, " | ");
					dialogString += " --file-filter='" + filterPatterns[i].first + " | " + extensions + "'";
				}
			}
		}
		if(allFiles)
			dialogString += " --file-filter='All Files | *'";
		dialogString += " 2>/dev/null ";
	}
	else if(YadPresent())
	{
		dialogString = "yad --file-selection --save --confirm-overwrite";
		if(!title.empty())
			dialogString += " --title=\"" + title + "\"";
		if(!defaultPathAndFile.empty())
			dialogString += " --filename=\"" + defaultPathAndFile + "\"";
		if(!filterPatterns.empty())
		{
			for(uint32_t i = 0; i < filterPatterns.size(); i++)
			{
				if(filterPatterns[i].second.find(';') == std::string::npos)
					dialogString += " --file-filter='" + filterPatterns[i].first + " | " + filterPatterns[i].second + "'";
				else
				{
					std::string extensions = filterPatterns[i].second;
					int32_t index = 0;
					while((index = extensions.find(';')) != std::string::npos)
						extensions.replace(index, 1, " | "); 
					dialogString += " --file-filter='" + filterPatterns[i].first + " | " + extensions + "'";
				}
			}
		}
		if(allFiles)
			dialogString += " --file-filter='All Files | *'";
		dialogString += " 2>/dev/null ";
	}
	else if(TKinter3Present())
	{
		dialogString = Python3Name + " -S -c \"import tkinter;from tkinter import filedialog;root=tkinter.Tk();root.withdraw();";
		dialogString += "res=filedialog.asksaveasfilename(";
		if(!title.empty())
			dialogString += "title='" + title + "',";
		if(!defaultPathAndFile.empty())
		{
			std::string str = GetPathWithoutFinalSlash(defaultPathAndFile);
			if(!str.empty())
				dialogString += "initialdir='" + str + "',";
			str = GetLastName(defaultPathAndFile);
			if(!str.empty())
				dialogString += "initialfile='" + str + "',";
		}
		if(!filterPatterns.empty() && filterPatterns[0].second[filterPatterns[0].second.size() - 1] != '*')
		{
			dialogString += "filetypes=(";
			for(uint32_t i = 0; i < filterPatterns.size(); i++)
			{
				if(filterPatterns[i].second.find(';') == std::string::npos)
					dialogString += "('" + filterPatterns[i].first + "',('" + filterPatterns[i].second + "',)),";
				else
				{
					std::string extensions = filterPatterns[i].second;
					int32_t index = 0;
					while((index = extensions.find(';')) != std::string::npos)
						extensions.replace(index, 1, "','");
					dialogString += "('" + filterPatterns[i].first + "',('" + extensions + "',)),";
				}
			}
		}
		if(allFiles && !filterPatterns.empty())
			dialogString += "('All Files','*'))";
		else if(!allFiles && !filterPatterns.empty())
			dialogString += ")";
		dialogString += ");\nif not isinstance(res, tuple):\n\tprint(res)\n\"";
	}

	FILE* in;
	if(!(in = popen(dialogString.data(), "r")))
		return {};
	buffer.resize(MaxPathOrCMD);
	while (fgets(buffer.data(), buffer.size(), in) != nullptr){}
	pclose(in);
	if (buffer[buffer.length() - 1] == '\n')
		buffer.pop_back();
	if (buffer.empty())
		return {};
	path = buffer;
#endif

	if (path.empty())
		return {};
	std::string str = GetPathWithoutFinalSlash(path);
	if (str.empty() && !DirExists(str))
		return {};
	str = GetLastName(path);
	if (!FilenameValid(str))
		return {};
	
	return path;
}

//-------------------------------------------------------------------------------------------------------------------//
#include <iostream>
std::vector<std::string> OpenFile(const std::string& title,
                                  const std::string& defaultPathAndFile,
                                  const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                                  const bool allowMultipleSelects,
                                  const bool allFiles)
{
	if (QuoteDetected(title))
		return OpenFile("INVALID TITLE WITH QUOTES", defaultPathAndFile, filterPatterns, allowMultipleSelects, allFiles);
	if (QuoteDetected(defaultPathAndFile))
		return OpenFile(title, "INVALID DEFAULT_PATH WITH QUOTES", filterPatterns, allowMultipleSelects, allFiles);
	for(const auto& filter : filterPatterns)
	{
		if (QuoteDetected(filter.first) || QuoteDetected(filter.second))
			return OpenFile("INVALID FILTER_PATTERN WITH QUOTES", defaultPathAndFile, {}, allowMultipleSelects, allFiles);
	}

	std::vector<std::string> paths;
#ifdef _WIN32
	paths = OpenFileWinGUI(title, defaultPathAndFile, filterPatterns, allowMultipleSelects, allFiles);
#else
	std::string dialogString;
	bool wasKDialog = false;
	if(KDialogPresent())
	{
		wasKDialog = true;
		dialogString = "kdialog ";
		if(KDialogPresent() == 2 && XPropPresent())
			dialogString += " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)";
		dialogString += " --getopenfilename ";
		if(allowMultipleSelects)
			dialogString += "--multiple --separate-output ";
		if(!defaultPathAndFile.empty())
		{
			if(defaultPathAndFile[0] != '/')
				dialogString += "$PWD/";
			dialogString += "\"" + defaultPathAndFile + "\"";
		}
		else
			dialogString += "$PWD/";
			
		if(!filterPatterns.empty())
		{
			dialogString += " \"";
			for(uint32_t i = 0; i < filterPatterns.size(); i++)
			{
				if(filterPatterns[i].second.find(';') == std::string::npos)
					dialogString += filterPatterns[i].first + " (" + filterPatterns[i].second + ")\n";
				else
				{
					std::string extensions = filterPatterns[i].second;
					std::replace(extensions.begin(), extensions.end(), ';', ' ');
					dialogString += filterPatterns[i].first + " (" + extensions + ")\n";
				}
			}
		}
		if(allFiles && filterPatterns.empty())
			dialogString += "\"All Files (*.*)\"";
		else if(allFiles && !filterPatterns.empty())
			dialogString += "All Files (*.*)\"";
		else if(!allFiles && !filterPatterns.empty())
		{
			dialogString.pop_back();
			dialogString += "\"";
		}
		if(!title.empty())
			dialogString += " --title \"" + title + "\"";
	}
	else if(ZenityPresent() || MateDialogPresent() || ShellementaryPresent() || QarmaPresent())
	{
		if(ZenityPresent())
		{
			dialogString = "zenity";
			if(Zenity3Present() >= 4 && XPropPresent())
				dialogString += " --attach=$(sleep .01;xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)";
		}
		else if(MateDialogPresent())
			dialogString = "matedialog";
		else if(ShellementaryPresent())
			dialogString = "shellementary";
		else
		{
			dialogString = "qarma";
			if(XPropPresent())
				dialogString += " --attach=$(xprop --root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)";
		}
		dialogString += " --file-selection";
		
		if(allowMultipleSelects)
			dialogString += " --multiple";
		if(!title.empty())
			dialogString += " --title=\"" + title + "\"";
		if(!defaultPathAndFile.empty())
			dialogString += " --filename=\"" + defaultPathAndFile + "\"";
		if(!filterPatterns.empty())
		{
			for(uint32_t i = 0; i < filterPatterns.size(); i++)
			{
				if(filterPatterns[i].second.find(';') == std::string::npos)
					dialogString += " --file-filter='" + filterPatterns[i].first + " | " + filterPatterns[i].second + "'";
				else
				{
					std::string extensions = filterPatterns[i].second;
					int32_t index = 0;
					while((index = extensions.find(';')) != std::string::npos)
						extensions.replace(index, 1, " | ");
					dialogString += " --file-filter='" + filterPatterns[i].first + " | " + extensions + "'";
				}
			}
		}
		if(allFiles)
			dialogString += " --file-filter='All Files | *'";
		dialogString += " 2>/dev/null ";
	}
	else if(YadPresent())
	{
		dialogString = "yad --file-selection";
		if(allowMultipleSelects)
			dialogString += " --multiple";
		if(!title.empty())
			dialogString += " --title=\"" + title + "\"";
		if(!defaultPathAndFile.empty())
			dialogString += " --filename=\"" + defaultPathAndFile + "\"";
		if(!filterPatterns.empty())
		{
			for(uint32_t i = 0; i < filterPatterns.size(); i++)
			{
				if(filterPatterns[i].second.find(';') == std::string::npos)
					dialogString += " --file-filter='" + filterPatterns[i].first + " | " + filterPatterns[i].second + "'";
				else
				{
					std::string extensions = filterPatterns[i].second;
					int32_t index = 0;
					while((index = extensions.find(';')) != std::string::npos)
						extensions.replace(index, 1, " | "); 
					dialogString += " --file-filter='" + filterPatterns[i].first + " | " + extensions + "'";
				}
			}
		}
		if(allFiles)
			dialogString += " --file-filter='All Files | *'";
		dialogString += " 2>/dev/null ";
	}
	else if(TKinter3Present())
	{
		dialogString = Python3Name + " -S -c \"import tkinter;from tkinter import filedialog;root=tkinter.Tk();root.withdraw();";
		dialogString += "lFiles=filedialog.askopenfilename(";
		if(allowMultipleSelects)
			dialogString += "multiple=1,";
		if(!title.empty())
			dialogString += "title='" + title + "',";
		if(!defaultPathAndFile.empty())
		{
			std::string tmp;
			tmp = GetPathWithoutFinalSlash(defaultPathAndFile);
			if(!tmp.empty())
				dialogString += "initialdir='" + tmp + "',";
			tmp = GetLastName(defaultPathAndFile);
			if(!tmp.empty())
				dialogString += "initialfile='" + tmp + "',";
		}
		if(!filterPatterns.empty() && filterPatterns[0].second[filterPatterns[0].second.size() - 1] != '*')
		{
			dialogString += "filetypes=(";
			for(uint32_t i = 0; i < filterPatterns.size(); i++)
			{
				if(filterPatterns[i].second.find(';') == std::string::npos)
					dialogString += "('" + filterPatterns[i].first + "',('" + filterPatterns[i].second + "',)),";
				else
				{
					std::string extensions = filterPatterns[i].second;
					int32_t index = 0;
					while((index = extensions.find(';')) != std::string::npos)
						extensions.replace(index, 1, "','");
					dialogString += "('" + filterPatterns[i].first + "',('" + extensions + "',)),";
				}
			}
		}
		if(allFiles && !filterPatterns.empty())
			dialogString += "('All Files','*'))";
		else if(!allFiles && !filterPatterns.empty())
			dialogString += ")";
		dialogString += ");\nif not isinstance(lFiles, tuple):\n\tprint(lFiles)\nelse:\n\tlFilesString=''\n\t";
		dialogString += "for lFile in lFiles:\n\t\tlFilesString+=str(lFile)+'|'\n\tprint(lFilesString[:-1])\n\"";
	}
	
	std::string buffer;
	if (allowMultipleSelects)
		buffer.resize(MaxMultipleFiles * MaxPathOrCMD + 1);
	else
		buffer.resize(MaxPathOrCMD + 1);
	
	FILE* in;
	if(!(in = popen(dialogString.data(), "r")))
	{
		buffer = {};
		return {};
	}
	char* data = buffer.data();
	while(fgets(data, &buffer[buffer.size() - 1] - data, in) != nullptr)
	{
		uint64_t off = 0;
		for(const char& c : buffer)
		{
			if(c == '\0')
				break;
			off++;
		}
		data = buffer.data() + off;
	}
	pclose(in);
	uint32_t off = 0;
	for(const auto& c : buffer)
	{
		if(c == '\0')
			break;
		off++;
	}
	buffer.resize(off);
	
	if(buffer[buffer.size() - 1] == '\n')
		buffer[buffer.size() - 1] = '\0';
		
	if(wasKDialog && allowMultipleSelects)
	{
		buffer += '\n';
		std::size_t pos = 0;
		while((pos = buffer.find('\n')) != std::string::npos)
		{
			std::string token = buffer.substr(0, pos);
			paths.push_back(token);
			buffer.erase(0, pos + 1);
		}
	}
	else if(allowMultipleSelects)
	{
		buffer += '|';
		std::size_t pos = 0;
		while((pos = buffer.find('|')) != std::string::npos)
		{
			std::string token = buffer.substr(0, pos);
			paths.push_back(token);
			buffer.erase(0, pos + 1);
		}
	}
	else if(!allowMultipleSelects)
		paths.push_back(buffer);
#endif

	if (paths.empty())
		return {};
	if (allowMultipleSelects && paths.size() > 1)
	{
		for(auto it = paths.begin(); it != paths.end(); it++)
		{
			if(!FileExists(*it))
				it = paths.erase(it);
		}
	}
	else if (!FileExists(paths[0]))
		return {};
	
	return paths;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string OpenSingleFile(const std::string& title,
                           const std::string& defaultPathAndFile,
                           const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                           const bool allFiles)
{
	std::vector<std::string> path = OpenFile(title, defaultPathAndFile, filterPatterns, false, allFiles);
	return path.empty() ? std::string() : path[0];
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> OpenMultipleFiles(const std::string& title,
                              const std::string& defaultPathAndFile,
                              const std::vector<std::pair<std::string, std::string>>& filterPatterns,
                              const bool allFiles)
{
	const std::vector<std::string> paths = OpenFile(title, defaultPathAndFile, filterPatterns, true, allFiles);
	return paths.empty() ? std::vector<std::string>() : paths;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string SelectFolder(const std::string& title, const std::string& defaultPath)
{
	static std::string buffer;

	if (QuoteDetected(title))
		return SelectFolder("INVALID TITLE WITH QUOTES", defaultPath);
	if (QuoteDetected(defaultPath))
		return SelectFolder(title, "INVALID DEFAULT_PATH WITH QUOTES");

	std::string path;
#ifdef _WIN32
	path = SelectFolderWinGUI(title, defaultPath);
	buffer = path;
#else
	//TODO Linux
	buffer.resize(MaxPathOrCMD);
	std::string dialogString;
	if(KDialogPresent())
	{
		dialogString = "kdialog";
		if(KDialogPresent() == 2 && XPropPresent())
			dialogString += " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)";
		dialogString += " --getexistingdirectory ";
		
		if(!defaultPath.empty())
		{
			if(defaultPath[0] != '/')
				dialogString += "$PWD/";
			dialogString += "\"" + defaultPath + "\"";
		}
		else
			dialogString += "$PWD/";
			
		if(!title.empty())
			dialogString += " --title=\"" + title + "\"";
	}
	else if(ZenityPresent() || MateDialogPresent() || ShellementaryPresent() || QarmaPresent())
	{
		if(ZenityPresent())
		{
			dialogString = "zenity";
			if(Zenity3Present() >= 4 && XPropPresent())
				dialogString += " --attach=$(sleep .01;xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)";
		}
		else if(MateDialogPresent())
			dialogString = "matedialog";
		else if(ShellementaryPresent())
			dialogString = "shellementary";
		else
		{
			dialogString = "qarma";
			if(XPropPresent())
				dialogString += " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)";
		}
		dialogString += " --file-selection --directory";
		
		if(!title.empty())
			dialogString += " --title=\"" + title + "\"";
		if(!defaultPath.empty())
			dialogString += " --filename=\"" + defaultPath + "\"";
		dialogString += " 2>/dev/null ";
	}
	else if(YadPresent())
	{
		dialogString = "yad --file-selection --directory";
		if(!title.empty())
			dialogString += " --title=\"" + title + "\"";
		if(!defaultPath.empty())
			dialogString += " --filename=\"" + defaultPath + "\"";
		dialogString += " 2>/dev/null ";
	}
	else if(TKinter3Present())
	{
		dialogString = Python3Name;
		dialogString += " -S -c \"import tkinter;from tkinter import filedialog;root=tkinter.Tk();root.withdraw();";
		dialogString += "res=filedialog.askdirectory(";
		if(!title.empty())
			dialogString += "title='" + title + "',";
		if(!defaultPath.empty())
			dialogString += "initialdir='" + defaultPath + "'";
		dialogString += ");\nif not isinstance(res, tuple):\n\tprint(res)\n\"";
	}
	
	FILE* in;
	if(!(in = popen(dialogString.data(), "r")))
		return {};
	while(fgets(buffer.data(), buffer.size(), in) != nullptr)
	{}
	uint32_t off = 0;
	for(const auto& c : buffer)
	{
		if(c == '\0')
			break;
		off++;
	}
	buffer.resize(off);
	
	if(buffer[buffer.size() - 1] == '\n')
		buffer[buffer.size() - 1] = '\0';
		
	if(!DirExists(buffer))
		return {};
	path = buffer;
#endif

	if (path.empty())
		return {};

	return path;
}