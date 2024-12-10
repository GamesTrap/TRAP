#include "TRAPPCH.h"
#include "DiscordGameSDK.h"

#ifndef TRAP_HEADLESS_MODE

#include "Log/Log.h"

#include "Core/Base.h"

#ifdef USE_DISCORD_GAME_SDK

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
#include <discord.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

namespace
{
    //The Discord Game SDK is not thread safe according to https://discord.com/developers/docs/developer-tools/game-sdk#step-3-start-coding
    //So restrict access to the discord instance to a single thread at a time.
    TRAP::Utils::Safe<discord::Core*> DiscordGameSDKCore{};
    std::atomic<i64> CurrentAppID = TRAP::Utils::Discord::TRAPDiscordAppID;

    //Forward declares
    constexpr void DiscordLogger(discord::LogLevel logLevel, std::string_view msg);
    constexpr void DiscordLogResult(discord::Result res);

    void DefaultReconnectCallback()
    {
        if(CurrentAppID != TRAP::Utils::Discord::TRAPDiscordAppID)
            return;

        //Set activity to TRAP as default
        const std::string version = fmt::format("{}[{}]", TRAP::Log::WindowVersion, TRAP_VERSION);

        discord::Activity acti{};
        acti.SetDetails(version.c_str());
        acti.SetState("Developed by TrappedGames");
        auto& asset = acti.GetAssets();
        asset.SetLargeImage("trapwhitelogo2048x2048");
        asset.SetLargeText("TRAP™");

        auto core = DiscordGameSDKCore.WriteLock();
        if(*core != nullptr)
            (*core)->ActivityManager().UpdateActivity(acti, DiscordLogResult);
    }
}

