#include "TRAPPCH.h"
#include "SteamworksSDK.h"

#include "Log/Log.h"

#include "Core/Base.h"

#ifdef USE_STEAMWORKS_SDK
bool steamInitialized = false;
#endif

//-------------------------------------------------------------------------------------------------------------------//
#ifdef USE_STEAMWORKS_SDK
void SteamLogCallback(const int32_t severity, const char* const msg)
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
		TRAP::Utils::Dialogs::ShowMsgBox("Application error",
										 "Please launch the game through Steam!\nError code: 0x0015",
										 Utils::Dialogs::Style::Error,
										 Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::SteamworksSDKPrefix, "Please launch the game through Steam! (0x0015)");
        exit(0x0015);
    }

    if(!SteamAPI_Init())
    {
		TRAP::Utils::Dialogs::ShowMsgBox("Application error",
										 "Steam must be running to play the game!\nError code: 0x0016",
										 Utils::Dialogs::Style::Error,
										 Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::SteamworksSDKPrefix, "Steam must be running to play the game! (0x0016)");
        exit(0x0016);
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

[[nodiscard]] bool TRAP::Utils::Steam::IsInitialized() noexcept
{
#ifdef USE_STEAMWORKS_SDK
    return steamInitialized;
#else
    return false;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef USE_STEAMWORKS_SDK

[[nodiscard]] ISteamApps* TRAP::Utils::Steam::GetSteamApps()
{
    return SteamApps();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamFriends* TRAP::Utils::Steam::GetSteamFriends()
{
    return SteamFriends();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamHTMLSurface* TRAP::Utils::Steam::GetSteamHTMLSurface()
{
    return SteamHTMLSurface();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamHTTP* TRAP::Utils::Steam::GetSteamHTTP()
{
    return SteamHTTP();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamInput* TRAP::Utils::Steam::GetSteamInput()
{
    return SteamInput();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamInventory* TRAP::Utils::Steam::GetSteamInventory()
{
    return SteamInventory();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamMatchmaking* TRAP::Utils::Steam::GetSteamMatchmaking()
{
    return SteamMatchmaking();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamMatchmakingServers* TRAP::Utils::Steam::GetSteamMatchmakingServers()
{
    return SteamMatchmakingServers();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamMusic* TRAP::Utils::Steam::GetSteamMusic()
{
    return SteamMusic();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamMusicRemote* TRAP::Utils::Steam::GetSteamMusicRemote()
{
    return SteamMusicRemote();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamNetworkingMessages* TRAP::Utils::Steam::GetSteamNetworkingMessages()
{
    return SteamNetworkingMessages();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamNetworkingSockets* TRAP::Utils::Steam::GetSteamNetworkingSockets()
{
    return SteamNetworkingSockets();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamNetworkingUtils* TRAP::Utils::Steam::GetSteamNetworkingUtils()
{
    return SteamNetworkingUtils();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamParties* TRAP::Utils::Steam::GetSteamParties()
{
    return SteamParties();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamRemotePlay* TRAP::Utils::Steam::GetSteamRemotePlay()
{
    return SteamRemotePlay();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamRemoteStorage* TRAP::Utils::Steam::GetSteamRemoteStorage()
{
    return SteamRemoteStorage();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamScreenshots* TRAP::Utils::Steam::GetSteamScreenshots()
{
    return SteamScreenshots();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUGC* TRAP::Utils::Steam::GetSteamUGC()
{
    return SteamUGC();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUser* TRAP::Utils::Steam::GetSteamUser()
{
    return SteamUser();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUserStats* TRAP::Utils::Steam::GetSteamUserStats()
{
    return SteamUserStats();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamUtils* TRAP::Utils::Steam::GetSteamUtils()
{
    return SteamUtils();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] ISteamVideo* TRAP::Utils::Steam::GetSteamVideo()
{
    return SteamVideo();
}

#endif
