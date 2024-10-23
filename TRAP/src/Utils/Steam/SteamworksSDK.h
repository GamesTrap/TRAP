#ifndef TRAP_STEAMWORKSSDK_H
#define TRAP_STEAMWORKSSDK_H

#include <string>

#include "Core/Types.h"

#ifdef USE_STEAMWORKS_SDK

#ifdef _MSC_VER
    #pragma warning(disable: 4996)
#endif /*_MSC_VER*/
#include <steam_api.h>
#include <steam_gameserver.h>
#ifdef _MSC_VER
    #pragma warning(default: 4996)
#endif /*_MSC_VER*/

#endif /*USE_STEAMWORKS_SDK*/

namespace TRAP::Utils::Steam
{
    /// @brief Initialize the Steamworks SDK.
    /// If the application was launched without Steam, it will be relaunched through Steam.
    /// A callback for messages will be registered.
    /// @param appID Steam AppID for this application.
    /// @note This function stops the engine if it fails to initialize.
    void InitializeClient(u32 appID);

#ifdef USE_STEAMWORKS_SDK
    /// @brief Initialize the Steamworks SDK for servers.
    /// @param bindIPv4 IPv4 to bind to. The IP should be in host order (i.e. 127.0.0.1 == 0x7F000001).
    /// @param gamePort Port that clients will connect to.
    /// @param queryPort Port for server browser related duties and info pings from clients.
    /// @param authenticationMethod Method of authentication to use.
    /// @param version Version of the server. This usually has the format "x.x.x.x".
    /// It is used by the master server to detect out of date servers.
    void InitializeServer(u32 bindIPv4, u16 gamePort,
                          u16 queryPort, EServerMode authenticationMethod,
                          const std::string& version);
#endif /*USE_STEAMWORKS_SDK*/

    /// @brief Shutdown the Steamworks SDK.
    void Shutdown();

    /// @brief Dispatches callbacks and call results to all of the registered listeners.
    void RunCallbacks();

    /// @brief Check whether the Steamworks SDK for clients is initialized or not.
    /// @return True if Steamworks SDK is initialized, false otherwise.
    [[nodiscard]] bool IsClientInitialized() noexcept;

    /// @brief Check whether the Steamworks SDK for servers is initialized or not.
    /// @return True if Steamworks SDK is initialized, false otherwise.
    [[nodiscard]] bool IsServerInitialized() noexcept;

#ifdef USE_STEAMWORKS_SDK

    /// @brief Retrieve the SteamApps interface.
    /// @return Pointer to the SteamApps interface.
    [[nodiscard]] ISteamApps* GetSteamApps();

    /// @brief Retrieve the SteamFriends interface.
    /// @return Pointer to the SteamFriends interface.
    [[nodiscard]] ISteamFriends* GetSteamFriends();

    /// @brief Retrieve the SteamHTMLSurface interface.
    /// @return Pointer to the SteamHTMLSurface interface.
    [[nodiscard]] ISteamHTMLSurface* GetSteamHTMLSurface();

    /// @brief Retrieve the SteamHTTP interface from the client.
    /// @return Pointer to the SteamHTTP interface.
    [[nodiscard]] ISteamHTTP* GetSteamClientHTTP();

    /// @brief Retrieve the SteamHTTP interface from the server.
    /// @return Pointer to the SteamHTTP interface.
    [[nodiscard]] ISteamHTTP* GetSteamServerHTTP();

    /// @brief Retrieve the SteamInput interface.
    /// @return Pointer to the SteamInput interface.
    [[nodiscard]] ISteamInput* GetSteamInput();

    /// @brief Retrieve the SteamInventory interface from the client.
    /// @return Pointer to the SteamInventory interface.
    [[nodiscard]] ISteamInventory* GetSteamClientInventory();

    /// @brief Retrieve the SteamInventory interface from the server.
    /// @return Pointer to the SteamInventory interface.
    [[nodiscard]] ISteamInventory* GetSteamServerInventory();

    /// @brief Retrieve the SteamMatchmaking interface.
    /// @return Pointer to the SteamMatchmaking interface.
    [[nodiscard]] ISteamMatchmaking* GetSteamMatchmaking();

    /// @brief Retrieve the SteamMatchmakingServers interface.
    /// @return Pointer to the SteamMatchmakingServers interface.
    [[nodiscard]] ISteamMatchmakingServers* GetSteamMatchmakingServers();

