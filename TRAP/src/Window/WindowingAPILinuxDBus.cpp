#include "TRAPPCH.h"
#include "WindowingAPI.h"

#include "Core/PlatformDetection.h"

#ifdef TRAP_PLATFORM_LINUX

#include "Utils/DynamicLoading/DynamicLoading.h"

void TRAP::INTERNAL::WindowingAPI::InitDBusPOSIX()
{
    //Initialize DBus library functions
    s_Data.DBUS.Handle = nullptr;
    s_Data.DBUS.Connection = nullptr;

    s_Data.DBUS.Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libdbus-1.so.3");
    if (s_Data.DBUS.Handle == nullptr)
    {
        InputError(Error::Platform_Error, "[DBus] Failed to load DBus");
        return;
    }

    s_Data.DBUS.ErrorInit = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusErrorInit>(s_Data.DBUS.Handle, "dbus_error_init");
    s_Data.DBUS.ErrorIsSet = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusErrorIsSet>(s_Data.DBUS.Handle, "dbus_error_is_set");
    s_Data.DBUS.ErrorFree = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusErrorFree>(s_Data.DBUS.Handle, "dbus_error_free");
    s_Data.DBUS.ConnectionUnref = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusConnectionUnref>(s_Data.DBUS.Handle, "dbus_connection_unref");
    s_Data.DBUS.ConnectionSend = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusConnectionSend>(s_Data.DBUS.Handle, "dbus_connection_send");
    s_Data.DBUS.ConnectionFlush = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusConnectionFlush>(s_Data.DBUS.Handle, "dbus_connection_flush");
    s_Data.DBUS.BusRequestName = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusBusRequestName>(s_Data.DBUS.Handle, "dbus_bus_request_name");
    s_Data.DBUS.BusGet = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusBusGet>(s_Data.DBUS.Handle, "dbus_bus_get");
    s_Data.DBUS.MessageUnref = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageUnref>(s_Data.DBUS.Handle, "dbus_message_unref");
    s_Data.DBUS.MessageNewSignal = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageNewSignal>(s_Data.DBUS.Handle, "dbus_message_new_signal");
    s_Data.DBUS.MessageIterInitAppend = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageIterInitAppend>(s_Data.DBUS.Handle, "dbus_message_iter_init_append");
    s_Data.DBUS.MessageIterAppendBasic = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageIterAppendBasic>(s_Data.DBUS.Handle, "dbus_message_iter_append_basic");
    s_Data.DBUS.MessageIterOpenContainer = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageIterOpenContainer>(s_Data.DBUS.Handle, "dbus_message_iter_open_container");
    s_Data.DBUS.MessageIterCloseContainer = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageIterCloseContainer>(s_Data.DBUS.Handle, "dbus_message_iter_close_container");

    if ((s_Data.DBUS.ErrorInit == nullptr) ||
        (s_Data.DBUS.ErrorIsSet == nullptr) ||
        (s_Data.DBUS.ErrorFree == nullptr) ||
        (s_Data.DBUS.ConnectionUnref == nullptr) ||
        (s_Data.DBUS.ConnectionSend == nullptr) ||
        (s_Data.DBUS.ConnectionFlush == nullptr) ||
        (s_Data.DBUS.BusRequestName == nullptr) ||
        (s_Data.DBUS.BusGet == nullptr) ||
        (s_Data.DBUS.MessageUnref == nullptr) ||
        (s_Data.DBUS.MessageNewSignal == nullptr) ||
        (s_Data.DBUS.MessageIterInitAppend == nullptr) ||
        (s_Data.DBUS.MessageIterAppendBasic == nullptr) ||
        (s_Data.DBUS.MessageIterOpenContainer == nullptr) ||
        (s_Data.DBUS.MessageIterCloseContainer == nullptr))
    {
        InputError(Error::Platform_Error, "[DBus] Failed to load DBus entry points");
        return;
    }

    //Initialize DBus connection
    s_Data.DBUS.ErrorInit(&s_Data.DBUS.Error);
    s_Data.DBUS.Connection = s_Data.DBUS.BusGet(DBusBusType::DBUS_BUS_SESSION, &s_Data.DBUS.Error);

    //Check for errors
    if((s_Data.DBUS.ErrorIsSet(&s_Data.DBUS.Error) != 0u) || (s_Data.DBUS.Connection == nullptr))
    {
        if(s_Data.DBUS.ErrorIsSet(&s_Data.DBUS.Error) != 0u)
            s_Data.DBUS.ErrorFree(&s_Data.DBUS.Error);

        InputError(Error::Platform_Error, "[DBus] Failed to connect to DBus");

        s_Data.DBUS.ConnectionUnref(s_Data.DBUS.Connection);
        s_Data.DBUS.Connection = nullptr;
        return;
    }

    //Request name

    CacheLegalExecutableNameDBusPOSIX();
    if(s_Data.DBUS.LegalExecutableName.empty())
        return;

    //"com.trap.<exe_name>_<pid>"
    const pid_t pid = getpid();
    const std::string busName = fmt::format("com.trap.{}_{}", s_Data.DBUS.LegalExecutableName, pid);

    const int32_t res = s_Data.DBUS.BusRequestName(s_Data.DBUS.Connection, busName.c_str(), DBUS_NAME_FLAG_REPLACE_EXISTING, &s_Data.DBUS.Error);

    //Check for errors
    if((s_Data.DBUS.ErrorIsSet(&s_Data.DBUS.Error) != 0u) || res != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
    {
        if(s_Data.DBUS.ErrorIsSet(&s_Data.DBUS.Error) != 0u)
            s_Data.DBUS.ErrorFree(&s_Data.DBUS.Error);

        InputError(Error::Platform_Error, "[DBus] Failed to request DBus name");

        s_Data.DBUS.ConnectionUnref(s_Data.DBUS.Connection);
        s_Data.DBUS.Connection = nullptr;
        return;
    }

    CacheFullExecutableNameDBusPOSIX();
    CacheDesktopFilePathDBusPOSIX();
    CacheSignalNameDBusPOSIX();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::CacheSignalNameDBusPOSIX()
{
    if(s_Data.DBUS.LegalExecutableName.empty())
        return;

    //"/com/trap/<exe_name>_<pid>"
    const pid_t pid = getpid();
    s_Data.DBUS.SignalName = fmt::format("/com/trap/{}_{}", s_Data.DBUS.LegalExecutableName, pid);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::CacheFullExecutableNameDBusPOSIX()
{
    std::string exeName(PATH_MAX, '\0');
    if(readlink("/proc/self/exe", exeName.data(), exeName.size()) == -1)
    {
        InputError(Error::Platform_Error, "[DBus] Failed to get name of the running executable");
        return;
    }

    const auto exeNameEnd = exeName.find_first_of('\0');
    const auto lastFoundSlash = exeName.find_last_of('/');

    if(lastFoundSlash == std::string::npos || exeNameEnd == std::string::npos || (lastFoundSlash + 1) >= exeNameEnd)
    {
        InputError(Error::Platform_Error, "[DBus] Failed to get name of the running executable");
        return;
    }

    s_Data.DBUS.FullExecutableName = exeName.substr(lastFoundSlash + 1, (exeNameEnd - lastFoundSlash - 1));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::CacheLegalExecutableNameDBusPOSIX()
{
    //The executable name is stripped of any illegal characters
    //according to the DBus specification

    std::string exeName(PATH_MAX, '\0');
    if(readlink("/proc/self/exe", exeName.data(), exeName.size()) == -1)
    {
        InputError(Error::Platform_Error, "[DBus] Failed to get name of the running executable");
        return;
    }

    const auto exeNameEnd = exeName.find_first_of('\0');
    const auto lastFoundSlash = exeName.find_last_of('/');

    if(lastFoundSlash == std::string::npos || exeNameEnd == std::string::npos || (lastFoundSlash + 1) >= exeNameEnd)
    {
        InputError(Error::Platform_Error, "[DBus] Failed to get name of the running executable");
        return;
    }

    std::string_view exeNameIntermediate = std::string_view(exeName).substr(lastFoundSlash + 1, exeNameEnd - lastFoundSlash - 1);

    std::string exeNameFinal{};
    exeNameFinal.reserve(exeNameIntermediate.size());
    for(const char c : exeNameIntermediate)
    {
        if(Utils::String::IsAlphaNumeric(c))
            exeNameFinal.push_back(c);
    }

    s_Data.DBUS.LegalExecutableName = exeNameFinal;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::CacheDesktopFilePathDBusPOSIX()
{
    if(s_Data.DBUS.FullExecutableName.empty())
        return;

    //Cache path of .desktop file

    //Create our final desktop file uri
    //"application://<exe_name>.desktop"
    s_Data.DBUS.DesktopFilePath += "application://" + s_Data.DBUS.FullExecutableName + ".desktop";
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::TerminateDBusPOSIX()
{
    s_Data.DBUS.SignalName.clear();
    s_Data.DBUS.LegalExecutableName.clear();
    s_Data.DBUS.FullExecutableName.clear();
    s_Data.DBUS.DesktopFilePath.clear();

    if (s_Data.DBUS.Connection != nullptr)
    {
        s_Data.DBUS.ConnectionUnref(s_Data.DBUS.Connection);
        s_Data.DBUS.Connection = nullptr;
    }

    if (s_Data.DBUS.Handle != nullptr)
    {
        TRAP::Utils::DynamicLoading::FreeLibrary(s_Data.DBUS.Handle);
        s_Data.DBUS.Handle = nullptr;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::WindowingAPI::UpdateTaskbarProgressDBusPOSIX(const dbus_bool_t progressVisible,
                                                                  const double progressValue)
{
    DBusMessage* msg = nullptr;

    if(s_Data.DBUS.Handle == nullptr || s_Data.DBUS.Connection == nullptr ||
       s_Data.DBUS.DesktopFilePath.empty() || s_Data.DBUS.SignalName.empty())
    {
        return;
    }

    //Signal signature:
    //signal com.canonical.Unity.LauncherEntry.Update (in s app_uri, in a{sv} properties)

    DBusMessageIter args{};

    if(NewMessageSignalDBusPOSIX(s_Data.DBUS.SignalName, "com.canonical.Unity.LauncherEntry", "Update", &msg) == 0u)
        return;

    s_Data.DBUS.MessageIterInitAppend(msg, &args);

    //Setup app_uri parameter
    AppendDataDBusPOSIX(args, DBUS_TYPE_STRING, s_Data.DBUS.DesktopFilePath.c_str());

    //Set properties parameter
    DBusMessageIter sub1{};

    OpenContainerDBusPOSIX(args, DBUS_TYPE_ARRAY, "{sv}", sub1);

    //Set progress visible property
    const std::string_view progressVisibleStr = "progress-visible";
    AppendDictDataDBusPOSIX(sub1, DBUS_TYPE_STRING, progressVisibleStr.data(), DBUS_TYPE_BOOLEAN, &progressVisible);

    //Set progress value property
    const std::string_view progressStr = "progress";
    AppendDictDataDBusPOSIX(sub1, DBUS_TYPE_STRING, progressStr.data(), DBUS_TYPE_DOUBLE, &progressValue);

    CloseContainerDBusPOSIX(args, sub1);

    SendMessageDBusPOSIX(*msg);

    //Free the message
    s_Data.DBUS.MessageUnref(msg);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::dbus_bool_t TRAP::INTERNAL::WindowingAPI::NewMessageSignalDBusPOSIX(const std::string_view objectPath,
                                                                                                  const std::string_view interfaceName,
                                                                                                  const std::string_view signalName,
                                                                                                  DBusMessage** const outMessage)
{
    if(outMessage == nullptr)
    {
        InputError(Error::Platform_Error, "[DBus] Failed to create new DBus message, output message pointer is NULL");
        return 0u;
    }

    *outMessage = s_Data.DBUS.MessageNewSignal(objectPath.data(), interfaceName.data(), signalName.data());
    if((*outMessage) == nullptr)
    {
        *outMessage = nullptr;
        InputError(Error::Platform_Error, "[DBus] Failed to create new DBus message");
        return 0u;
    }

    return 1u;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::dbus_bool_t TRAP::INTERNAL::WindowingAPI::OpenContainerDBusPOSIX(DBusMessageIter& iterator,
                                                                                               const int32_t DBusType,
                                                                                               const std::string_view signature,
                                                                                               DBusMessageIter& subIterator)
{
    if(DBusType != DBUS_TYPE_ARRAY && DBusType != DBUS_TYPE_STRUCT_OPEN &&
       DBusType != DBUS_TYPE_STRUCT_CLOSE && DBusType != DBUS_TYPE_VARIANT &&
       DBusType != DBUS_TYPE_DICT_ENTRY)
    {
        InputError(Error::Platform_Error, "[DBus] Invalid DBUS container type provided");
        return 0u;
    }

    return s_Data.DBUS.MessageIterOpenContainer(&iterator, DBusType, signature.data(), &subIterator);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::dbus_bool_t TRAP::INTERNAL::WindowingAPI::CloseContainerDBusPOSIX(DBusMessageIter& iterator,
                                                                                                DBusMessageIter& subIterator)
{
    return s_Data.DBUS.MessageIterCloseContainer(&iterator, &subIterator);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::dbus_bool_t TRAP::INTERNAL::WindowingAPI::AppendDataDBusPOSIX(DBusMessageIter& iterator,
                                                                                            const int32_t DBusType,
                                                                                            const void* const data)
{
    if(DBusType == DBUS_TYPE_ARRAY || DBusType == DBUS_TYPE_VARIANT || DBusType == DBUS_TYPE_DICT_ENTRY ||
       DBusType == DBUS_TYPE_STRUCT_OPEN || DBusType == DBUS_TYPE_STRUCT_CLOSE)
    {
        InputError(Error::Platform_Error, "[DBus] Invalid DBus type provided");
        return 0u;
    }
    if(data == nullptr)
    {
        InputError(Error::Platform_Error, "[DBus] DBus data to append is NULL");
        return 0u;
    }

    return s_Data.DBUS.MessageIterAppendBasic(&iterator, DBusType, data);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::dbus_bool_t TRAP::INTERNAL::WindowingAPI::AppendDictDataDBusPOSIX(DBusMessageIter& iterator,
                                                                                                const int32_t keyType,
                                                                                                const void* const keyData,
                                                                                                const int32_t valueType,
                                                                                                const void* const valueData)
{
    DBusMessageIter keyIterator{};
    DBusMessageIter valueIterator{};

    if(OpenContainerDBusPOSIX(iterator, DBUS_TYPE_DICT_ENTRY, {}, keyIterator) == 0u)
        return 0u;

    //Append key data
    if(AppendDataDBusPOSIX(keyIterator, keyType, keyData) == 0u)
        return 0u;

    if(OpenContainerDBusPOSIX(keyIterator, DBUS_TYPE_VARIANT, reinterpret_cast<const char*>(&valueType), valueIterator) == 0u)
        return 0u;

    //Append value data
    if(AppendDataDBusPOSIX(valueIterator, valueType, valueData) == 0u)
        return 0u;

    if(CloseContainerDBusPOSIX(keyIterator, valueIterator) == 0u)
        return 0u;

    if(CloseContainerDBusPOSIX(iterator, keyIterator) == 0u)
        return 0u;

    return 1u;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::dbus_bool_t TRAP::INTERNAL::WindowingAPI::SendMessageDBusPOSIX(DBusMessage& message)
{
    uint32_t serial = 0;
    if(s_Data.DBUS.ConnectionSend(s_Data.DBUS.Connection, &message, &serial) == 0u)
    {
        InputError(Error::Platform_Error, "[DBus] Failed to send DBus message");
        return 0u;
    }

    s_Data.DBUS.ConnectionFlush(s_Data.DBUS.Connection);

    return 1u;
}

#endif /*TRAP_PLATFORM_LINUX*/
