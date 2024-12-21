#include "TRAPPCH.h"
#include "SteamworksSDK.h"

#include "Log/Log.h"

#include "Utils/ErrorCodes/ErrorCodes.h"

namespace
{
    std::atomic<bool> steamClientInitialized = false;
    std::atomic<bool> steamServerInitialized = false;

    //-------------------------------------------------------------------------------------------------------------------//

#ifdef USE_STEAMWORKS_SDK
    /// @threadsafe
    constexpr void SteamLogCallback(const i32 severity, const char* const msg)
    {
        if(severity == 0)
            TP_INFO(TRAP::Log::SteamworksSDKPrefix, msg);
        else if(severity == 1)
            TP_WARN(TRAP::Log::SteamworksSDKPrefix, msg);
    }
#endif /*USE_STEAMWORKS_SDK*/

    //-------------------------------------------------------------------------------------------------------------------//

#ifdef USE_STEAMWORKS_SDK
    /// @threadsafe
    void ShutdownClient()
    {
        if(!steamClientInitialized)
            return;

        TP_INFO(TRAP::Log::SteamworksSDKPrefix, "Destroying Steam");
        SteamAPI_Shutdown();
        steamClientInitialized = false;
    }
#endif /*USE_STEAMWORKS_SDK*/

    //-------------------------------------------------------------------------------------------------------------------//

#ifdef USE_STEAMWORKS_SDK
    /// @threadsafe
    void ShutdownServer()
    {
        if(!steamServerInitialized)
            return;

        TP_INFO(TRAP::Log::SteamworksSDKPrefix, "Destroying Steam Server");
        SteamGameServer_Shutdown();
        steamServerInitialized = false;
    }
#endif /*USE_STEAMWORKS_SDK*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Steam::InitializeClient([[maybe_unused]] const u32 appID)
{
#ifdef USE_STEAMWORKS_SDK
    if(steamClientInitialized)
        return;

    TP_INFO(TRAP::Log::SteamworksSDKPrefix, "Initializing Steam");

    if(SteamAPI_RestartAppIfNecessary(appID))
        Utils::DisplayError(Utils::ErrorCode::ApplicationNotLaunchedWithSteam);

    switch(SteamAPI_InitEx(nullptr))
    {
    case k_ESteamAPIInitResult_FailedGeneric:
        Utils::DisplayError(Utils::ErrorCode::SteamFailedGeneric);
        break;

    case k_ESteamAPIInitResult_NoSteamClient:
        Utils::DisplayError(Utils::ErrorCode::SteamNotRunning);
        break;

    case k_ESteamAPIInitResult_VersionMismatch:
        Utils::DisplayError(Utils::ErrorCode::SteamVersionMismatch);
        break;

    case k_ESteamAPIInitResult_OK:
        break;
    }

    steamClientInitialized = true;

    SteamUtils()->SetWarningMessageHook(&SteamLogCallback);
#endif /*USE_STEAMWORKS_SDK*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef USE_STEAMWORKS_SDK
void TRAP::Utils::Steam::InitializeServer(const u32 bindIPv4, const u16 gamePort,
                                          const u16 queryPort, const EServerMode authenticationMethod,
                                          const std::string& version)
{
    if(steamServerInitialized)
        return;

    switch(SteamGameServer_InitEx(bindIPv4, gamePort, queryPort, authenticationMethod, version.c_str(), nullptr))
    {
    case k_ESteamAPIInitResult_FailedGeneric:
        Utils::DisplayError(Utils::ErrorCode::SteamFailedGeneric);
        break;

    case k_ESteamAPIInitResult_NoSteamClient:
        Utils::DisplayError(Utils::ErrorCode::SteamNotRunning);
        break;

    case k_ESteamAPIInitResult_VersionMismatch:
        Utils::DisplayError(Utils::ErrorCode::SteamVersionMismatch);
        break;

    case k_ESteamAPIInitResult_OK:
        break;
    }

    steamServerInitialized = true;

    SteamGameServerUtils()->SetWarningMessageHook(&SteamLogCallback);
}
#endif /*USE_STEAMWORKS_SDK*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Steam::Shutdown()
{
#ifdef USE_STEAMWORKS_SDK
    ShutdownClient();
    ShutdownServer();
#endif /*USE_STEAMWORKS_SDK*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Steam::RunCallbacks()
{
#ifdef USE_STEAMWORKS_SDK
    if(steamServerInitialized)
        SteamGameServer_RunCallbacks();

    if(steamClientInitialized)
        SteamAPI_RunCallbacks();
#endif /*USE_STEAMWORKS_SDK*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Utils::Steam::IsClientInitialized() noexcept
{
    return steamClientInitialized;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Utils::Steam::IsServerInitialized() noexcept
{
    return steamServerInitialized;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef USE_STEAMWORKS_SDK

[[nodiscard]] ISteamApps* TRAP::Utils::Steam::GetSteamApps()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamApps(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamApps();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamFriends* TRAP::Utils::Steam::GetSteamFriends()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamFriends(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamFriends();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamHTMLSurface* TRAP::Utils::Steam::GetSteamHTMLSurface()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamHTMLSurface(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamHTMLSurface();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamHTTP* TRAP::Utils::Steam::GetSteamClientHTTP()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamClientHTTP(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamHTTP();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamHTTP* TRAP::Utils::Steam::GetSteamServerHTTP()
{
    if(!steamServerInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamServerHTTP(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamGameServerHTTP();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamInput* TRAP::Utils::Steam::GetSteamInput()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamInput(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamInput();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamInventory* TRAP::Utils::Steam::GetSteamClientInventory()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamClientInventory(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamInventory();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamInventory* TRAP::Utils::Steam::GetSteamServerInventory()
{
    if(!steamServerInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamServerInventory(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamGameServerInventory();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamMatchmaking* TRAP::Utils::Steam::GetSteamMatchmaking()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamMatchmaking(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamMatchmaking();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamMatchmakingServers* TRAP::Utils::Steam::GetSteamMatchmakingServers()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamMatchmakingServers(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamMatchmakingServers();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamMusic* TRAP::Utils::Steam::GetSteamMusic()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamMusic(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamMusic();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamMusicRemote* TRAP::Utils::Steam::GetSteamMusicRemote()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamMusicRemote(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamMusicRemote();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamNetworkingMessages* TRAP::Utils::Steam::GetSteamClientNetworkingMessages()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamClientNetworkingMessages(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamNetworkingMessages();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamNetworkingMessages* TRAP::Utils::Steam::GetSteamServerNetworkingMessages()
{
    if(!steamServerInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamServerNetworkingMessages(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamGameServerNetworkingMessages();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamNetworkingSockets* TRAP::Utils::Steam::GetSteamClientNetworkingSockets()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamClientNetworkingSockets(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamNetworkingSockets();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamNetworkingSockets* TRAP::Utils::Steam::GetSteamServerNetworkingSockets()
{
    if(!steamServerInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamServerNetworkingSockets(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamGameServerNetworkingSockets();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamNetworkingUtils* TRAP::Utils::Steam::GetSteamNetworkingUtils()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamNetworkingUtils(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamNetworkingUtils();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamParties* TRAP::Utils::Steam::GetSteamParties()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamParties(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamParties();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamRemotePlay* TRAP::Utils::Steam::GetSteamRemotePlay()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamRemotePlay(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamRemotePlay();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamRemoteStorage* TRAP::Utils::Steam::GetSteamRemoteStorage()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamRemoteStorage(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamRemoteStorage();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamScreenshots* TRAP::Utils::Steam::GetSteamScreenshots()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamScreenshots(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamScreenshots();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUGC* TRAP::Utils::Steam::GetSteamClientUGC()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamClientUGC(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamUGC();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUGC* TRAP::Utils::Steam::GetSteamServerUGC()
{
    if(!steamServerInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamServerUGC(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamGameServerUGC();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUser* TRAP::Utils::Steam::GetSteamUser()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamUser(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamUser();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUserStats* TRAP::Utils::Steam::GetSteamUserStats()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamUserStats(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamUserStats();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUtils* TRAP::Utils::Steam::GetSteamClientUtils()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamClientUtils(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamUtils();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUtils* TRAP::Utils::Steam::GetSteamServerUtils()
{
    if(!steamServerInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamServerUtils(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamGameServerUtils();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamVideo* TRAP::Utils::Steam::GetSteamVideo()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamVideo(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamVideo();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamTimeline* TRAP::Utils::Steam::GetSteamTimeline()
{
    if(!steamClientInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamTimeline(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamTimeline();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamGameServer* TRAP::Utils::Steam::GetSteamGameServer()
{
    if(!steamServerInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamGameServer(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamGameServer();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamGameServerStats* TRAP::Utils::Steam::GetSteamGameServerStats()
{
    if(!steamServerInitialized)
    {
        TRAP_ASSERT(false, "Steam::GetSteamGameServerStats(): Steamworks SDK is not initialized!");
        return nullptr;
    }

    return SteamGameServerStats();
}

#endif /*USE_STEAMWORKS_SDK*/
