#include "TRAPPCH.h"
#include "SteamworksSDK.h"

#include "Log/Log.h"

#include "Utils/ErrorCodes/ErrorCodes.h"

bool steamClientInitialized = false;
bool steamServerInitialized = false;

//-------------------------------------------------------------------------------------------------------------------//
#ifdef USE_STEAMWORKS_SDK
void SteamLogCallback(const int32_t severity, const char* const msg)
{
    if(severity == 0)
        TP_INFO(TRAP::Log::SteamworksSDKPrefix, msg);
    else if(severity == 1)
        TP_WARN(TRAP::Log::SteamworksSDKPrefix, msg);
}
#endif /*USE_STEAMWORKS_SDK*/
//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Steam::InitializeClient([[maybe_unused]] const uint32_t appID)
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
        [[fallthrough]];
    default:
        break;
    }

    steamClientInitialized = true;

    SteamUtils()->SetWarningMessageHook(&SteamLogCallback);
#endif /*USE_STEAMWORKS_SDK*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef USE_STEAMWORKS_SDK
bool TRAP::Utils::Steam::InitializeServer(const uint32_t bindIPv4, const uint16_t gamePort,
                                          const uint16_t queryPort, const EServerMode authenticationMethod,
                                          const std::string& version)
{
    if(steamServerInitialized)
        return true;

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
        [[fallthrough]];
    default:
        break;
    }

    SteamGameServerUtils()->SetWarningMessageHook(&SteamLogCallback);

    steamServerInitialized = true;

    return steamServerInitialized;
}
#endif /*USE_STEAMWORKS_SDK*/

//-------------------------------------------------------------------------------------------------------------------//

#ifdef USE_STEAMWORKS_SDK
void ShutdownClient()
{
    if(steamClientInitialized)
    {
        TP_INFO(TRAP::Log::SteamworksSDKPrefix, "Destroying Steam");
        SteamAPI_Shutdown();
    }
}
#endif /*USE_STEAMWORKS_SDK*/

//-------------------------------------------------------------------------------------------------------------------//

