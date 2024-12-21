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
    ///                It is used by the master server to detect out of date servers.
    /// @note This function stops the engine if it fails to initialize.
    void InitializeServer(u32 bindIPv4, u16 gamePort,
                          u16 queryPort, EServerMode authenticationMethod,
                          const std::string& version);
#endif /*USE_STEAMWORKS_SDK*/

    /// @brief Shutdown the Steamworks SDK.
    /// @threadsafe
    void Shutdown();

    /// @brief Dispatches callbacks and call results to all of the registered listeners.
    /// @note Callbacks can be executed on any thread so they must be thread-safe!
    /// @threadsafe
    void RunCallbacks();

    /// @brief Check whether the Steamworks SDK for clients is initialized or not.
    /// @return True if Steamworks SDK is initialized, false otherwise.
    /// @threadsafe
    [[nodiscard]] bool IsClientInitialized() noexcept;

    /// @brief Check whether the Steamworks SDK for servers is initialized or not.
    /// @return True if Steamworks SDK is initialized, false otherwise.
    /// @threadsafe
    [[nodiscard]] bool IsServerInitialized() noexcept;

#ifdef USE_STEAMWORKS_SDK

    /// @brief Retrieve the SteamApps interface.
    ///        Exposes a wide range of information and actions for applications and Downloadable Content (DLC).
    /// @return Pointer to the SteamApps interface.
    [[nodiscard]] ISteamApps* GetSteamApps();

    /// @brief Retrieve the SteamFriends interface.
    ///        Interface to access information about individual users and interact with the Steam Overlay.
    /// @return Pointer to the SteamFriends interface.
    [[nodiscard]] ISteamFriends* GetSteamFriends();

    /// @brief Retrieve the SteamHTMLSurface interface.
    ///        Interface for rendering and interacting with HTML pages.
    /// @return Pointer to the SteamHTMLSurface interface.
    [[nodiscard]] ISteamHTMLSurface* GetSteamHTMLSurface();

    /// @brief Retrieve the SteamHTTP interface from the client.
    ///        A small and easy to use HTTP client to send and receive data from the web.
    /// @return Pointer to the SteamHTTP interface.
    [[nodiscard]] ISteamHTTP* GetSteamClientHTTP();

    /// @brief Retrieve the SteamHTTP interface from the server.
    ///        A small and easy to use HTTP client to send and receive data from the web.
    /// @return Pointer to the SteamHTTP interface.
    [[nodiscard]] ISteamHTTP* GetSteamServerHTTP();

    /// @brief Retrieve the SteamInput interface.
    ///        Interface for supporting Xbox, PlayStation, Switch and Steam Controllers.
    /// @return Pointer to the SteamInput interface.
    [[nodiscard]] ISteamInput* GetSteamInput();

    /// @brief Retrieve the SteamInventory interface from the client.
    ///        Steam Inventory query and manipulation API.
    /// @return Pointer to the SteamInventory interface.
    [[nodiscard]] ISteamInventory* GetSteamClientInventory();

    /// @brief Retrieve the SteamInventory interface from the server.
    ///        Steam Inventory query and manipulation API.
    /// @return Pointer to the SteamInventory interface.
    [[nodiscard]] ISteamInventory* GetSteamServerInventory();

    /// @brief Retrieve the SteamMatchmaking interface.
    ///        Functions for clients to access matchmaking services, favorites, and to operate on game lobbies.
    /// @return Pointer to the SteamMatchmaking interface.
    [[nodiscard]] ISteamMatchmaking* GetSteamMatchmaking();

    /// @brief Retrieve the SteamMatchmakingServers interface.
    ///        Functions which provide access to the game server browser.
    /// @return Pointer to the SteamMatchmakingServers interface.
    [[nodiscard]] ISteamMatchmakingServers* GetSteamMatchmakingServers();

    /// @brief Retrieve the SteamMusic interface.
    ///        Functions to control music playback in the steam client.
    /// @return Pointer to the SteamMusic interface.
    [[nodiscard]] ISteamMusic* GetSteamMusic();

    /// @brief Retrieve the SteamMusicRemote interface.
    ///        Allows direct interaction with the Steam Music player.
    /// @return Pointer to the SteamMusicRemote interface.
    [[nodiscard]] ISteamMusicRemote* GetSteamMusicRemote();

    /// @brief Retrieve the SteamNetworkingMessages interface from the client.
    /// @return Pointer to the SteamNetworkingMessages interface.
    [[nodiscard]] ISteamNetworkingMessages* GetSteamClientNetworkingMessages();

    /// @brief Retrieve the SteamNetworkingMessages interface from the server.
    /// @return Pointer to the SteamNetworkingMessages interface.
    [[nodiscard]] ISteamNetworkingMessages* GetSteamServerNetworkingMessages();

    /// @brief Retrieve the SteamNetworkingSockets interface from the client.
    ///        Newer networking API. Higher performance API that relays data through the Valve network and protects player IP addresses.
    /// @return Pointer to the SteamNetworkingSockets interface.
    [[nodiscard]] ISteamNetworkingSockets* GetSteamClientNetworkingSockets();

    /// @brief Retrieve the SteamNetworkingSockets interface from the server.
    ///        Newer networking API. Higher performance API that relays data through the Valve network and protects player IP addresses.
    /// @return Pointer to the SteamNetworkingSockets interface.
    [[nodiscard]] ISteamNetworkingSockets* GetSteamServerNetworkingSockets();

    /// @brief Retrieve the SteamNetworkingUtils interface.
    ///        Misc networking utilities useful when using ISteamNetworkingSockets.
    /// @return Pointer to the SteamNetworkingUtils interface.
    [[nodiscard]] ISteamNetworkingUtils* GetSteamNetworkingUtils();

    /// @brief Retrieve the SteamParties interface.
    /// @return Pointer to the SteamParties interface.
    [[nodiscard]] ISteamParties* GetSteamParties();

    /// @brief Retrieve the SteamRemotePlay interface.
    ///        Functions for getting information about Steam Remote Play sessions.
    /// @return Pointer to the SteamRemotePlay interface.
    [[nodiscard]] ISteamRemotePlay* GetSteamRemotePlay();

    /// @brief Retrieve the SteamRemoteStorage interface.
    ///        Provides functions for reading, writing, and accessing files which can be stored remotely in the Steam Cloud.
    /// @return Pointer to the SteamRemoteStorage interface.
    [[nodiscard]] ISteamRemoteStorage* GetSteamRemoteStorage();

    /// @brief Retrieve the SteamScreenshots interface.
    ///        Functions for adding screenshots to the user's screenshot library.
    /// @return Pointer to the SteamScreenshots interface.
    [[nodiscard]] ISteamScreenshots* GetSteamScreenshots();

    /// @brief Retrieve the SteamUGC interface from the client.
    ///        Functions to create, consume, and interact with the Steam Workshop.
    /// @return Pointer to the SteamUGC interface.
    [[nodiscard]] ISteamUGC* GetSteamClientUGC();

    /// @brief Retrieve the SteamUGC interface from the server.
    ///        Functions to create, consume, and interact with the Steam Workshop.
    /// @return Pointer to the SteamUGC interface.
    [[nodiscard]] ISteamUGC* GetSteamServerUGC();

    /// @brief Retrieve the SteamUser interface.
    ///        Functions for accessing and manipulating Steam user information.
    /// @return Pointer to the SteamUser interface.
    [[nodiscard]] ISteamUser* GetSteamUser();

    /// @brief Retrieve the SteamUserStats interface.
    ///        Provides functions for accessing and submitting stats, achievements, and leaderboards.
    /// @return Pointer to the SteamUserStats interface.
    [[nodiscard]] ISteamUserStats* GetSteamUserStats();

    /// @brief Retrieve the SteamUtils interface from the client.
    ///        Interface which provides access to a range of miscellaneous utility functions.
    /// @return Pointer to the SteamUtils interface.
    [[nodiscard]] ISteamUtils* GetSteamClientUtils();

    /// @brief Retrieve the SteamUtils interface from the server.
    ///        Interface which provides access to a range of miscellaneous utility functions.
    /// @return Pointer to the SteamUtils interface.
    [[nodiscard]] ISteamUtils* GetSteamServerUtils();

    /// @brief Retrieve the SteamVideo interface.
    ///        Provides functions to interface with the Steam video and broadcasting platforms.
    /// @return Pointer to the SteamVideo interface.
    [[nodiscard]] ISteamVideo* GetSteamVideo();

    /// @brief Retrieve the SteamTimeline interface.
    ///        Functions for adding events to the game recording timeline.
    /// @return Pointer to the SteamTimeline interface.
    [[nodiscard]] ISteamTimeline* GetSteamTimeline();

    /// @brief Retrieve the SteamGameServer interface.
    ///        Provides the core of the Steam Game Servers API.
    /// @return Pointer to the SteamGameServer interface.
    [[nodiscard]] ISteamGameServer* GetSteamGameServer();

    /// @brief Retrieve the SteamGameServerStats interface.
    ///        Functions to allow game servers to set stats and achievements on players.
    /// @return Pointer to the SteamGameServerStats interface.
    [[nodiscard]] ISteamGameServerStats* GetSteamGameServerStats();

#endif /*USE_STEAMWORKS_SDK*/
}

#endif /*TRAP_STEAMWORKSSDK_H*/
