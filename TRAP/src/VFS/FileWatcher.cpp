#include "TRAPPCH.h"
#include "FileWatcher.h"

#include "Utils/String/String.h"
#include "FileSystem.h"
#include "VFS.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileWatcher::FileWatcher(const std::string& virtualPath, const float updateTimeInMilliseconds)
	: m_virtualPathToWatch(virtualPath), m_delay(updateTimeInMilliseconds)
{
	TP_PROFILE_FUNCTION();

	static auto VirtualFilepathFormatter = [](const std::string& virtualPathLower, const std::filesystem::directory_entry& file,
	                            const std::filesystem::path& path)
	{
		std::string result = virtualPathLower + '/';
		std::filesystem::path temp = file.path();
		temp.make_preferred();
	#ifdef TRAP_PLATFORM_WINDOWS
		std::vector<std::string> dirs = Utils::String::SplitString(temp.string(), "\\");
	#else
		std::vector<std::string> dirs = Utils::String::SplitString(temp.string(), '/');
	#endif
		for (uint32_t i = Utils::String::GetCount(path.string(), '/') + 1; i < dirs.size(); i++)
			result += dirs[i] + '/';
		result.pop_back();

		return Utils::String::ToLower(result);
	};
	static auto FilepathFormatter = [](const std::filesystem::directory_entry& file,
		const std::filesystem::path& path)
	{
		std::string result = path.string() + '/';
		std::filesystem::path temp = file.path();
		temp.make_preferred();
#ifdef TRAP_PLATFORM_WINDOWS
		std::vector<std::string> dirs = Utils::String::SplitString(temp.string(), "\\");
#else
		std::vector<std::string> dirs = Utils::String::SplitString(temp.string(), '/');
#endif
		for (uint32_t i = Utils::String::GetCount(path.string(), '/') + 1; i < dirs.size(); i++)
			result += dirs[i] + '/';
		result.pop_back();

		return result;
	};

	m_timer.Reset();
	const std::string virtualPathLower = Utils::String::ToLower(virtualPath);
	m_physicalPathsToWatch = VFS::ResolveToPhysicalPaths(virtualPathLower);

	for (const auto& path : m_physicalPathsToWatch)
	{
		if (FileSystem::SilentFileOrFolderExists(path))
		{
			for (const auto& file : std::filesystem::recursive_directory_iterator(path))
			{
				if (!std::filesystem::is_directory(file))
				{
					std::string formattedPath = FilepathFormatter(file, path);
					m_physicalPaths[formattedPath] = FileSystem::GetLastWriteTime(formattedPath);
					m_virtualPaths[formattedPath] = VirtualFilepathFormatter(virtualPathLower, file, path);
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileWatcher::Check(const std::function<void(std::filesystem::path, std::string, FileStatus)>& action)
{
	static auto VirtualFilepathFormatter = [](const std::string& virtualPathLower, const std::filesystem::directory_entry& file,
		const std::filesystem::path& path)
	{
		std::string result = virtualPathLower + '/';
		std::filesystem::path temp = file.path();
		temp.make_preferred();
#ifdef TRAP_PLATFORM_WINDOWS
		std::vector<std::string> dirs = Utils::String::SplitString(temp.string(), "\\");
#else
		std::vector<std::string> dirs = Utils::String::SplitString(temp.string(), '/');
#endif
		for (uint32_t i = Utils::String::GetCount(path.string(), '/') + 1; i < dirs.size(); i++)
			result += dirs[i] + '/';
		result.pop_back();

		return Utils::String::ToLower(result);
	};
	static auto FilepathFormatter = [](const std::filesystem::directory_entry& file,
		const std::filesystem::path& path)
	{
		std::string result = path.string() + '/';
		std::filesystem::path temp = file.path();
		temp.make_preferred();
#ifdef TRAP_PLATFORM_WINDOWS
		std::vector<std::string> dirs = Utils::String::SplitString(temp.string(), "\\");
#else
		std::vector<std::string> dirs = Utils::String::SplitString(temp.string(), '/');
#endif
		for (uint32_t i = Utils::String::GetCount(path.string(), '/') + 1; i < dirs.size(); i++)
			result += dirs[i] + '/';
		result.pop_back();

		return result;
	};
	
	TP_PROFILE_FUNCTION();

	//Check if Files have changed
	if (m_timer.ElapsedMilliseconds() >= m_delay)
	{
		auto it = m_physicalPaths.begin();
		while (it != m_physicalPaths.end())
		{
			if (!FileSystem::SilentFileOrFolderExists(it->first))
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
			if (FileSystem::SilentFileOrFolderExists(path))
			{
				for (const auto& file : std::filesystem::recursive_directory_iterator(path))
				{
					std::string formattedPath = FilepathFormatter(file, path);
					auto currentFileLastWriteTime = FileSystem::GetLastWriteTime(formattedPath);
					if (currentFileLastWriteTime != std::filesystem::file_time_type::min())
					{
						//File creation
						if (m_physicalPaths.find(formattedPath) == m_physicalPaths.end())
						{
							m_physicalPaths[formattedPath] = currentFileLastWriteTime;
							m_virtualPaths[formattedPath] = VirtualFilepathFormatter(Utils::String::ToLower(m_virtualPathToWatch), file, path);
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