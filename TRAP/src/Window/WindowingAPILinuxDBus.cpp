#include "TRAPPCH.h"
#include "WindowingAPI.h"

#include "Core/PlatformDetection.h"

#if defined(TRAP_PLATFORM_LINUX) && !defined(TRAP_HEADLESS_MODE)

#include "Utils/String/String.h"
#include "Utils/DBus/DBus.h"

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::UpdateTaskbarProgressDBusPOSIX(const bool progressVisible, const f64 progressValue)
{
    const DBus::dbus_uint32_t progressVisibleu32 = static_cast<DBus::dbus_uint32_t>(progressVisible);

    //Signal signature:
    //signal com.canonical.Unity.LauncherEntry.Update (in s app_uri, in a{sv} properties)

    const TRAP::DBus::Message msg{"com.canonical.Unity.LauncherEntry", "Update"};

    if(!msg.IsValid())
        return;

    TRAP::Optional<TRAP::DBus::MessageIterator> args = msg.GetMessageIterator();
    if(!args)
        return;

    const char* const applicationURICStr = TRAP::DBus::GetApplicationURI().data();
    static constexpr const char* const progressVisibleCStr = "progress-visible";
    static constexpr const char* const progressCStr = "progress";

    //Setup app_uri parameter
    if(!args->AppendData(TRAP::DBus::Type::String, &applicationURICStr))
        return;

    //Set properties parameter
    {
        auto sub1(args->OpenContainer(TRAP::DBus::Type::Array, "{sv}"));
        if(!sub1)
            return;

        //Set progress visible property
        if(!sub1->AppendDictData(TRAP::DBus::Type::String, &progressVisibleCStr, TRAP::DBus::Type::Boolean, &progressVisibleu32))
            return;

        //Set progress value property
        if(!sub1->AppendDictData(TRAP::DBus::Type::String, &progressCStr, TRAP::DBus::Type::Double, &progressValue))
            return;
    }

    TRAP::DBus::SendMessage(msg);
}

#endif /*defined(TRAP_PLATFORM_LINUX) && !defined(TRAP_HEADLESS_MODE)*/