#endif /*USE_DISCORD_GAME_SDK*/

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Discord::Create([[maybe_unused]] const i64 appID)
{
#ifdef USE_DISCORD_GAME_SDK
    TP_INFO(TRAP::Log::DiscordGameSDKPrefix, "Creating Discord instance");

    CurrentAppID = appID;

    //Set the default reconnect callback
    if(appID == TRAPDiscordAppID)
        *OnReconnect.WriteLock() = DefaultReconnectCallback;

    {
        auto core = DiscordGameSDKCore.WriteLock();

        const auto createRes = discord::Core::Create(appID, DiscordCreateFlags_NoRequireDiscord, &(*core)); //Don't require Discord to play the game
        if((*core == nullptr) || createRes != discord::Result::Ok)
        {
            DiscordLogResult(createRes);
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Failed to create Discord instance!");
            return false;
        }

        //Set log hook
        (*core)->SetLogHook(discord::LogLevel::Warn, DiscordLogger);
    }

    if(appID == TRAPDiscordAppID)
        DefaultReconnectCallback();

    return true;
#else
    return false;
#endif /*USE_DISCORD_GAME_SDK*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Discord::Destroy()
{
#ifdef USE_DISCORD_GAME_SDK
    TP_INFO(TRAP::Log::DiscordGameSDKPrefix, "Destroying Discord");
    *DiscordGameSDKCore.WriteLock() = nullptr; //Should implicitly call discord::Core's destructor
#endif /*USE_DISCORD_GAME_SDK*/
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Discord::RunCallbacks()
{
#ifdef USE_DISCORD_GAME_SDK
    bool hasReconnected = false;
    {
        auto core = DiscordGameSDKCore.WriteLock();

        if(*core != nullptr)
        {
            const auto callbackRes = (*core)->RunCallbacks();
            DiscordLogResult(callbackRes);

            if(callbackRes == discord::Result::NotRunning)
                *core = nullptr;

            return *core != nullptr;
        }

        //Try to reconnect with discord using the last set application ID
        const auto createRes = discord::Core::Create(CurrentAppID, DiscordCreateFlags_NoRequireDiscord, &*core);
        hasReconnected = createRes == discord::Result::Ok && *core != nullptr;
    }

    if(hasReconnected)
    {
        TP_INFO(TRAP::Log::DiscordGameSDKPrefix, "Reconnected Discord instance");
        const auto reconnectCallback = OnReconnect.ReadLock();
        if(*reconnectCallback)
            (*reconnectCallback)();

        return true;
    }

    return false;
#else
    return true;
#endif /*USE_DISCORD_GAME_SDK*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef USE_DISCORD_GAME_SDK
[[nodiscard]] TRAP::Utils::Safe<discord::Core*>& TRAP::Utils::Discord::GetDiscordCore() noexcept
{
    return DiscordGameSDKCore;
}
#endif /*USE_DISCORD_GAME_SDK*/

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
#ifdef USE_DISCORD_GAME_SDK
    /// @threadsafe
    constexpr void DiscordLogger(const discord::LogLevel logLevel, const std::string_view msg)
    {
        switch (logLevel)
        {
        case discord::LogLevel::Debug:
            TP_DEBUG(TRAP::Log::DiscordGameSDKPrefix, msg);
            break;

        case discord::LogLevel::Info:
            TP_INFO(TRAP::Log::DiscordGameSDKPrefix, msg);
            break;

        case discord::LogLevel::Warn:
            TP_WARN(TRAP::Log::DiscordGameSDKPrefix, msg);
            break;

        case discord::LogLevel::Error:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, msg);
            break;
        }
    }
#endif /*USE_DISCORD_GAME_SDK*/

    //-------------------------------------------------------------------------------------------------------------------//

#ifdef USE_DISCORD_GAME_SDK
    /// @threadsafe
    constexpr void DiscordLogResult(const discord::Result res)
    {
        switch(res)
        {
        case discord::Result::Ok:
            return;

        case discord::Result::ServiceUnavailable:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Service unavailable");
            break;
        case discord::Result::InvalidVersion:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid version");
            break;
        case discord::Result::LockFailed:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Lock failed");
            break;
        case discord::Result::InternalError:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Internal API error");
            break;
        case discord::Result::InvalidPayload:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid payload");
            break;
        case discord::Result::InvalidCommand:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid command");
            break;
        case discord::Result::InvalidPermissions:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid permissions");
            break;
        case discord::Result::NotFetched:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Not fetched");
            break;
        case discord::Result::NotFound:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Not found");
            break;
        case discord::Result::Conflict:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Conflict");
            break;
        case discord::Result::InvalidSecret:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid secret");
            break;
        case discord::Result::InvalidJoinSecret:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid join secret");
            break;
        case discord::Result::NoEligibleActivity:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "No eligible activity");
            break;
        case discord::Result::InvalidInvite:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid invite");
            break;
        case discord::Result::NotAuthenticated:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Not authenticated");
            break;
        case discord::Result::InvalidAccessToken:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid access token");
            break;
        case discord::Result::ApplicationMismatch:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Application mismatch");
            break;
        case discord::Result::InvalidDataUrl:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid data URL");
            break;
        case discord::Result::InvalidBase64:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid Base64");
            break;
        case discord::Result::NotFiltered:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Not filtered");
            break;
        case discord::Result::LobbyFull:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Lobby full");
            break;
        case discord::Result::InvalidLobbySecret:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid lobby secret");
            break;
        case discord::Result::InvalidFilename:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid filename");
            break;
        case discord::Result::InvalidFileSize:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid file size");
            break;
        case discord::Result::InvalidEntitlement:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid entitlement");
            break;
        case discord::Result::NotInstalled:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Not installed");
            break;
        case discord::Result::NotRunning:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Not running");
            break;
        case discord::Result::InsufficientBuffer:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Insufficient buffer");
            break;
        case discord::Result::PurchaseCanceled:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Purchase canceled");
            break;
        case discord::Result::InvalidGuild:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid guild");
            break;
        case discord::Result::InvalidEvent:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid event");
            break;
        case discord::Result::InvalidChannel:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid channel");
            break;
        case discord::Result::InvalidOrigin:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid origin");
            break;
        case discord::Result::RateLimited:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Rate limited");
            break;
        case discord::Result::OAuth2Error:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "OAuth2 error");
            break;
        case discord::Result::SelectChannelTimeout:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Select channel timeout");
            break;
        case discord::Result::GetGuildTimeout:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Get guild timeout");
            break;
        case discord::Result::SelectVoiceForceRequired:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Select voice force required");
            break;
        case discord::Result::CaptureShortcutAlreadyListening:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Capture shortcut already listening");
            break;
        case discord::Result::UnauthorizedForAchievement:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Unauthorized for achievement");
            break;
        case discord::Result::InvalidGiftCode:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid gift code");
            break;
        case discord::Result::PurchaseError:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Purchase error");
            break;
        case discord::Result::TransactionAborted:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Transaction aborted");
            break;
        case discord::Result::DrawingInitFailed:
            TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Drawing init failed");
            break;
        }
    }
#endif /*USE_DISCORD_GAME_SDK*/
}

#endif /*TRAP_HEADLESS_MODE*/