    /// @brief Retrieve the SteamMusic interface.
    /// @return Pointer to the SteamMusic interface.
    [[nodiscard]] ISteamMusic* GetSteamMusic();

    /// @brief Retrieve the SteamMusicRemote interface.
    /// @return Pointer to the SteamMusicRemote interface.
    [[nodiscard]] ISteamMusicRemote* GetSteamMusicRemote();

    /// @brief Retrieve the SteamNetworkingMessages interface from the client.
    /// @return Pointer to the SteamNetworkingMessages interface.
    [[nodiscard]] ISteamNetworkingMessages* GetSteamClientNetworkingMessages();

    /// @brief Retrieve the SteamNetworkingMessages interface from the server.
    /// @return Pointer to the SteamNetworkingMessages interface.
    [[nodiscard]] ISteamNetworkingMessages* GetSteamServerNetworkingMessages();

    /// @brief Retrieve the SteamNetworkingSockets interface from the client.
    /// @return Pointer to the SteamNetworkingSockets interface.
    [[nodiscard]] ISteamNetworkingSockets* GetSteamClientNetworkingSockets();

    /// @brief Retrieve the SteamNetworkingSockets interface from the server.
    /// @return Pointer to the SteamNetworkingSockets interface.
    [[nodiscard]] ISteamNetworkingSockets* GetSteamServerNetworkingSockets();

    /// @brief Retrieve the SteamNetworkingUtils interface.
    /// @return Pointer to the SteamNetworkingUtils interface.
    [[nodiscard]] ISteamNetworkingUtils* GetSteamNetworkingUtils();

    /// @brief Retrieve the SteamParties interface.
    /// @return Pointer to the SteamParties interface.
    [[nodiscard]] ISteamParties* GetSteamParties();

    /// @brief Retrieve the SteamRemotePlay interface.
    /// @return Pointer to the SteamRemotePlay interface.
    [[nodiscard]] ISteamRemotePlay* GetSteamRemotePlay();

    /// @brief Retrieve the SteamRemoteStorage interface.
    /// @return Pointer to the SteamRemoteStorage interface.
    [[nodiscard]] ISteamRemoteStorage* GetSteamRemoteStorage();

    /// @brief Retrieve the SteamScreenshots interface.
    /// @return Pointer to the SteamScreenshots interface.
    [[nodiscard]] ISteamScreenshots* GetSteamScreenshots();

    /// @brief Retrieve the SteamUGC interface from the client.
    /// @return Pointer to the SteamUGC interface.
    [[nodiscard]] ISteamUGC* GetSteamClientUGC();

    /// @brief Retrieve the SteamUGC interface from the server.
    /// @return Pointer to the SteamUGC interface.
    [[nodiscard]] ISteamUGC* GetSteamServerUGC();

    /// @brief Retrieve the SteamUser interface.
    /// @return Pointer to the SteamUser interface.
    [[nodiscard]] ISteamUser* GetSteamUser();

    /// @brief Retrieve the SteamUserStats interface.
    /// @return Pointer to the SteamUserStats interface.
    [[nodiscard]] ISteamUserStats* GetSteamUserStats();

    /// @brief Retrieve the SteamUtils interface from the client.
    /// @return Pointer to the SteamUtils interface.
    [[nodiscard]] ISteamUtils* GetSteamClientUtils();

    /// @brief Retrieve the SteamUtils interface from the server.
    /// @return Pointer to the SteamUtils interface.
    [[nodiscard]] ISteamUtils* GetSteamServerUtils();

    /// @brief Retrieve the SteamVideo interface.
    /// @return Pointer to the SteamVideo interface.
    [[nodiscard]] ISteamVideo* GetSteamVideo();

    /// @brief Retrieve the SteamTimeline interface.
    /// @return Pointer to the SteamTimeline interface.
    [[nodiscard]] ISteamTimeline* GetSteamTimeline();

    /// @brief Retrieve the SteamGameServer interface.
    /// @return Pointer to the SteamGameServer interface.
    [[nodiscard]] ISteamGameServer* GetSteamGameServer();

    /// @brief Retrieve the SteamGameServerStats interface.
    /// @return Pointer to the SteamGameServerStats interface.
    [[nodiscard]] ISteamGameServerStats* GetSteamGameServerStats();

#endif /*USE_STEAMWORKS_SDK*/
}

#endif /*TRAP_STEAMWORKSSDK_H*/