#ifdef USE_STEAMWORKS_SDK
void ShutdownServer()
{
    if(steamServerInitialized)
    {
        TP_INFO(TRAP::Log::SteamworksSDKPrefix, "Destroying Steam Server");
        SteamGameServer_Shutdown();
    }
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
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamApps(): SteamAPI is not initialized!");

    return SteamApps();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamFriends* TRAP::Utils::Steam::GetSteamFriends()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamFriends(): SteamAPI is not initialized!");

    return SteamFriends();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamHTMLSurface* TRAP::Utils::Steam::GetSteamHTMLSurface()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamHTMLSurface(): SteamAPI is not initialized!");

    return SteamHTMLSurface();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamHTTP* TRAP::Utils::Steam::GetSteamClientHTTP()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamClientHTTP(): SteamAPI is not initialized!");

    return SteamHTTP();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamHTTP* TRAP::Utils::Steam::GetSteamServerHTTP()
{
    TRAP_ASSERT(steamServerInitialized, "Steam::GetSteamServerHTTP(): SteamAPI is not initialized!");

    return SteamGameServerHTTP();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamInput* TRAP::Utils::Steam::GetSteamInput()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamInput(): SteamAPI is not initialized!");

    return SteamInput();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamInventory* TRAP::Utils::Steam::GetSteamClientInventory()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamClientInventory(): SteamAPI is not initialized!");

    return SteamInventory();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamInventory* TRAP::Utils::Steam::GetSteamServerInventory()
{
    TRAP_ASSERT(steamServerInitialized, "Steam::GetSteamServerInventory(): SteamAPI is not initialized!");

    return SteamGameServerInventory();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamMatchmaking* TRAP::Utils::Steam::GetSteamMatchmaking()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamMatchmaking(): SteamAPI is not initialized!");

    return SteamMatchmaking();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamMatchmakingServers* TRAP::Utils::Steam::GetSteamMatchmakingServers()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamMatchmakingServers(): SteamAPI is not initialized!");

    return SteamMatchmakingServers();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamMusic* TRAP::Utils::Steam::GetSteamMusic()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamMusic(): SteamAPI is not initialized!");

    return SteamMusic();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamMusicRemote* TRAP::Utils::Steam::GetSteamMusicRemote()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamMusicRemote(): SteamAPI is not initialized!");

    return SteamMusicRemote();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamNetworkingMessages* TRAP::Utils::Steam::GetSteamClientNetworkingMessages()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamClientNetworkingMessages(): SteamAPI is not initialized!");

    return SteamNetworkingMessages();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamNetworkingMessages* TRAP::Utils::Steam::GetSteamServerNetworkingMessages()
{
    TRAP_ASSERT(steamServerInitialized, "Steam::GetSteamServerNetworkingMessages(): SteamAPI is not initialized!");

    return SteamGameServerNetworkingMessages();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamNetworkingSockets* TRAP::Utils::Steam::GetSteamClientNetworkingSockets()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamClientNetworkingSockets(): SteamAPI is not initialized!");

    return SteamNetworkingSockets();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamNetworkingSockets* TRAP::Utils::Steam::GetSteamServerNetworkingSockets()
{
    TRAP_ASSERT(steamServerInitialized, "Steam::GetSteamServerNetworkingSockets(): SteamAPI is not initialized!");

    return SteamGameServerNetworkingSockets();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamNetworkingUtils* TRAP::Utils::Steam::GetSteamNetworkingUtils()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamNetworkingUtils(): SteamAPI is not initialized!");

    return SteamNetworkingUtils();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamParties* TRAP::Utils::Steam::GetSteamParties()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamParties(): SteamAPI is not initialized!");

    return SteamParties();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamRemotePlay* TRAP::Utils::Steam::GetSteamRemotePlay()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamRemotePlay(): SteamAPI is not initialized!");

    return SteamRemotePlay();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamRemoteStorage* TRAP::Utils::Steam::GetSteamRemoteStorage()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamRemoteStorage(): SteamAPI is not initialized!");

    return SteamRemoteStorage();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamScreenshots* TRAP::Utils::Steam::GetSteamScreenshots()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamScreenshots(): SteamAPI is not initialized!");

    return SteamScreenshots();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUGC* TRAP::Utils::Steam::GetSteamClientUGC()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamClientUGC(): SteamAPI is not initialized!");

    return SteamUGC();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUGC* TRAP::Utils::Steam::GetSteamServerUGC()
{
    TRAP_ASSERT(steamServerInitialized, "Steam::GetSteamServerUGC(): SteamAPI is not initialized!");

    return SteamGameServerUGC();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUser* TRAP::Utils::Steam::GetSteamUser()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamUser(): SteamAPI is not initialized!");

    return SteamUser();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUserStats* TRAP::Utils::Steam::GetSteamUserStats()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamUserStats(): SteamAPI is not initialized!");

    return SteamUserStats();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUtils* TRAP::Utils::Steam::GetSteamClientUtils()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamClientUtils(): SteamAPI is not initialized!");

    return SteamUtils();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUtils* TRAP::Utils::Steam::GetSteamServerUtils()
{
    TRAP_ASSERT(steamServerInitialized, "Steam::GetSteamServerUtils(): SteamAPI is not initialized!");

    return SteamGameServerUtils();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamVideo* TRAP::Utils::Steam::GetSteamVideo()
{
    TRAP_ASSERT(steamClientInitialized, "Steam::GetSteamVideo(): SteamAPI is not initialized!");

    return SteamVideo();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamGameServer* TRAP::Utils::Steam::GetSteamGameServer()
{
    TRAP_ASSERT(steamServerInitialized, "Steam::GetSteamGameServer(): SteamGameServer is not initialized!");

    return SteamGameServer();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamGameServerStats* TRAP::Utils::Steam::GetSteamGameServerStats()
{
    TRAP_ASSERT(steamServerInitialized, "Steam::GetSteamGameServerStats(): SteamGameServer is not initialized!");

    return SteamGameServerStats();
}

#endif /*USE_STEAMWORKS_SDK*/
