#include "TRAPPCH.h"
#include "FileWatcher.h"

namespace TRAP
{
	FileWatcher::FileWatcher(const std::string& virtualPath, float updateTimeInMilliseconds)
		: m_virtualPathToWatch(virtualPath), m_delay(updateTimeInMilliseconds)
	{
		m_timer.Reset();
		const std::string virtualPathLower = Utils::ToLower(virtualPath);
		m_physicalPathsToWatch = VFS::Get()->ResolveToPhysicalPaths(virtualPathLower);

		for (const auto& path : m_physicalPathsToWatch)
		{
			if (!FileSystem::SilentFileOrFolderExists(path))
			{
				for (const auto& file : std::filesystem::recursive_directory_iterator(path))
				{
					m_physicalPaths[file.path().string()] = FileSystem::GetLastWriteTime(file);
					m_virtualPaths[file.path().string()] = virtualPathLower + '/' + file.path().filename().string();
				}
			}
		}
	}

	void FileWatcher::Start(const std::function<void(std::filesystem::path, std::string, FileStatus)>& action)
	{
		//Check if Files have changed
		if (m_timer.ElapsedMilliseconds() >= m_delay)
		{
			auto it = m_physicalPaths.begin();
			while (it != m_physicalPaths.end())
			{
				if (FileSystem::SilentFileOrFolderExists(it->first))
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
				if (!FileSystem::SilentFileOrFolderExists(path))
				{
					for (const auto& file : std::filesystem::recursive_directory_iterator(path))
					{
						auto currentFileLastWriteTime = FileSystem::GetLastWriteTime(file);

						//File creation
						if (m_physicalPaths.find(file.path().string()) == m_physicalPaths.end())
						{
							m_physicalPaths[file.path().string()] = currentFileLastWriteTime;
							action(file.path(), m_virtualPaths[file.path().string()], FileStatus::Created);
						}
						else //File modification
						{
							if (m_physicalPaths[file.path().string()] != currentFileLastWriteTime)
							{
								m_physicalPaths[file.path().string()] = currentFileLastWriteTime;
								action(file.path(), m_virtualPaths[file.path().string()], FileStatus::Modified);
							}
						}
					}
				}
			}

			m_timer.Reset();
		}
	}
}