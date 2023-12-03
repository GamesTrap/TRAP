premake.modules.includethirdparty = {}
local m = premake.modules.includethirdparty

local p = premake

function m.IncludeDiscordGameSDK()
    local winLibPath        = path.join(_MAIN_SCRIPT_DIR, "Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.dll")
    local winDLLPath        = path.join(_MAIN_SCRIPT_DIR, "Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.dll.lib")
    local linuxLibSOPath    = path.join(_MAIN_SCRIPT_DIR, "Dependencies/DiscordGameSDK/lib/x86_64/libdiscord_game_sdk.so")
    local linuxSOPath       = path.join(_MAIN_SCRIPT_DIR, "Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.so")
    local discordHeaderPath = path.join(_MAIN_SCRIPT_DIR, "Dependencies/DiscordGameSDK/cpp/discord.h")

    filter {}

    if not os.isfile(discordHeaderPath) then
        return false
    end

    if os.target() == "linux" then
        if(not (os.isfile(linuxLibSOPath) or os.isfile(linuxSOPath)))  then
            return false
        end
    elseif os.target() == "windows" then
        if(not (os.isfile(winLibPath) or os.isfile(winDLLPath)))  then
            return false
        end
    end

    filter {"system:windows", "kind:ConsoleApp or WindowedApp"}
        links "%{IncludeDir.DISCORDGAMESDK}/../lib/x86_64/discord_game_sdk.dll.lib"
        postbuildcommands "{COPYDIR} %{IncludeDir.DISCORDGAMESDK}/../lib/x86_64/discord_game_sdk.dll %{cfg.targetdir}"

    filter {"system:linux", "kind:ConsoleApp or WindowedApp"}
        libdirs "%{IncludeDir.DISCORDGAMESDK}/../lib/x86_64"
        postbuildcommands "{COPYFILE} %{IncludeDir.DISCORDGAMESDK}/../lib/x86_64/libdiscord_game_sdk.so %{cfg.targetdir}"
        links "discord_game_sdk"

    filter {}

    defines "USE_DISCORD_GAME_SDK"
    files
    {
        "%{IncludeDir.DISCORDGAMESDK}/**.h",
        "%{IncludeDir.DISCORDGAMESDK}/**.cpp"
    }
    externalincludedirs "%{IncludeDir.DISCORDGAMESDK}"

    if m.AlreadyExecutedDiscordCompat == nil or m.AlreadyExecutedDiscordCompat == false then
        dofileopt(path.join(_MAIN_SCRIPT_DIR, "Dependencies/DiscordGameSDK/Compatibility"))
        m.AlreadyExecutedDiscordCompat = true;
    end

    return true
end

function m.IncludeNsightAftermathSDK()
    local linuxSOPath         = path.join(_MAIN_SCRIPT_DIR, "Dependencies/Nsight-Aftermath/lib/x64/libGFSDK_Aftermath_Lib.x64.so")
    local winDLLPath          = path.join(_MAIN_SCRIPT_DIR, "Dependencies/Nsight-Aftermath/lib/x64/GFSDK_Aftermath_Lib.x64.dll")
    local winLibPath          = path.join(_MAIN_SCRIPT_DIR, "Dependencies/Nsight-Aftermath/lib/x64/GFSDK_Aftermath_Lib.x64.lib")
    local winLLVMDLLPath      = path.join(_MAIN_SCRIPT_DIR, "Dependencies/Nsight-Aftermath/lib/x64/llvm_7_0_1.dll")
    local aftermathHeaderPath = path.join(_MAIN_SCRIPT_DIR, "Dependencies/Nsight-Aftermath/include/GFSDK_Aftermath.h")

    filter {}

    if not os.isfile(aftermathHeaderPath) then
        return false
    end

    if os.target() == "linux" then
        if(not (os.isfile(linuxSOPath)))  then
            return false
        end
    elseif os.target() == "windows" then
        if(not (os.isfile(winLibPath) or os.isfile(winDLLPath) or os.isfile(winLLVMDLLPath)))  then
            return false
        end
    end

    externalincludedirs "%{IncludeDir.NSIGHTAFTERMATH}"
    defines "NSIGHT_AFTERMATH_AVAILABLE"

    filter {"system:windows", "kind:ConsoleApp or WindowedApp"}
        postbuildcommands "{COPYDIR} %{IncludeDir.NSIGHTAFTERMATH}/../lib/x64/GFSDK_Aftermath_Lib.x64.dll %{cfg.targetdir}"

    filter {"system:linux", "kind:ConsoleApp or WindowedApp"}
        postbuildcommands "{COPYFILE} %{IncludeDir.NSIGHTAFTERMATH}/../lib/x64/libGFSDK_Aftermath_Lib.x64.so %{cfg.targetdir}"

    filter {}

    return true
