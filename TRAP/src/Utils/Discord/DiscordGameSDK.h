#ifndef TRAP_DISCORDGAMESDK_H
#define TRAP_DISCORDGAMESDK_H

#ifndef TRAP_HEADLESS_MODE

#include <string>

#include "Core/Types.h"

namespace discord
{
    class Core;
}

namespace TRAP::Utils::Discord
{
    constexpr i64 TRAPDiscordAppID = 639903785971613728;

    /// <summary>
    /// Activity used by Discord Game SDK.
    /// </summary>
    struct Activity
    {
        //Image name from Discord Developer portal.
        std::string LargeImage;
        //Text displayer when user hovers over the large image.
        std::string LargeText;
        //Description of the activity. For example, "Main menu".
        std::string Details;
        //Description of the current state for the activity. For example, "Lobby 1/4".
        std::string State;
    };

    /// <summary>
    /// Create/Initialize Discord Game SDK.
    /// </summary>
    /// <param name="appID">Discord App ID. Default TRAP™'s App ID.</param>
    /// <returns>True on success, false otherwise</returns>
    bool Create(i64 appID = TRAPDiscordAppID);
    /// <summary>
    /// Destroy/Shutdown Discord Game SDK.
    /// </summary>
    void Destroy();
    /// <summary>
    /// Runs Discord Game SDK updates.
    /// When Discord gets closed this function will also destroy the current
    /// initialized Discord Game SDK instance.
    /// Note: This function also tries to reconnect to Discord if it was disconnected.
    ///       When this happens the last activity will be used.
    /// </summary>
    /// <returns>True if Discord Game SDK is initialized, false otherwise</returns>
    bool RunCallbacks();
    /// <summary>
    /// Set the activity of the game.
    /// </summary>
    /// <param name="activity">Activity to be displayed.</param>
    /// <returns>True on success, false otherwise</returns>
    bool SetActivity(const Activity& activity);

#ifdef USE_DISCORD_GAME_SDK
    /// <summary>
    /// Get the Discord Game SDK discord::Core* instance.
    /// </summary>
    /// <returns>Handle to internal Discord Game SDK discord::Core*, nullptr when not initialized.</returns>
    [[nodiscard]] discord::Core* GetDiscordCore() noexcept;
#endif /*USE_DISCORD_GAME_SDK*/
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_DISCORDGAMESDK_H*/