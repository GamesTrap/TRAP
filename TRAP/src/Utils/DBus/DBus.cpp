#include "TRAPPCH.h"

#ifdef TRAP_PLATFORM_LINUX

#include "DBus.h"
#include "Utils/DynamicLoading/DynamicLoading.h"
#include "Utils/Expected.h"

namespace
{
    void* Handle = nullptr;

    std::string ApplicationURI{};
    std::string SignalName{};
    std::string BusName{};

    constexpr u32 DBUS_NAME_FLAG_ALLOW_REPLACEMENT = BIT(0u);
    constexpr u32 DBUS_NAME_FLAG_REPLACE_EXISTING = BIT(1u);
    constexpr u32 DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER = 1;

    TRAP::DBus::DBusConnection* s_connection = nullptr;
    TRAP::DBus::DBusError s_error{};

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] TRAP::Expected<std::string, std::string> GetExecutableName()
    {
	    ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                      (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

        std::error_code ec{};
        std::string exeName = std::filesystem::read_symlink("/proc/self/exe", ec).string();
        if(ec)
            return TRAP::MakeUnexpected(fmt::format("Failed to get name of the running executable ({})", ec.message()));

        const auto lastFoundSlash = exeName.find_last_of('/');

        if(lastFoundSlash == std::string::npos)
            return TRAP::MakeUnexpected(fmt::format("Failed to get name of the running executable ({})", ec.message()));

        return exeName.substr(lastFoundSlash + 1);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    //The executable name is stripped of any illegal characters according to the DBus specification
    [[nodiscard]] TRAP::Expected<std::string, std::string> GetLegalExecutableName()
    {
	    ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                      (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

        const auto exeName = GetExecutableName();

        if(exeName)
        {
            std::string exeNameFinal{};
            exeNameFinal.reserve(exeName->size());
            for(const char c : std::ranges::filter_view(*exeName, TRAP::Utils::String::IsAlphaNumeric))
                exeNameFinal.push_back(c);

            return exeNameFinal;
        }

        return exeName;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] TRAP::Expected<std::string, std::string> RetrieveApplicationURI()
    {
	    ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                      (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

        const auto exeName = GetExecutableName();

        if(exeName)
            return fmt::format("application://{}.desktop", *exeName);

        return exeName;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void Disconnect()
    {
        ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                      (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

        if(TRAP::DBus::IsConnected())
        {
            TRAP::DBus::ConnectionUnref(s_connection);
            s_connection = nullptr;
        }

        if(TRAP::DBus::IsErrorSet())
        {
            TRAP::DBus::ErrorFree(&s_error);
            s_error = {};
        }
    }

    //-------------------------------------------------------------------------------------------------------------------//

    bool Connect()
    {
        ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                      (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

        if((!TRAP::DBus::IsConnected() && (TRAP::DBus::SymbolsLoaded() || TRAP::DBus::LoadSymbols())))
        {
            TRAP::DBus::ErrorInit(&s_error);
            s_connection = TRAP::DBus::BusGet(TRAP::DBus::BusType::Session, &s_error);

            if(TRAP::DBus::IsErrorSet() || s_connection == nullptr)
            {
                Disconnect();
                TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Failed to connect to DBus");
                return false;
            }

            const i32 res = TRAP::DBus::BusRequestName(s_connection, BusName.c_str(), DBUS_NAME_FLAG_ALLOW_REPLACEMENT | DBUS_NAME_FLAG_REPLACE_EXISTING, &s_error);

            if(TRAP::DBus::IsErrorSet() || res != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
            {
                Disconnect();
                TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Failed to request DBus name \"", BusName, "\"");
                return false;
            }

            return true;
        }

        return false;
    }
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::DBus::LoadSymbols()
{
    ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    Handle = TRAP::Utils::DynamicLoading::LoadLibrary("libdbus-1.so.3");
    if(Handle == nullptr)
    {
        TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Failed to load DBus");
        return false;
    }

    ErrorInit = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusErrorInit>(Handle, "dbus_error_init");
    ErrorIsSet = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusErrorIsSet>(Handle, "dbus_error_is_set");
    ErrorFree = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusErrorFree>(Handle, "dbus_error_free");
    ConnectionUnref = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusConnectionUnref>(Handle, "dbus_connection_unref");
    ConnectionSend = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusConnectionSend>(Handle, "dbus_connection_send");
    ConnectionFlush = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusConnectionFlush>(Handle, "dbus_connection_flush");
    BusRequestName = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusBusRequestName>(Handle, "dbus_bus_request_name");
    BusGet = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusBusGet>(Handle, "dbus_bus_get");
    MessageUnref = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageUnref>(Handle, "dbus_message_unref");
    MessageNewSignal = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageNewSignal>(Handle, "dbus_message_new_signal");
    MessageIterInitAppend = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageIterInitAppend>(Handle, "dbus_message_iter_init_append");
    MessageIterAppendBasic = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageIterAppendBasic>(Handle, "dbus_message_iter_append_basic");
    MessageIterOpenContainer = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageIterOpenContainer>(Handle, "dbus_message_iter_open_container");
    MessageIterCloseContainer = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageIterCloseContainer>(Handle, "dbus_message_iter_close_container");

    if ((ErrorInit == nullptr) ||
        (ErrorIsSet == nullptr) ||
        (ErrorFree == nullptr) ||
        (ConnectionUnref == nullptr) ||
        (ConnectionSend == nullptr) ||
        (ConnectionFlush == nullptr) ||
        (BusRequestName == nullptr) ||
        (BusGet == nullptr) ||
        (MessageUnref == nullptr) ||
        (MessageNewSignal == nullptr) ||
        (MessageIterInitAppend == nullptr) ||
        (MessageIterAppendBasic == nullptr) ||
        (MessageIterOpenContainer == nullptr) ||
        (MessageIterCloseContainer == nullptr))
    {
        TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Failed to load DBus entry points");
        return false;
    }

    if(const auto legalExeName = GetLegalExecutableName())
    {
        const pid_t pid = getpid();
        BusName = fmt::format("com.trap.{}_{}", *legalExeName, pid);
        SignalName = fmt::format("/com/trap/{}_{}", *legalExeName, pid);
    }
    else
    {
        TP_ERROR(TRAP::Log::UtilsDBusPrefix, legalExeName.Error());
        return false;
    }

    if(const auto applicationURI = RetrieveApplicationURI())
        ApplicationURI = *applicationURI;
    else
    {
        TP_ERROR(TRAP::Log::UtilsDBusPrefix, applicationURI.Error());
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::DBus::UnloadSymbols()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    if(!SymbolsLoaded())
        return;

    Disconnect();

    TRAP::Utils::DynamicLoading::FreeLibrary(Handle);
    Handle = nullptr;

    ErrorInit = nullptr;
    ErrorIsSet = nullptr;
    ErrorFree = nullptr;
    ConnectionUnref = nullptr;
    ConnectionSend = nullptr;
    ConnectionFlush = nullptr;
    BusRequestName = nullptr;
    BusGet = nullptr;
    MessageUnref = nullptr;
    MessageNewSignal = nullptr;
    MessageIterInitAppend = nullptr;
    MessageIterAppendBasic = nullptr;
    MessageIterOpenContainer = nullptr;
    MessageIterCloseContainer = nullptr;

    ApplicationURI.clear();
    SignalName.clear();
    BusName.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::DBus::SymbolsLoaded() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    return Handle != nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::DBus::IsErrorSet()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    return ErrorIsSet(&s_error) != 0u;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<const TRAP::DBus::DBusError&> TRAP::DBus::GetError()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    if (IsErrorSet())
        return s_error;

    return TRAP::NullOpt;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::DBus::IsConnected() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    return s_connection != nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<const TRAP::DBus::DBusConnection&> TRAP::DBus::GetConnection()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    if(IsConnected())
        return *s_connection;

    return TRAP::NullOpt;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string_view TRAP::DBus::GetApplicationURI()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    return ApplicationURI;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::DBus::SendMessage(const Message& msg, const bool flush)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    u32 serial{};
    if(ConnectionSend(s_connection, msg.DBusMsg, &serial) == 0u)
    {
        TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Failed to send DBus message");
        return false;
    }

    if(flush)
        ConnectionFlush(s_connection);

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::DBus::MessageIterator::AppendData(const Type type, const void* const data)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    TRAP_ASSERT(data, "TRAP::DBus::MessageIterator::AppendData(): data is nullptr!");

    if(data == nullptr)
    {
        TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Data to append is nullptr!");
        return false;
    }

    if(type == TRAP::DBus::Type::Array || type == TRAP::DBus::Type::Variant || type == TRAP::DBus::Type::DictEntry ||
       type == TRAP::DBus::Type::StructOpen || type == TRAP::DBus::Type::StructClose)
    {
        TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Invalid DBus type provided");
        return false;
    }

    return MessageIterAppendBasic(&DBusMsgIter, std::to_underlying(type), data) != 0u;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::DBus::MessageIterator::AppendDictData(const TRAP::DBus::Type keyType, const void* const keyData,
                                                 const TRAP::DBus::Type valueType, const void* const valueData)
{
    auto keyIterator(OpenContainer(TRAP::DBus::Type::DictEntry, {}));
    if(!keyIterator)
        return false;

    //Append key data
    if(!keyIterator->AppendData(keyType, keyData))
        return false;

    {
        auto valueIterator(keyIterator->OpenContainer(TRAP::DBus::Type::Variant, reinterpret_cast<const char*>(&valueType)));
        if(!valueIterator)
            return false;

        //Append value data
        if(!valueIterator->AppendData(valueType, valueData))
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<TRAP::DBus::MessageIteratorContainer> TRAP::DBus::MessageIterator::OpenContainer(const Type type,
                                                                                                              const std::string_view signature)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    TRAP_ASSERT(!(signature.empty() && type != Type::DictEntry), "TRAP::DBus::MessageIterator::OpenContainer(): signature is empty!");

    if(signature.empty() && type != Type::DictEntry)
    {
        TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Signature is empty!");
        return TRAP::NullOpt;
    }

    if(type != TRAP::DBus::Type::Array && type != TRAP::DBus::Type::StructOpen &&
       type != TRAP::DBus::Type::StructClose && type != TRAP::DBus::Type::Variant &&
       type != TRAP::DBus::Type::DictEntry)
    {
        TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Invalid DBus container type provided");
        return TRAP::NullOpt;
    }

    DBusMessageIter dbusMsgSubIter{};

    if(TRAP::DBus::MessageIterOpenContainer(&DBusMsgIter, std::to_underlying(type), signature.data(), &dbusMsgSubIter) == 0u)
        return TRAP::NullOpt;

    return TRAP::MakeOptional<MessageIteratorContainer>(DBusMsgIter, std::move(dbusMsgSubIter));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::DBus::Message::Message(const std::string_view objectPath, const std::string_view interfaceName)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    TRAP_ASSERT(!objectPath.empty(), "TRAP::DBus::Message::Message(): objectPath is empty!");
    TRAP_ASSERT(!interfaceName.empty(), "TRAP::DBus::Message::Message(): interfaceName is empty!");

    if(objectPath.empty() || interfaceName.empty())
    {
        TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Invalid input parameters!");
        return;
    }

    if(!IsConnected())
        Connect();
    if(!IsConnected())
        return;

    DBusMsg = MessageNewSignal(SignalName.c_str(), objectPath.data(), interfaceName.data());

    if(DBusMsg == nullptr)
        TP_ERROR("Failed to create new DBus message (objectPath=\"", objectPath, "\", interfaceName=\"", interfaceName, "\")");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::DBus::Message::~Message()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    if(DBusMsg != nullptr)
        MessageUnref(DBusMsg);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<TRAP::DBus::MessageIterator> TRAP::DBus::Message::GetMessageIterator() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    if(!IsValid())
        return TRAP::NullOpt;

    DBusMessageIter msgIt{};

    TRAP::DBus::MessageIterInitAppend(DBusMsg, &msgIt);

    return TRAP::MakeOptional<MessageIterator>(std::move(msgIt));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::DBus::MessageIteratorContainer::~MessageIteratorContainer()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    if(DBusMsgSubIter != DBusMessageIter{})
        MessageIterCloseContainer(&DBusMsgMainIter.get(), &DBusMsgSubIter);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::DBus::MessageIteratorContainer::AppendData(const Type type, const void* const data)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    TRAP_ASSERT(data, "TRAP::DBus::MessageIteratorContainer::AppendData(): data is nullptr!");

    if(data == nullptr)
    {
        TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Data to append is nullptr!");
        return false;
    }

    if(type == TRAP::DBus::Type::Array || type == TRAP::DBus::Type::Variant || type == TRAP::DBus::Type::DictEntry ||
       type == TRAP::DBus::Type::StructOpen || type == TRAP::DBus::Type::StructClose)
    {
        TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Invalid DBus type provided");
        return false;
    }

    return MessageIterAppendBasic(&DBusMsgSubIter, std::to_underlying(type), data) != 0u;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::DBus::MessageIteratorContainer::AppendDictData(const TRAP::DBus::Type keyType, const void* const keyData,
                                                          const TRAP::DBus::Type valueType,
                                                          const void* const valueData)
{
    auto keyIterator(OpenContainer(TRAP::DBus::Type::DictEntry, {}));
    if(!keyIterator)
        return false;

    //Append key data
    if(!keyIterator->AppendData(keyType, keyData))
        return false;

    {
        auto valueIterator(keyIterator->OpenContainer(TRAP::DBus::Type::Variant, reinterpret_cast<const char*>(&valueType)));
        if(!valueIterator)
            return false;

        //Append value data
        if(!valueIterator->AppendData(valueType, valueData))
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<TRAP::DBus::MessageIteratorContainer> TRAP::DBus::MessageIteratorContainer::OpenContainer(const Type type,
                                                                                                                       const std::string_view signature)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    TRAP_ASSERT(!(signature.empty() && type != Type::DictEntry), "TRAP::DBus::MessageIteratorContainer::OpenContainer(): signature is empty!");

    if(signature.empty() && type != Type::DictEntry)
    {
        TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Signature is empty!");
        return TRAP::NullOpt;
    }

    if(type != TRAP::DBus::Type::Array && type != TRAP::DBus::Type::StructOpen &&
       type != TRAP::DBus::Type::StructClose && type != TRAP::DBus::Type::Variant &&
       type != TRAP::DBus::Type::DictEntry)
    {
        TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Invalid DBus container type provided");
        return TRAP::NullOpt;
    }

    DBusMessageIter dbusMsgSubIter{};

    if(TRAP::DBus::MessageIterOpenContainer(&DBusMsgSubIter, std::to_underlying(type), signature.data(), &dbusMsgSubIter) == 0u)
        return TRAP::NullOpt;

    return TRAP::MakeOptional<MessageIteratorContainer>(DBusMsgSubIter, std::move(dbusMsgSubIter));
}

#endif /*TRAP_PLATFORM_LINUX*/
