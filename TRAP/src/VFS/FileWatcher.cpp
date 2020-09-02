#include "TRAPPCH.h"
#include "FileWatcher.h"

#include "Utils/String/String.h"
#include "VFS.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileWatcher::FileWatcher(std::string virtualPath, const float updateTimeInMilliseconds)
	: m_virtualPathToWatch(std::move(virtualPath)), m_delay(updateTimeInMilliseconds)
{
	TP_PROFILE_FUNCTION();

	if (m_delay < 15.0f)
		m_delay = 15.0f; //Prevent issues with programs that first delete a file and then recreate it

	m_timer.Reset();
	const std::string virtualPathLower = Utils::String::ToLower(m_virtualPathToWatch);
	m_physicalPathsToWatch = VFS::ResolveToPhysicalPaths(virtualPathLower);

	for (const auto& path : m_physicalPathsToWatch)
	{
		if (VFS::FileOrFolderExists(path, true))
		{
			for (const auto& file : std::filesystem::recursive_directory_iterator(path))
			{
				if (!std::filesystem::is_directory(file))
				{
					std::string formattedPath = PhysicalFilePathFormatter(file, path);
					m_physicalPaths[formattedPath] = VFS::GetLastWriteTime(formattedPath);
					m_virtualPaths[formattedPath] = VirtualFilePathFormatter(virtualPathLower, file, path);
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileWatcher::Check(const std::function<void(std::filesystem::path, std::string, FileStatus)>& action)
{
	TP_PROFILE_FUNCTION();

	//Check if Files have changed
	if (m_timer.ElapsedMilliseconds() >= m_delay)
	{
		auto it = m_physicalPaths.begin();
		while (it != m_physicalPaths.end())
		{
			if (!VFS::FileOrFolderExists(it->first, true))
			{
				auto virtualIt = m_virtualPaths.find(it->first);
				if (virtualIt != m_virtualPaths.end())
				{
					//Found						
					action(std::filesystem::path(it->first), virtualIt->second, FileStatus::Erased);
					it = m_physicalPaths.erase(it);
					virtualIt = m_virtualPaths.erase(virtualIt);
				}
				else
				{
					action(std::filesystem::path(it->first), "", FileStatus::Erased);
					it = m_physicalPaths.erase(it);
				}
			}
			else
				++it;
		}

		//Check if a file was created or modified
		for (const auto& path : m_physicalPathsToWatch)
		{
			if (VFS::FileOrFolderExists(path, true))
			{
				for (const auto& file : std::filesystem::recursive_directory_iterator(path))
				{
					std::string formattedPath = PhysicalFilePathFormatter(file, path);
					auto currentFileLastWriteTime = VFS::GetLastWriteTime(formattedPath);
					if (currentFileLastWriteTime != std::filesystem::file_time_type::min())
					{
						//File creation
						if (m_physicalPaths.find(formattedPath) == m_physicalPaths.end())
						{
							m_physicalPaths[formattedPath] = currentFileLastWriteTime;
							m_virtualPaths[formattedPath] = VirtualFilePathFormatter(Utils::String::ToLower(m_virtualPathToWatch), file, path);
							action(formattedPath, m_virtualPaths[formattedPath], FileStatus::Created);
						}
						else //File modification
						{
							if (m_physicalPaths[formattedPath] != currentFileLastWriteTime)
							{
								m_physicalPaths[formattedPath] = currentFileLastWriteTime;
								action(formattedPath, m_virtualPaths[formattedPath], FileStatus::Modified);
							}
						}
					}
					else
					{
						//File erased
						const auto virtualIt = m_virtualPaths.find(formattedPath);
						action(std::filesystem::path(formattedPath), virtualIt->second, FileStatus::Erased);
						m_physicalPaths.erase(formattedPath);
						m_virtualPaths.erase(virtualIt);
					}
				}
			}
		}

		m_timer.Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::FileWatcher::VirtualFilePathFormatter(const std::string& virtualPath,
                                                    const std::filesystem::directory_entry& physicalFilePath,
                                                    const std::filesystem::path& physicalFolderPath) const
{
	std::string result = TRAP::Utils::String::ToLower(virtualPath) + '/';
	std::filesystem::path temp = physicalFilePath.path();
	temp.make_preferred();
#ifdef TRAP_PLATFORM_WINDOWS
	std::vector<std::string> dirs = Utils::String::SplitString(temp.string(), "\\");
#else
	std::vector<std::string> dirs = Utils::String::SplitString(temp.string(), '/');
#endif
	for (uint32_t i = Utils::String::GetCount(physicalFolderPath.string(), '/') + 1; i < dirs.size() - 1; i++)
		result += dirs[i] + '/';
	TRAP::Utils::String::ToLower(result);
	result += dirs[dirs.size() - 1];

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::FileWatcher::PhysicalFilePathFormatter(const std::filesystem::directory_entry& physicalFilePath,
                                                         const std::filesystem::path& physicalPath) const
{
	std::string result = physicalPath.string() + '/';
	std::filesystem::path temp = physicalFilePath.path();
	temp.make_preferred();
#ifdef TRAP_PLATFORM_WINDOWS
	std::vector<std::string> dirs = Utils::String::SplitString(temp.string(), "\\");
#else
	std::vector<std::string> dirs = Utils::String::SplitString(temp.string(), '/');
#endif
	for (uint32_t i = Utils::String::GetCount(physicalPath.string(), '/') + 1; i < dirs.size(); i++)
		result += dirs[i] + '/';
	result.pop_back();

	return result;
}