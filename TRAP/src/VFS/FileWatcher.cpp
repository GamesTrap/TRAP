#include "TRAPPCH.h"
#include "FileWatcher.h"

#include "Core/PlatformDetection.h"
#include "Utils/String/String.h"
#include "FS/FS.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileWatcher::FileWatcher(const std::string& virtualPath, const float updateTimeInMilliseconds)
	: m_virtualPathToWatch(Utils::String::ToLower(virtualPath)), m_delay(updateTimeInMilliseconds)
{
	// TP_PROFILE_FUNCTION();

	// if (m_delay < 500.0f)
	// 	m_delay = 500.0f; //Prevent issues with programs that first delete a file and then recreate it

	// m_timer.Reset();
	// m_physicalPathsToWatch = VFS::ResolveToPhysicalPaths(m_virtualPathToWatch);

	// for (const auto& path : m_physicalPathsToWatch)
	// {
	// 	if (!VFS::FileOrFolderExists(path, true))
	// 		continue;

	// 	for (const auto& file : std::filesystem::recursive_directory_iterator(path))
	// 	{
	// 		if (std::filesystem::is_directory(file))
	// 			continue;

	// 		std::string formattedPath = file.path().string();
	// 		m_physicalPaths[formattedPath] = VFS::GetLastWriteTime(formattedPath);
	// 		m_virtualPaths[formattedPath] = VirtualFilePathFormatter(m_virtualPathToWatch, file);
	// 	}
	// }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileWatcher::Check(const std::function<void(std::filesystem::path, std::string, FileStatus)>& action)
{
	// TP_PROFILE_FUNCTION();
	// static const std::thread::id mainID = TRAP::Application::GetMainThreadID();
	// static const std::thread::id thisID = std::this_thread::get_id();

	// if (thisID != mainID || m_timer.ElapsedMilliseconds() >= m_delay)
	// {
	// 	//Check if Files have changed
	// 	auto it = m_physicalPaths.begin();
	// 	while (it != m_physicalPaths.end())
	// 	{
	// 		if (!VFS::FileOrFolderExists(it->first, true))
	// 		{
	// 			auto virtualIt = m_virtualPaths.find(it->first);
	// 			if (virtualIt != m_virtualPaths.end())
	// 			{
	// 				//Found
	// 				action(std::filesystem::path(it->first), virtualIt->second, FileStatus::Erased);
	// 				it = m_physicalPaths.erase(it);
	// 				virtualIt = m_virtualPaths.erase(virtualIt);
	// 			}
	// 			else
	// 			{
	// 				action(std::filesystem::path(it->first), "", FileStatus::Erased);
	// 				it = m_physicalPaths.erase(it);
	// 			}
	// 		}
	// 		else
	// 			++it;
	// 	}

	// 	//Check if a file was created or modified
	// 	for (const auto& path : m_physicalPathsToWatch)
	// 	{
	// 		if (!VFS::FileOrFolderExists(path, true))
	// 			continue;

	// 		for (const auto& file : std::filesystem::recursive_directory_iterator(path))
	// 		{
	// 			if (std::filesystem::is_directory(file))
	// 				continue;

	// 			std::string formattedPath = file.path().string();
	// 			const auto currentFileLastWriteTime = VFS::GetLastWriteTime(formattedPath);
	// 			if (currentFileLastWriteTime != std::filesystem::file_time_type::min())
	// 			{
	// 				//File creation
	// 				if (m_physicalPaths.find(formattedPath) == m_physicalPaths.end())
	// 				{
	// 					m_physicalPaths[formattedPath] = currentFileLastWriteTime;
	// 					m_virtualPaths[formattedPath] = VirtualFilePathFormatter(m_virtualPathToWatch, file);
	// 					action(formattedPath, m_virtualPaths[formattedPath], FileStatus::Created);
	// 				}
	// 				else //File modification
	// 				{
	// 					if (m_physicalPaths[formattedPath] != currentFileLastWriteTime)
	// 					{
	// 						m_physicalPaths[formattedPath] = currentFileLastWriteTime;
	// 						action(formattedPath, m_virtualPaths[formattedPath], FileStatus::Modified);
	// 					}
	// 				}
	// 			}
	// 			else
	// 			{
	// 				//File erased
	// 				const auto virtualIt = m_virtualPaths.find(formattedPath);
	// 				action(std::filesystem::path(formattedPath), virtualIt->second, FileStatus::Erased);
	// 				m_physicalPaths.erase(formattedPath);
	// 				m_virtualPaths.erase(virtualIt);
	// 			}
	// 		}
	// 	}

	// 	//Check if a new folder needs to be tracked
	// 	m_physicalPathsToWatch = VFS::ResolveToPhysicalPaths(m_virtualPathToWatch);

	// 	for (const auto& path : m_physicalPathsToWatch)
	// 	{
	// 		if (!VFS::FileOrFolderExists(path, true))
	// 			continue;

	// 		for (const auto& file : std::filesystem::recursive_directory_iterator(path))
	// 		{
	// 			if (std::filesystem::is_directory(file))
	// 				continue;

	// 			const std::string formattedPath = file.path().string();
	// 			if(m_physicalPaths.find(formattedPath) == m_physicalPaths.end())
	// 			{
	// 				m_physicalPaths[formattedPath] = VFS::GetLastWriteTime(formattedPath);
	// 				m_virtualPaths[formattedPath] = VirtualFilePathFormatter(m_virtualPathToWatch, file);
	// 			}
	// 		}
	// 	}

	// 	if(thisID != mainID)
	// 		std::this_thread::sleep_for(std::chrono::milliseconds{ static_cast<int>(m_delay) });
	// 	else
	// 		m_timer.Reset();
	// }
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::FileWatcher::VirtualFilePathFormatter(const std::string& virtualPath,
                                                        const std::filesystem::directory_entry& physicalFilePath) const
{
	return "";
// 	std::string result = virtualPath + '/';
// 	std::filesystem::path temp = physicalFilePath.path();
// 	temp.make_preferred();
// #ifdef TRAP_PLATFORM_WINDOWS
// 	std::vector<std::string> dirs = Utils::String::SplitString(temp.string(), "\\");
// #else
// 	std::vector<std::string> dirs = Utils::String::SplitString(temp.string(), '/');
// #endif
// 	for (uint32_t i = Utils::String::GetCount(temp.parent_path().string(), '/') + 2; i < dirs.size() - 1; i++)
// 		result += Utils::String::ToLower(dirs[i]) + '/';
// 	result += dirs.back();

// 	return result;
}