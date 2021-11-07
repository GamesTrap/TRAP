#include "TRAPPCH.h"
#include "DiscordGameSDK.h"

#include "Log/Log.h"

#include "Core/Base.h"

#ifdef USE_DISCORD_GAME_SDK

#include <discord.h>

discord::Core* core{};
discord::Result lastRes = discord::Result::Ok;
int64_t CurrentAppID = 639903785971613728;
TRAP::Utils::Discord::Activity CurrentActivity{};

//Forward declares
void DiscordLogger(const discord::LogLevel logLevel, const char* msg);
void DiscordLogResult(const discord::Result res);
#endif

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Discord::Create([[maybe_unused]] const int64_t appID)
{
#ifdef USE_DISCORD_GAME_SDK
    TP_INFO(TRAP::Log::DiscordGameSDKPrefix, "Creating Discord");
    lastRes = discord::Core::Create(appID, DiscordCreateFlags_NoRequireDiscord, &core);
    DiscordLogResult(lastRes);
    if(lastRes != discord::Result::Ok)
    {
        core = nullptr;
        return false;
    }

    CurrentAppID = appID;

    //Set log hook
    core->SetLogHook(discord::LogLevel::Warn, DiscordLogger);

    //Set activity to TRAP as default
    std::string version(TRAP::Log::WindowVersion);
    version += "[" + std::to_string(TRAP_VERSION_MAJOR(TRAP_VERSION)) + "." +
                     std::to_string(TRAP_VERSION_MINOR(TRAP_VERSION)) + "." +
                     std::to_string(TRAP_VERSION_PATCH(TRAP_VERSION)) + "]";

    CurrentActivity.LargeImage = "trapwhitelogo2048x2048";
    CurrentActivity.LargeText = u8"TRAP™";
    CurrentActivity.Details = version;
    CurrentActivity.State = "Developed by TrappedGames";

    return SetActivity(CurrentActivity);
#else
    return false;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Discord::Destroy()
{
#ifdef USE_DISCORD_GAME_SDK
    TP_INFO(TRAP::Log::DiscordGameSDKPrefix, "Destroying Discord");
    core = nullptr; //Should implicitly call discord::Core's destructor
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Discord::RunCallbacks()
{
#ifdef USE_DISCORD_GAME_SDK
    if(lastRes == discord::Result::Ok)
    {
        lastRes = core->RunCallbacks();

        DiscordLogResult(lastRes);
        if(lastRes != discord::Result::Ok)
        {
            TRAP::Utils::Discord::Destroy();
            return false;
        }

        return true;
    }

    //Try to reconnect
    lastRes = discord::Core::Create(CurrentAppID, DiscordCreateFlags_NoRequireDiscord, &core);
    if(lastRes != discord::Result::Ok)
    {
        core = nullptr;
        return false;
    }

    TRAP::Utils::Discord::SetActivity(CurrentActivity);

#endif

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Discord::SetActivity([[maybe_unused]] const Activity& activity)
{
#ifdef USE_DISCORD_GAME_SDK
    if(lastRes == discord::Result::Ok)
    {
        discord::Activity acti{};
        acti.SetDetails(activity.Details.c_str());
        acti.SetState(activity.State.c_str());
        auto& asset = acti.GetAssets();
        asset.SetLargeImage(activity.LargeImage.c_str());
        asset.SetLargeText(activity.LargeText.c_str());
        core->ActivityManager().UpdateActivity(acti, [](const discord::Result res)
        {
            lastRes = res;
            DiscordLogResult(lastRes);
            if(lastRes != discord::Result::Ok)
                TRAP::Utils::Discord::Destroy();
        });

        CurrentActivity = activity;

        return lastRes == discord::Result::Ok;
    }

#endif

    return false;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef USE_DISCORD_GAME_SDK
discord::Core* TRAP::Utils::Discord::GetDiscordCore()
{
    return core;
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

#ifdef USE_DISCORD_GAME_SDK
void DiscordLogger(const discord::LogLevel logLevel, const char* msg)
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

    default:
        break;
    }
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

#ifdef USE_DISCORD_GAME_SDK
void DiscordLogResult(const discord::Result res)
{
    switch(res)
    {
    // case discord::Result::Ok:
    //     TP_DEBUG(TRAP::Log::DiscordGameSDKPrefix, "Ok");
    //     break;
    case discord::Result::ServiceUnavailable:
        TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Service unavailable");
        break;
    case discord::Result::InvalidVersion:
        TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid version");
        break;
    case discord::Result::LockFailed:
    case discord::Result::InternalError:
        TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Internal API error");
        break;
    case discord::Result::InvalidPayload:
        TP_ERROR(TRAP::Log::DiscordGameSDKPrefix, "Invalid Payload");
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

    default:
        break;
    }
}
#endif