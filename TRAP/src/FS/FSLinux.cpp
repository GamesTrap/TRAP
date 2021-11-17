// The MIT License (MIT)

// Copyright (c) 2015 Poul Sander

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//Modified by Jan "GamesTrap" Schuerkamp

#include "TRAPPCH.h"
#include "FS.h"

#ifdef TRAP_PLATFORM_LINUX
std::string TRAP::FS::GetHomeFolderPathLinux()
{
    static std::string homeDir{};

    if(!homeDir.empty())
        return homeDir;

    int uid = getuid();
    const char* homeEnv = std::getenv("HOME");
    if(uid != 0 && homeEnv)
    {
        //We only acknowledge HOME if not root.
        homeDir = homeEnv;
        return homeDir;
    }

    passwd* pw = nullptr;
    passwd pwd;
    int32_t bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if(bufSize < 0)
        bufSize = 16384;
    std::vector<char> buffer{};
    buffer.resize(bufSize);
    int32_t errorCode = getpwuid_r(uid, &pwd, buffer.data(), buffer.size(), &pw);
    if(errorCode)
    {
        TP_ERROR(Log::FileSystemPrefix, "Failed to get home folder path (", errorCode, ")");
        return {};
    }
    const char* tempRes = pw->pw_dir;
    if(!tempRes)
    {
        TP_ERROR(Log::FileSystemPrefix, "Failed to get home folder path (", errorCode, ")");
        return {};
    }
    homeDir = tempRes;

    return homeDir;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::FS::GetDocumentsFolderPathLinux()
{
    static std::string documentsDir{};

    if(!documentsDir.empty())
        return documentsDir;

    documentsDir = "$HOME/Documents";

    //Get config folder
    const char* tempRes = std::getenv("XDG_CONFIG_HOME");
    std::string configPath{};
    if(tempRes)
        configPath = tempRes;
    else
        configPath = GetHomeFolderPathLinux() + "/.config";
    configPath += "/user-dirs.dirs";

    //Get Documents folder
    std::ifstream file(configPath);
    if(file.is_open() && file.good())
    {
        std::string line;
        while(std::getline(file, line))
        {
            //Skip invalid entries and comments
            if(line.empty() || line[0] == '#' || line.substr(0, 4) != "XDG_" || line.find("_DIR") == std::string::npos)
                continue;
            std::size_t splitPos = line.find('=');
            if(splitPos == std::string::npos)
                continue;
            std::string key = line.substr(0, splitPos);
            if(key != "XDG_DOCUMENTS_DIR") //Only interested in documents folder
                continue;
            std::size_t valueStart = line.find('"', splitPos);
            if(valueStart == std::string::npos)
                continue;
            std::size_t valueEnd = line.find('"', valueStart + 1);
            if(valueEnd == std::string::npos)
                continue;
            documentsDir = line.substr(valueStart + 1, valueEnd - valueStart - 1);
            break;
        }
    }
    else
    {
        TP_ERROR(Log::FileSystemPrefix, "Failed to get documents folder path");
        return {};
    }
    file.close();

    if(documentsDir.compare(0, 5, "$HOME") == 0)
        documentsDir = GetHomeFolderPathLinux() + documentsDir.substr(5, std::string::npos);
    TP_TRACE("DocumentsDir: ", documentsDir);

    //Finally
    return documentsDir;
}
#endif