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
#include <steam_gameserver.h>
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
    void InitializeClient(uint32_t appID);

    /// <summary>
    /// Initialize the Steamworks SDK for servers.
    /// </summary>
    /// <param name="bindIPv4">IPv4 to bind to. The IP should be in host order (i.e. 127.0.0.1 == 0x7F000001).</param>
    /// <param name="gamePort">Port that clients will connect to.</param>
    /// <param name="queryPort">Port for server browser related duties and info pings from clients.</param>
    /// <param name="authenticationMethod">Method of authentication to use.</param>
    /// <param name="version">
    /// Version of the server. This usually has the format "x.x.x.x".
    /// It is used by the master server to detect out of date servers.
    /// </param>
    /// <returns>True if Steamworks SDK for servers is initialized, false otherwise.</returns>
    bool InitializeServer(uint32_t bindIPv4, uint16_t gamePort,
                          uint16_t queryPort, EServerMode authenticationMethod,
                          const std::string& version);

    /// <summary>
    /// Shutdown the Steamworks SDK.
    /// </summary>
    void Shutdown();

    /// <summary>
    /// Shutdown the Steamworks SDK client.
    /// </summary>
    void ShutdownClient();

    /// <summary>
    /// Shutdown the Steamworks SDK server.
    /// </summary>
    void ShutdownServer();

    /// <summary>
    /// Dispatches callbacks and call results to all of the registered listeners.
    /// </summary>
    void RunCallbacks();

    /// <summary>
    /// Check whether the Steamworks SDK for clients is initialized or not.
    /// </summary>
    /// <returns>True if Steamworks SDK is initialized, false otherwise.</returns>
    [[nodiscard]] bool IsClientInitialized() noexcept;

    /// <summary>
    /// Check whether the Steamworks SDK for servers is initialized or not.
    /// </summary>
    /// <returns>True if Steamworks SDK is initialized, false otherwise.</returns>
    [[nodiscard]] bool IsServerInitialized() noexcept;

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
    /// Retrieve the SteamHTTP interface from the client.
    /// </summary>
    /// <returns>Pointer to the SteamHTTP interface.</returns>
    [[nodiscard]] ISteamHTTP* GetSteamClientHTTP();

    /// <summary>
    /// Retrieve the SteamHTTP interface from the server.
    /// </summary>
    /// <returns>Pointer to the SteamHTTP interface.</returns>
    [[nodiscard]] ISteamHTTP* GetSteamServerHTTP();

    /// <summary>
    /// Retrieve the SteamInput interface.
    /// </summary>
    /// <returns>Pointer to the SteamInput interface.</returns>
    [[nodiscard]] ISteamInput* GetSteamInput();

    /// <summary>
    /// Retrieve the SteamInventory interface from the client.
    /// </summary>
    /// <returns>Pointer to the SteamInventory interface.</returns>
    [[nodiscard]] ISteamInventory* GetSteamClientInventory();

    /// <summary>
    /// Retrieve the SteamInventory interface from the server.
    /// </summary>
    /// <returns>Pointer to the SteamInventory interface.</returns>
    [[nodiscard]] ISteamInventory* GetSteamServerInventory();

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
    /// Retrieve the SteamNetworkingMessages interface from the client.
    /// </summary>
    /// <returns>Pointer to the SteamNetworkingMessages interface.</returns>
    [[nodiscard]] ISteamNetworkingMessages* GetSteamClientNetworkingMessages();

    /// <summary>
    /// Retrieve the SteamNetworkingMessages interface from the server.
    /// </summary>
    /// <returns>Pointer to the SteamNetworkingMessages interface.</returns>
    [[nodiscard]] ISteamNetworkingMessages* GetSteamServerNetworkingMessages();

    /// <summary>
    /// Retrieve the SteamNetworkingSockets interface from the client.
    /// </summary>
    /// <returns>Pointer to the SteamNetworkingSockets interface.</returns>
    [[nodiscard]] ISteamNetworkingSockets* GetSteamClientNetworkingSockets();

    /// <summary>
    /// Retrieve the SteamNetworkingSockets interface from the server.
    /// </summary>
    /// <returns>Pointer to the SteamNetworkingSockets interface.</returns>
    [[nodiscard]] ISteamNetworkingSockets* GetSteamServerNetworkingSockets();

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
    /// Retrieve the SteamUGC interface from the client.
    /// </summary>
    /// <returns>Pointer to the SteamUGC interface.</returns>
    [[nodiscard]] ISteamUGC* GetSteamClientUGC();

    /// <summary>
    /// Retrieve the SteamUGC interface from the server.
    /// </summary>
    /// <returns>Pointer to the SteamUGC interface.</returns>
    [[nodiscard]] ISteamUGC* GetSteamServerUGC();

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
    /// Retrieve the SteamUtils interface from the client.
    /// </summary>
    /// <returns>Pointer to the SteamUtils interface.</returns>
    [[nodiscard]] ISteamUtils* GetSteamClientUtils();

    /// <summary>
    /// Retrieve the SteamUtils interface from the server.
    /// </summary>
    /// <returns>Pointer to the SteamUtils interface.</returns>
    [[nodiscard]] ISteamUtils* GetSteamServerUtils();

    /// <summary>
    /// Retrieve the SteamVideo interface.
    /// </summary>
    /// <returns>Pointer to the SteamVideo interface.</returns>
    [[nodiscard]] ISteamVideo* GetSteamVideo();

    /// <summary>
    /// Retrieve the SteamGameServer interface.
    /// </summary>
    /// <returns>Pointer to the SteamGameServer interface.</returns>
    [[nodiscard]] ISteamGameServer* GetSteamGameServer();

    /// <summary>
    /// Retrieve the SteamGameServerStats interface.
    /// </summary>
    /// <returns>Pointer to the SteamGameServerStats interface.</returns>
    [[nodiscard]] ISteamGameServerStats* GetSteamGameServerStats();

#endif /*USE_STEAMWORKS_SDK*/
}

#endif /*TRAP_STEAMWORKSSDK_H*/