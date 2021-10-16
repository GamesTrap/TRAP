#ifndef _TRAP_DISCORDGAMESDK_H_
#define _TRAP_DISCORDGAMESDK_H_

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
}

#endif /*_TRAP_DISCORDGAMESDK_H_*/