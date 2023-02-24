#ifndef TRAP_STEAMWORKSSDK_H
#define TRAP_STEAMWORKSSDK_H

#include <cstdint>
#include <string>
#include <optional>

#ifdef USE_STEAMWORKS_SDK

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
#include <steam_api.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#endif

namespace TRAP::Utils::Steam
{
    /// <summary>
    /// Initialize the Steamworks SDK.
    /// If the application was launched without Steam, it will be relaunched through Steam.
    /// A callback for messages will be registered.
    ///
    /// Note: This function stops the engine if it fails to initialize.
    /// </summary>
    /// <param name="appID">Steam AppID for this application.</param>
    void Initalize(uint32_t appID);

    /// <summary>
    /// Shutdown the Steamworks SDK.
    /// </summary>
    void Shutdown();

    /// <summary>
    /// Dispatches callbacks and call results to all of the registered listeners.
    /// </summary>
    void RunCallbacks();

    /// <summary>
    /// Check whether the Steamworks SDK is initialized or not.
    /// </summary>
    /// <returns>True if Steamworks SDK is initialized, false otherwise.</returns>
    [[nodiscard]] bool IsInitialized() noexcept;

#ifdef USE_STEAMWORKS_SDK
    /// <summary>
    /// Retrieve the SteamApps interface.
    /// </summary>
    /// <returns>Pointer to the SteamApps interface.</returns>
    [[nodiscard]] ISteamApps* GetSteamApps();

    /// <summary>
    /// Retrieve the SteamFriends interface.
    /// </summary>
    /// <returns>Pointer to the SteamFriends interface.</returns>
    [[nodiscard]] ISteamFriends* GetSteamFriends();

    /// <summary>
    /// Retrieve the SteamHTMLSurface interface.
    /// </summary>
    /// <returns>Pointer to the SteamHTMLSurface interface.</returns>
    [[nodiscard]] ISteamHTMLSurface* GetSteamHTMLSurface();

    /// <summary>
    /// Retrieve the SteamHTTP interface.
    /// </summary>
    /// <returns>Pointer to the SteamHTTP interface.</returns>
    [[nodiscard]] ISteamHTTP* GetSteamHTTP();

    /// <summary>
    /// Retrieve the SteamInput interface.
    /// </summary>
    /// <returns>Pointer to the SteamInput interface.</returns>
    [[nodiscard]] ISteamInput* GetSteamInput();

    /// <summary>
    /// Retrieve the SteamInventory interface.
    /// </summary>
    /// <returns>Pointer to the SteamInventory interface.</returns>
    [[nodiscard]] ISteamInventory* GetSteamInventory();

    /// <summary>
    /// Retrieve the SteamMatchmaking interface.
    /// </summary>
    /// <returns>Pointer to the SteamMatchmaking interface.</returns>
    [[nodiscard]] ISteamMatchmaking* GetSteamMatchmaking();

    /// <summary>
    /// Retrieve the SteamMatchmakingServers interface.
    /// </summary>
    /// <returns>Pointer to the SteamMatchmakingServers interface.</returns>
    [[nodiscard]] ISteamMatchmakingServers* GetSteamMatchmakingServers();

    /// <summary>
    /// Retrieve the SteamMusic interface.
    /// </summary>
    /// <returns>Pointer to the SteamMusic interface.</returns>
    [[nodiscard]] ISteamMusic* GetSteamMusic();

    /// <summary>
    /// Retrieve the SteamMusicRemote interface.
    /// </summary>
    /// <returns>Pointer to the SteamMusicRemote interface.</returns>
    [[nodiscard]] ISteamMusicRemote* GetSteamMusicRemote();

    /// <summary>
    /// Retrieve the SteamNetworkingMessages interface.
    /// </summary>
    /// <returns>Pointer to the SteamNetworkingMessages interface.</returns>
    [[nodiscard]] ISteamNetworkingMessages* GetSteamNetworkingMessages();

    /// <summary>
    /// Retrieve the SteamNetworkingSockets interface.
    /// </summary>
    /// <returns>Pointer to the SteamNetworkingSockets interface.</returns>
    [[nodiscard]] ISteamNetworkingSockets* GetSteamNetworkingSockets();

    /// <summary>
    /// Retrieve the SteamNetworkingUtils interface.
    /// </summary>
    /// <returns>Pointer to the SteamNetworkingUtils interface.</returns>
    [[nodiscard]] ISteamNetworkingUtils* GetSteamNetworkingUtils();

    /// <summary>
    /// Retrieve the SteamParties interface.
    /// </summary>
    /// <returns>Pointer to the SteamParties interface.</returns>
    [[nodiscard]] ISteamParties* GetSteamParties();

    /// <summary>
    /// Retrieve the SteamRemotePlay interface.
    /// </summary>
    /// <returns>Pointer to the SteamRemotePlay interface.</returns>
    [[nodiscard]] ISteamRemotePlay* GetSteamRemotePlay();

    /// <summary>
    /// Retrieve the SteamRemoteStorage interface.
    /// </summary>
    /// <returns>Pointer to the SteamRemoteStorage interface.</returns>
    [[nodiscard]] ISteamRemoteStorage* GetSteamRemoteStorage();

    /// <summary>
    /// Retrieve the SteamScreenshots interface.
    /// </summary>
    /// <returns>Pointer to the SteamScreenshots interface.</returns>
    [[nodiscard]] ISteamScreenshots* GetSteamScreenshots();

    /// <summary>
    /// Retrieve the SteamUGC interface.
    /// </summary>
    /// <returns>Pointer to the SteamUGC interface.</returns>
    [[nodiscard]] ISteamUGC* GetSteamUGC();

    /// <summary>
    /// Retrieve the SteamUser interface.
    /// </summary>
    /// <returns>Pointer to the SteamUser interface.</returns>
    [[nodiscard]] ISteamUser* GetSteamUser();

    /// <summary>
    /// Retrieve the SteamUserStats interface.
    /// </summary>
    /// <returns>Pointer to the SteamUserStats interface.</returns>
    [[nodiscard]] ISteamUserStats* GetSteamUserStats();

    /// <summary>
    /// Retrieve the SteamUtils interface.
    /// </summary>
    /// <returns>Pointer to the SteamUtils interface.</returns>
    [[nodiscard]] ISteamUtils* GetSteamUtils();

    /// <summary>
    /// Retrieve the SteamVideo interface.
    /// </summary>
    /// <returns>Pointer to the SteamVideo interface.</returns>
    [[nodiscard]] ISteamVideo* GetSteamVideo();
#endif
}

#endif /*TRAP_STEAMWORKSSDK_H*/