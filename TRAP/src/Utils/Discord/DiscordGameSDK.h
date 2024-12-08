#ifndef TRAP_DISCORDGAMESDK_H
#define TRAP_DISCORDGAMESDK_H

#ifndef TRAP_HEADLESS_MODE

#include <functional>

#include "Core/Types.h"
#include "Utils/Concurrency/Safe.h"

#ifdef USE_DISCORD_GAME_SDK

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
#include <discord.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#endif /*USE_DISCORD_GAME_SDK*/

namespace TRAP::Utils::Discord
{
    constexpr i64 TRAPDiscordAppID = 639903785971613728;

    /// @brief Create/Initialize Discord Game SDK.
    /// @param appID Discord App ID. Default TRAP™'s App ID.
    /// @return True on success, false otherwise
	/// @remark @headless This function is not available in headless mode.
    bool Create(i64 appID = TRAPDiscordAppID);
    /// @brief Destroy/Shutdown Discord Game SDK.
	/// @remark @headless This function is not available in headless mode.
    void Destroy();
    /// @brief Runs Discord Game SDK updates.
    /// @return True if Discord Game SDK is initialized, false otherwise
    /// @note This function also tries to reconnect to Discord if it was disconnected.
    ///       When this happens the OnReconnect callback gets called.
	/// @remark @headless This function is not available in headless mode.
    bool RunCallbacks();

    /// @brief Notify that the engine reconnected with Discord.
    ///        The callback gets called when Discord gets closed and reopened while the engine is running.
    /// @note The new connection will use the appID of the previous connection.
    /// @note The callback may be called by from any thread, so make sure that the provided function is threadsafe.
    /// @threadsafe
    inline TRAP::Utils::Safe<std::function<void()>> OnReconnect{};

#ifdef USE_DISCORD_GAME_SDK
    /// @brief Get the Discord Game SDK discord::Core* instance.
    /// @return Handle to internal Discord Game SDK discord::Core*, nullptr when not initialized or not available.
	/// @remark @headless This function is not available in headless mode.
	/// @remark This function is only available when Discord Game SDK is provided during compilation.
    /// @threadsafe
    [[nodiscard]] TRAP::Utils::Safe<discord::Core*>& GetDiscordCore() noexcept;
#endif /*USE_DISCORD_GAME_SDK*/
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_DISCORDGAMESDK_H*/
