#ifndef TRAP_DISCORDGAMESDK_H
#define TRAP_DISCORDGAMESDK_H

namespace discord
{
    class Core;
}

namespace TRAP::Utils::Discord
{
    struct Activity
    {
        std::string LargeImage;
        std::string LargeText;
        std::string Details;
        std::string State;
    };

    bool Create(int64_t appID = 639903785971613728);
    void Destroy();
    bool RunCallbacks();
    bool SetActivity(const Activity& activity);

#ifdef USE_DISCORD_GAME_SDK
    discord::Core* GetDiscordCore();
#endif
}

#endif /*TRAP_DISCORDGAMESDK_H*/