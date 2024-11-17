#include "TRAPPCH.h"
#include "SafeSystem.h"

#include "Utils/String/String.h"

namespace
{
#ifdef TRAP_PLATFORM_LINUX
    [[nodiscard]] TRAP::Expected<void, std::string> SafeSystemLinux(const std::string& program,
                                                                    const std::vector<std::string>& args,
                                                                    const bool waitForChild)
    {
        ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

        //Prepare arguments fpr execvp
        std::vector<char*> argsCStr{};
        argsCStr.reserve(args.size() + 2u); //program + args + nullptr
        argsCStr.push_back(const_cast<char*>(program.c_str()));
        for(const auto& arg : args)
            argsCStr.push_back(const_cast<char*>(arg.c_str()));
        argsCStr.push_back(nullptr);

        //Fork a new process
        const pid_t pid = fork();
        if(pid == -1)
            return TRAP::MakeUnexpected("TRAP::Utils::SafeSystem(): Failed to fork a new process!");

        if(pid == 0)
        {
            //Child process: Execute the program
            execvp(program.c_str(), argsCStr.data());

            //If execvp returns, an error occurred
            return TRAP::MakeUnexpected(fmt::format("TRAP::Utils::SafeSystem(): Failed to execute {} ({})", program, TRAP::Utils::String::GetStrError()));
        }

        if(waitForChild)
        {
            //Parent process: Wait for the child process to complete
            i32 status = 0;
            if(waitpid(pid, &status, 0) == -1)
            {
                return TRAP::MakeUnexpected("TRAP::Utils::SafeSystem(): Failed to wait for child process!");
            }

            //Check if the child process exited successfully
            if(WIFEXITED(status))
            {
                const i32 exitStatus = WEXITSTATUS(status);
                if(exitStatus != 0)
                    return TRAP::MakeUnexpected(fmt::format("TRAP::Utils::SafeSystem(): Program exited with non-zero status: {}", exitStatus));
            }
            else
                return TRAP::MakeUnexpected("TRAP::Utils::SafeSystem(): Child process did not terminate normally!");
        }

        return {};
    }
#endif /*TRAP_PLATFORM_LINUX*/

    //-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_WINDOWS
    [[nodiscard]] TRAP::Expected<void, std::string> SafeSystemWindows(const std::string& program, const std::vector<std::string>& args, const bool waitForChild)
    {
        ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

        if (program.size() > MAX_PATH)
            return TRAP::MakeUnexpected(fmt::format("TRAP::Utils::SafeSystem(): program \"{}\" length is > than MAX_PATH ({})!", program, program.size()));

        std::string command = program;
        for (const auto& arg : args)
            command += fmt::format(" \"{}\"", arg); //Quote arguments to handle spaces

        std::wstring commandW = TRAP::Utils::String::CreateWideStringFromUTF8StringWin32(command);

        if (commandW.size() > std::numeric_limits<i16>::max())
            return TRAP::MakeUnexpected(fmt::format("TRAP::Utils::SafeSystem(): command string is > than 32767!"));

        //Initialize the process startup and process information structures
        STARTUPINFOW startupInfo{};
        startupInfo.cb = sizeof(STARTUPINFOW);
        PROCESS_INFORMATION processInfo{};

        //Create process
        const bool res = CreateProcessW(nullptr, commandW.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startupInfo, &processInfo);
        if (!res)
        {
            return TRAP::MakeUnexpected(fmt::format("TRAP::Utils::SafeSystem(): Failed to create process ({})!", program, TRAP::Utils::String::GetStrError()));
        }

        if (waitForChild)
        {
            //Wait for process to finish
            DWORD waitResult = WaitForSingleObject(processInfo.hProcess, INFINITE);
            if (waitResult == WAIT_FAILED)
            {
                const auto strErr = TRAP::Utils::String::GetStrError();
                CloseHandle(processInfo.hProcess);
                CloseHandle(processInfo.hThread);
                return TRAP::MakeUnexpected(fmt::format("TRAP::Utils::SafeSystem(): Failed to wait for process ({})!", strErr));
            }

            //Check the exit code of the process
            DWORD exitCode = 0;
            if (!GetExitCodeProcess(processInfo.hProcess, &exitCode))
            {
                const auto strErr = TRAP::Utils::String::GetStrError();
                CloseHandle(processInfo.hProcess);
                CloseHandle(processInfo.hThread);
                return TRAP::MakeUnexpected(fmt::format("TRAP::Utils::SafeSystem(): Failed to get exit code ({})!", strErr));
            }

            //Check if the process exited normally
            if (exitCode != 0)
            {
                CloseHandle(processInfo.hProcess);
                CloseHandle(processInfo.hThread);
                return TRAP::MakeUnexpected(fmt::format("TRAP::Utils::SafeSystem(): Program exited with non-zero status {}", exitCode));
            }
        }

        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

        return {};
    }
#endif /*TRAP_PLATFORM_WINDOWS*/
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Expected<void, std::string> TRAP::Utils::SafeSystem(const std::string& program,
                                                                        const std::vector<std::string>& args,
                                                                        const bool waitForChild)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    if(program.empty())
    {
        TRAP_ASSERT(false, "TRAP::Utils::SafeSystem(): program variable is empty!");
        return TRAP::MakeUnexpected("program is empty!");
    }

#ifdef TRAP_PLATFORM_LINUX
    return SafeSystemLinux(program, args, waitForChild);
#elif defined(TRAP_PLATFORM_WINDOWS)
    return SafeSystemWindows(program, args, waitForChild);
#else
    return TRAP::MakeUnexpected("TRAP::Utils::SafeSystem(): Not implemented for this OS!");
#endif /*TRAP_PLATFORM_LINUX*/
}