end

function m.IncludeSteamworksSDK()
    local linuxSOPath          = path.join(_MAIN_SCRIPT_DIR, "Dependencies/SteamworksSDK/sdk/redistributable_bin/linux64/libsteam_api.so")
    local winDLLPath           = path.join(_MAIN_SCRIPT_DIR, "Dependencies/SteamworksSDK/sdk/redistributable_bin/win64/steam_api64.dll")
    local winLibPath           = path.join(_MAIN_SCRIPT_DIR, "Dependencies/SteamworksSDK/sdk/redistributable_bin/win64/steam_api64.lib")
    local steamworksFolderPath = path.join(_MAIN_SCRIPT_DIR, "Dependencies/SteamworksSDK/sdk/public/steam")

    filter {}

    if not os.isdir(steamworksFolderPath) then
        return false
    end

    if os.target() == "linux" then
        if(not (os.isfile(linuxSOPath)))  then
            return false
        end
    elseif os.target() == "windows" then
        if(not (os.isfile(winLibPath) or os.isfile(winDLLPath)))  then
            return false
        end
    end

    filter {"system:windows", "kind:ConsoleApp or WindowedApp"}
        links "%{IncludeDir.STEAMWORKSSDK}/../../redistributable_bin/win64/steam_api64.lib"
        postbuildcommands "{COPYDIR} %{IncludeDir.STEAMWORKSSDK}/../../redistributable_bin/win64/steam_api64.dll %{cfg.targetdir}"

    filter {"system:linux", "kind:ConsoleApp or WindowedApp"}
        links "steam_api"
        libdirs "%{IncludeDir.STEAMWORKSSDK}/../../redistributable_bin/linux64"
        postbuildcommands "{COPYFILE} %{IncludeDir.STEAMWORKSSDK}/../../redistributable_bin/linux64/libsteam_api.so %{cfg.targetdir}"

    filter {}
    externalincludedirs "%{IncludeDir.STEAMWORKSSDK}"
    defines "USE_STEAMWORKS_SDK"

    return true
end

function m.IncludeNVIDIAReflexSDK()
    filter {}

    if os.target() ~= "windows" then
        return false
    end

    local PCLStatsHeader     = os.matchfiles(_MAIN_SCRIPT_DIR .. "/Dependencies/NVIDIA-Reflex/**pclstats.h")
    local reflexVulkanHeader = os.matchfiles(_MAIN_SCRIPT_DIR .. "/Dependencies/NVIDIA-Reflex/**NvLowLatencyVk.h")
    local winVulkanLib       = os.matchfiles(_MAIN_SCRIPT_DIR .. "/Dependencies/NVIDIA-Reflex/**NvLowLatencyVk.lib")
    local winVulkanDLL       = os.matchfiles(_MAIN_SCRIPT_DIR .. "/Dependencies/NVIDIA-Reflex/**NvLowLatencyVk.dll")

    if next(reflexVulkanHeader) ~= nil and next(winVulkanLib) ~= nil and
       next(winVulkanDLL) ~= nil and next(PCLStatsHeader) ~= nil then
        filter "system:windows"
            externalincludedirs
            {
                "%{IncludeDir.NVIDIAREFLEX}",
                "%{IncludeDir.NVIDIAREFLEXSTATS}"
            }

            defines "NVIDIA_REFLEX_AVAILABLE"

        filter {"system:windows", "kind:ConsoleApp or WindowedApp"}
            links "%{IncludeDir.NVIDIAREFLEX}/../lib/NvLowLatencyVk.lib"
            postbuildcommands "{COPYDIR} %{IncludeDir.NVIDIAREFLEX}/../lib/NvLowLatencyVk.dll %{cfg.targetdir}"

        filter {}

        return true
    end

    return false
end

return m