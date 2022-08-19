#include "TRAPPCH.h"
#include "SteamworksSDK.h"

#include "Log/Log.h"

#include "Core/Base.h"

#ifdef USE_STEAMWORKS_SDK
bool steamInitialized = false;
#endif

//-------------------------------------------------------------------------------------------------------------------//
#ifdef USE_STEAMWORKS_SDK
void SteamLogCallback(const int32_t severity, const char* msg)
{
    if(severity == 0)
        TP_INFO(TRAP::Log::SteamworksSDKPrefix, msg);
    else if(severity == 1)
        TP_WARN(TRAP::Log::SteamworksSDKPrefix, msg);
}
#endif
//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Steam::Initalize([[maybe_unused]] const uint32_t appID)
{
#ifdef USE_STEAMWORKS_SDK
    if(steamInitialized)
        return;

    TP_INFO(TRAP::Log::SteamworksSDKPrefix, "Initializing Steam");

    if(SteamAPI_RestartAppIfNecessary(appID))
    {
        TP_ERROR(TRAP::Log::SteamworksSDKPrefix, "Please launch the game through Steam!");
        std::exit(-1);
    }

    if(!SteamAPI_Init())
    {
        TP_ERROR(TRAP::Log::SteamworksSDKPrefix, "Steam must be running to play this game!");
        std::exit(-1);
    }

    SteamUtils()->SetWarningMessageHook(&SteamLogCallback);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Steam::Shutdown()
{
#ifdef USE_STEAMWORKS_SDK
    if(!steamInitialized)
        return;

    TP_INFO(TRAP::Log::SteamworksSDKPrefix, "Destroying Steam");

    SteamAPI_Shutdown();
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Steam::RunCallbacks()
{
#ifdef USE_STEAMWORKS_SDK
    if(steamInitialized)
        SteamAPI_RunCallbacks();
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Steam::IsInitialized()
{
#ifdef USE_STEAMWORKS_SDK
    return steamInitialized;
#else
    return false;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef USE_STEAMWORKS_SDK

ISteamApps* TRAP::Utils::Steam::GetSteamApps()
{
    return SteamApps();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamFriends* TRAP::Utils::Steam::GetSteamFriends()
{
    return SteamFriends();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamHTMLSurface* TRAP::Utils::Steam::GetSteamHTMLSurface()
{
    return SteamHTMLSurface();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamHTTP* TRAP::Utils::Steam::GetSteamHTTP()
{
    return SteamHTTP();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamInput* TRAP::Utils::Steam::GetSteamInput()
{
    return SteamInput();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamInventory* TRAP::Utils::Steam::GetSteamInventory()
{
    return SteamInventory();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamMatchmaking* TRAP::Utils::Steam::GetSteamMatchmaking()
{
    return SteamMatchmaking();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamMatchmakingServers* TRAP::Utils::Steam::GetSteamMatchmakingServers()
{
    return SteamMatchmakingServers();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamMusic* TRAP::Utils::Steam::GetSteamMusic()
{
    return SteamMusic();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamMusicRemote* TRAP::Utils::Steam::GetSteamMusicRemote()
{
    return SteamMusicRemote();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamNetworkingMessages* TRAP::Utils::Steam::GetSteamNetworkingMessages()
{
    return SteamNetworkingMessages();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamNetworkingSockets* TRAP::Utils::Steam::GetSteamNetworkingSockets()
{
    return SteamNetworkingSockets();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamNetworkingUtils* TRAP::Utils::Steam::GetSteamNetworkingUtils()
{
    return SteamNetworkingUtils();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamParties* TRAP::Utils::Steam::GetSteamParties()
{
    return SteamParties();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamRemotePlay* TRAP::Utils::Steam::GetSteamRemotePlay()
{
    return SteamRemotePlay();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamRemoteStorage* TRAP::Utils::Steam::GetSteamRemoteStorage()
{
    return SteamRemoteStorage();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamScreenshots* TRAP::Utils::Steam::GetSteamScreenshots()
{
    return SteamScreenshots();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamUGC* TRAP::Utils::Steam::GetSteamUGC()
{
    return SteamUGC();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamUser* TRAP::Utils::Steam::GetSteamUser()
{
    return SteamUser();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamUserStats* TRAP::Utils::Steam::GetSteamUserStats()
{
    return SteamUserStats();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamUtils* TRAP::Utils::Steam::GetSteamUtils()
{
    return SteamUtils();
}

//-------------------------------------------------------------------------------------------------------------------//

ISteamVideo* TRAP::Utils::Steam::GetSteamVideo()
{
    return SteamVideo();
}

#endif
