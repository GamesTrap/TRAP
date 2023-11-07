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

    /// @brief Activity used by Discord Game SDK.
	/// @remark @headless This struct is not available in headless mode.
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

    /// @brief Create/Initialize Discord Game SDK.
    /// @param appID Discord App ID. Default TRAP™'s App ID.
    /// @return True on success, false otherwise
	/// @remark @headless This function is not available in headless mode.
    bool Create(i64 appID = TRAPDiscordAppID);
    /// @brief Destroy/Shutdown Discord Game SDK.
    void Destroy();
    /// @brief Runs Discord Game SDK updates.
    /// When Discord gets closed this function will also destroy the current
    /// initialized Discord Game SDK instance.
    /// @return True if Discord Game SDK is initialized, false otherwise
    /// @note This function also tries to reconnect to Discord if it was disconnected.
    ///       When this happens the last used activity will be set again.
	/// @remark @headless This function is not available in headless mode.
    bool RunCallbacks();
    /// @brief Set the activity of the game.
    /// @param activity Activity to be displayed.
    /// @return True on success, false otherwise
	/// @remark @headless This function is not available in headless mode.
    bool SetActivity(const Activity& activity);

#ifdef USE_DISCORD_GAME_SDK
    /// @brief Get the Discord Game SDK discord::Core* instance.
    /// @return Handle to internal Discord Game SDK discord::Core*, nullptr when not initialized.
	/// @remark @headless This function is not available in headless mode.
	/// @remark This function is only available when Discord Game SDK is provided.
    [[nodiscard]] discord::Core* GetDiscordCore() noexcept;
#endif /*USE_DISCORD_GAME_SDK*/
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_DISCORDGAMESDK_H*/