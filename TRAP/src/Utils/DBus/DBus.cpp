#include "TRAPPCH.h"

#ifdef TRAP_PLATFORM_LINUX

#include "DBus.h"
#include "Utils/DynamicLoading/DynamicLoading.h"
#include "Utils/Expected.h"

namespace
{
    enum class BusType : i32 //Must be i32, used in libdbus.so
    {
        Session,
        System,
        Starter
    };

    struct DBusConnection final
    {
        constexpr DBusConnection() = default;
        constexpr ~DBusConnection() = default;

        consteval DBusConnection(const DBusConnection&) = delete;
        constexpr DBusConnection(DBusConnection&&) = default;
        consteval DBusConnection& operator=(const DBusConnection&) = delete;
        constexpr DBusConnection& operator=(DBusConnection&&) = default;
    };

    struct DBusError final
    {
        constexpr DBusError() = default;
        constexpr ~DBusError() = default;

        consteval DBusError(const DBusError&) = delete;
        constexpr DBusError(DBusError&&) = default;
        consteval DBusError& operator=(const DBusError&) = delete;
        constexpr DBusError& operator=(DBusError&&) = default;

        const char* Name = nullptr;
        const char* Message = nullptr;
        u32 Dummy1 : 1 = 0u;
        u32 Dummy2 : 1 = 0u;
        u32 Dummy3 : 1 = 0u;
        u32 Dummy4 : 1 = 0u;
        u32 Dummy5 : 1 = 0u;
        void* Padding1 = nullptr;
    };

    struct DBusMessage final
    {
        constexpr DBusMessage() = default;
        constexpr ~DBusMessage() = default;

        consteval DBusMessage(const DBusMessage&) = delete;
        constexpr DBusMessage(DBusMessage&&) = default;
        consteval DBusMessage& operator=(const DBusMessage&) = delete;
        constexpr DBusMessage& operator=(DBusMessage&&) = default;
    };

    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    class LibDBus final //libdbus.so symbols
    {
    public:
        constexpr LibDBus() = default;
        constexpr ~LibDBus() = default;

        constexpr LibDBus(const LibDBus&) = delete;
        constexpr LibDBus(LibDBus&&) = delete;

        constexpr LibDBus& operator=(const LibDBus&) = delete;
        constexpr LibDBus& operator=(LibDBus&&) = delete;

        using PFN_DBusThreadsInitDefault = TRAP::DBus::INTERNAL::dbus_bool_t(*)();
        using PFN_DBusErrorInit = void(*)(DBusError*);
        using PFN_DBusErrorIsSet = TRAP::DBus::INTERNAL::dbus_bool_t(*)(const DBusError*);
        using PFN_DBusErrorFree = void(*)(DBusError*);
        using PFN_DBusConnectionUnref = void(*)(DBusConnection*);
        using PFN_DBusConnectionSend = TRAP::DBus::INTERNAL::dbus_bool_t(*)(DBusConnection*, TRAP::DBus::INTERNAL::DBusMessage*, TRAP::DBus::INTERNAL::dbus_uint32_t*);
        using PFN_DBusConnectionFlush = void(*)(DBusConnection*);
        using PFN_DBusBusRequestName = i32(*)(DBusConnection*, const char*, u32, DBusError*);
        using PFN_DBusBusGet = DBusConnection*(*)(BusType, DBusError*);
        using PFN_DBusMessageUnref = void(*)(TRAP::DBus::INTERNAL::DBusMessage*);
        using PFN_DBusMessageNewSignal = TRAP::DBus::INTERNAL::DBusMessage*(*)(const char*, const char*, const char*);
        using PFN_DBusMessageIterInitAppend = void(*)(TRAP::DBus::INTERNAL::DBusMessage*, TRAP::DBus::INTERNAL::DBusMessageIter*);
        using PFN_DBusMessageIterAppendBasic = TRAP::DBus::INTERNAL::dbus_bool_t(*)(TRAP::DBus::INTERNAL::DBusMessageIter*, i32, const void*);
        using PFN_DBusMessageIterOpenContainer = TRAP::DBus::INTERNAL::dbus_bool_t(*)(TRAP::DBus::INTERNAL::DBusMessageIter*, i32, const char*, TRAP::DBus::INTERNAL::DBusMessageIter*);
        using PFN_DBusMessageIterCloseContainer = TRAP::DBus::INTERNAL::dbus_bool_t(*)(TRAP::DBus::INTERNAL::DBusMessageIter*, TRAP::DBus::INTERNAL::DBusMessageIter*);

        PFN_DBusThreadsInitDefault ThreadsInitDefault{}; //dbus_threads_init_default
        PFN_DBusErrorInit ErrorInit{}; //dbus_error_init
        PFN_DBusErrorIsSet ErrorIsSet{}; //dbus_error_is_set
        PFN_DBusErrorFree ErrorFree{}; //dbus_error_free
        PFN_DBusConnectionUnref ConnectionUnref{}; //dbus_connection_unref
        PFN_DBusConnectionSend ConnectionSend{}; //dbus_connection_send
        PFN_DBusConnectionFlush ConnectionFlush{}; //dbus_connection_flush
        PFN_DBusBusRequestName BusRequestName{}; //dbus_bus_request_name
        PFN_DBusBusGet BusGet{}; //dbus_bus_get
        PFN_DBusMessageUnref MessageUnref{}; //dbus_message_unref
        PFN_DBusMessageNewSignal MessageNewSignal{}; //dbus_message_new_signal
        PFN_DBusMessageIterInitAppend MessageIterInitAppend{}; //dbus_message_iter_init_append
        PFN_DBusMessageIterAppendBasic MessageIterAppendBasic{}; //dbus_message_iter_append_basic
        PFN_DBusMessageIterOpenContainer MessageIterOpenContainer{}; //dbus_message_iter_open_container
        PFN_DBusMessageIterCloseContainer MessageIterCloseContainer{}; //dbus_message_iter_close_container

        void UnloadSymbols();
        [[nodiscard]] constexpr bool AreSymbolsLoaded() const;
        [[nodiscard]] constexpr bool IsConnected() const;
        [[nodiscard]] bool Connect();
        bool SendMessage(TRAP::DBus::INTERNAL::DBusMessage& msg, bool flush) const;

        [[nodiscard]] constexpr const std::string& GetApplicationURI() const;
        [[nodiscard]] constexpr const std::string& GetSignalName() const;
        [[nodiscard]] constexpr const std::string& GetBusName() const;

    private:
        static constexpr u32 DBUS_NAME_FLAG_ALLOW_REPLACEMENT = BIT(0u);
        static constexpr u32 DBUS_NAME_FLAG_REPLACE_EXISTING = BIT(1u);
        static constexpr u32 DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER = 1u;

        [[nodiscard]] bool LoadSymbols();
        void Disconnect();

        /// @threadsafe
        [[nodiscard]] static TRAP::Expected<std::string, std::string> GetExecutableName();
        /// @threadsafe
        [[nodiscard]] static TRAP::Expected<std::string, std::string> GetLegalExecutableName();
        /// @threadsafe
        [[nodiscard]] static TRAP::Expected<std::string, std::string> RetrieveApplicationURI();

        std::string m_applicationURI{};
        std::string m_signalName{};
        std::string m_busName{};

        DBusConnection* m_connection = nullptr;

        void* m_DLLHandle = nullptr;
    };

    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] bool LibDBus::LoadSymbols()
    {
        m_DLLHandle = TRAP::Utils::DynamicLoading::LoadLibrary("libdbus-1.so.3");
        if(m_DLLHandle == nullptr)
        {
            TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Failed to load DBus");
            return false;
        }

        ThreadsInitDefault = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusThreadsInitDefault>(m_DLLHandle, "dbus_threads_init_default");
        ErrorInit = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusErrorInit>(m_DLLHandle, "dbus_error_init");
        ErrorIsSet = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusErrorIsSet>(m_DLLHandle, "dbus_error_is_set");
        ErrorFree = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusErrorFree>(m_DLLHandle, "dbus_error_free");
        ConnectionUnref = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusConnectionUnref>(m_DLLHandle, "dbus_connection_unref");
        ConnectionSend = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusConnectionSend>(m_DLLHandle, "dbus_connection_send");
        ConnectionFlush = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusConnectionFlush>(m_DLLHandle, "dbus_connection_flush");
        BusRequestName = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusBusRequestName>(m_DLLHandle, "dbus_bus_request_name");
        BusGet = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusBusGet>(m_DLLHandle, "dbus_bus_get");
        MessageUnref = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageUnref>(m_DLLHandle, "dbus_message_unref");
        MessageNewSignal = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageNewSignal>(m_DLLHandle, "dbus_message_new_signal");
        MessageIterInitAppend = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageIterInitAppend>(m_DLLHandle, "dbus_message_iter_init_append");
        MessageIterAppendBasic = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageIterAppendBasic>(m_DLLHandle, "dbus_message_iter_append_basic");
        MessageIterOpenContainer = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageIterOpenContainer>(m_DLLHandle, "dbus_message_iter_open_container");
        MessageIterCloseContainer = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_DBusMessageIterCloseContainer>(m_DLLHandle, "dbus_message_iter_close_container");

        if ((ThreadsInitDefault == nullptr) ||
            (ErrorInit == nullptr) ||
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
            UnloadSymbols();
            return false;
        }

        if(ThreadsInitDefault() == 0u)
        {
            TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Failed to initialize libdbus threads!");
            UnloadSymbols();
            return false;
        }

        if(const auto legalExeName = GetLegalExecutableName())
        {
            const pid_t pid = getpid();
            m_busName = fmt::format("com.trap.{}_{}", *legalExeName, pid);
            m_signalName = fmt::format("/com/trap/{}_{}", *legalExeName, pid);
        }
        else
        {
            TP_ERROR(TRAP::Log::UtilsDBusPrefix, legalExeName.Error());
            UnloadSymbols();
            return false;
        }

        if(const auto applicationURI = RetrieveApplicationURI())
            m_applicationURI = *applicationURI;
        else
        {
            TP_ERROR(TRAP::Log::UtilsDBusPrefix, applicationURI.Error());
            UnloadSymbols();
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void LibDBus::UnloadSymbols()
    {
        if(AreSymbolsLoaded() && IsConnected())
            Disconnect();

        if(m_DLLHandle != nullptr)
        {
            TRAP::Utils::DynamicLoading::FreeLibrary(m_DLLHandle);
            m_DLLHandle = nullptr;
        }

        ThreadsInitDefault = nullptr;
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

        m_applicationURI.clear();
        m_signalName.clear();
        m_busName.clear();
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] constexpr bool LibDBus::AreSymbolsLoaded() const
    {
        return m_DLLHandle != nullptr;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    void LibDBus::Disconnect()
    {
        ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                      (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

        if(!AreSymbolsLoaded())
            return;

        if(IsConnected())
        {
            ConnectionUnref(m_connection);
            m_connection = nullptr;
        }
    }


    //-------------------------------------------------------------------------------------------------------------------//

    /// @threadsafe
    [[nodiscard]] bool LibDBus::Connect()
    {
        ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                      (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

        if(!AreSymbolsLoaded() && !LoadSymbols())
            return false;

        //Already connected?
        if(IsConnected())
            return true;

        //Try to connect
        DBusError error{};
        ErrorInit(&error);
        m_connection = BusGet(BusType::Session, &error);

        bool errorSet = ErrorIsSet(&error) != 0u;
        if(errorSet || !IsConnected())
        {
            TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Failed to connect to DBus");

            if(errorSet)
            {
                TP_ERROR(TRAP::Log::UtilsDBusPrefix, error.Message);
                ErrorFree(&error);
            }

            return false;
        }

        //Try to request bus name
        const i32 res = BusRequestName(m_connection, m_busName.c_str(), DBUS_NAME_FLAG_ALLOW_REPLACEMENT | DBUS_NAME_FLAG_REPLACE_EXISTING, &error);

        errorSet = ErrorIsSet(&error) != 0u;
        if(errorSet || res != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
        {
            TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Failed to request DBus name \"", m_busName, "\"");

            if(errorSet)
            {
                TP_ERROR(TRAP::Log::UtilsDBusPrefix, error.Message);
                ErrorFree(&error);
            }

            Disconnect();
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    bool LibDBus::SendMessage(TRAP::DBus::INTERNAL::DBusMessage& msg, const bool flush) const
    {
        ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                      (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

        if(!AreSymbolsLoaded())
            return false;

        u32 serial{};
        if(ConnectionSend(m_connection, &msg, &serial) == 0u)
        {
            TP_ERROR(TRAP::Log::UtilsDBusPrefix, "Failed to send DBus message");
            return false;
        }

        if(flush)
            ConnectionFlush(m_connection);

        return true;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] constexpr const std::string& LibDBus::GetApplicationURI() const
    {
        return m_applicationURI;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] constexpr const std::string& LibDBus::GetSignalName() const
    {
        return m_signalName;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] constexpr const std::string& LibDBus::GetBusName() const
    {
        return m_busName;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] constexpr bool LibDBus::IsConnected() const
    {
        return m_connection != nullptr;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @threadsafe
    [[nodiscard]] TRAP::Expected<std::string, std::string> LibDBus::GetExecutableName()
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
    /// @threadsafe
    [[nodiscard]] TRAP::Expected<std::string, std::string> LibDBus::GetLegalExecutableName()
    {
	    ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                      (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

        const auto exeName = GetExecutableName();
        if(!exeName)
            return exeName;

        std::string exeNameFinal{};
        exeNameFinal.reserve(exeName->size());
        for(const char c : std::ranges::filter_view(*exeName, TRAP::Utils::String::IsAlphaNumeric))
            exeNameFinal.push_back(c);

        return exeNameFinal;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @threadsafe
    [[nodiscard]] TRAP::Expected<std::string, std::string> LibDBus::RetrieveApplicationURI()
    {
	    ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                      (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

        const auto exeName = GetExecutableName();

        if(exeName)
            return fmt::format("application://{}.desktop", *exeName);

        return exeName;
    }

    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] TRAP::Utils::Safe<LibDBus>& GetLibDBus()
    {
        try
        {
            static TRAP::Utils::Safe<LibDBus> libdbus{};
            return libdbus;
        }
        catch(...)
        {
			TP_CRITICAL(TRAP::Log::UtilsDBusPrefix, "TRAP::DBus::GetLibDBus(): TRAP::Utils::Safe<LibDBus> constructor threw an exception!");
			throw std::runtime_error("TRAP::DBus::GetLibDBus(): TRAP::Utils::Safe<LibDBus> constructor threw an exception!");
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//Shared wrapper code

namespace
{
    [[nodiscard]] bool AppendData(TRAP::DBus::INTERNAL::DBusMessageIter& msgIter, const TRAP::DBus::Type type, const void* const data)
    {
        ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                      (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

        TRAP_ASSERT(data, "TRAP::DBus::AppendData(): data is nullptr!");

        const auto dbus = GetLibDBus().ReadLock();

        if(!dbus->AreSymbolsLoaded())
            return false;

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

        return dbus->MessageIterAppendBasic(&msgIter, std::to_underlying(type), data) != 0u;
    }

    [[nodiscard]] TRAP::Optional<TRAP::DBus::MessageIteratorContainer> OpenContainer(TRAP::DBus::INTERNAL::DBusMessageIter& msgIter,
                                                                                     const TRAP::DBus::Type type,
                                                                                     const std::string& signature)
    {
        ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                      (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

        TRAP_ASSERT(!(signature.empty() && type != TRAP::DBus::Type::DictEntry), "TRAP::DBus::OpenContainer(): signature is empty!");

        const auto dbus = GetLibDBus().ReadLock();

        if(!dbus->AreSymbolsLoaded())
            return TRAP::NullOpt;

        if(signature.empty() && type != TRAP::DBus::Type::DictEntry)
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

        TRAP::DBus::INTERNAL::DBusMessageIter dbusMsgSubIter{};

        const char* const signatureCStr = signature.empty() ? nullptr : signature.c_str();

        if(dbus->MessageIterOpenContainer(&msgIter, std::to_underlying(type), signatureCStr, &dbusMsgSubIter) == 0u)
            return TRAP::NullOpt;

        return TRAP::MakeOptional<TRAP::DBus::MessageIteratorContainer>(msgIter, std::move(dbusMsgSubIter));
    }

    [[nodiscard]] bool AppendDictData(TRAP::DBus::INTERNAL::DBusMessageIter& msgIter,
                                      const TRAP::DBus::Type keyType, const void* const keyData,
                                      const TRAP::DBus::Type valueType, const void* const valueData)
    {
	    ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                      (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

        auto keyIterator(OpenContainer(msgIter, TRAP::DBus::Type::DictEntry, {}));
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
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

void TRAP::DBus::INTERNAL::UnloadSymbols()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    GetLibDBus().WriteLock()->UnloadSymbols();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::DBus::GetApplicationURI()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    return GetLibDBus().ReadLock()->GetApplicationURI();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::DBus::MessageIterator::AppendData(const Type type, const void* const data)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    return ::AppendData(m_DBusMsgIter, type, data);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::DBus::MessageIterator::AppendDictData(const TRAP::DBus::Type keyType, const void* const keyData,
                                                               const TRAP::DBus::Type valueType, const void* const valueData)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    return ::AppendDictData(m_DBusMsgIter, keyType, keyData, valueType, valueData);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<TRAP::DBus::MessageIteratorContainer> TRAP::DBus::MessageIterator::OpenContainer(const Type type,
                                                                                                              const std::string& signature)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    return ::OpenContainer(m_DBusMsgIter, type, signature);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::DBus::Message::Message(const std::string& objectPath, const std::string& interfaceName)
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

    auto dbus = GetLibDBus().WriteLock();

    if((!dbus->IsConnected() && !dbus->Connect()) || !dbus->AreSymbolsLoaded())
        return;

    m_DBusMsg = dbus->MessageNewSignal(dbus->GetSignalName().c_str(), objectPath.c_str(), interfaceName.c_str());

    if(m_DBusMsg == nullptr)
        TP_ERROR("Failed to create new DBus message (objectPath=\"", objectPath, "\", interfaceName=\"", interfaceName, "\")");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::DBus::Message::~Message()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    const auto dbus = GetLibDBus().ReadLock();

    if(m_DBusMsg != nullptr && dbus->AreSymbolsLoaded())
        dbus->MessageUnref(m_DBusMsg);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<TRAP::DBus::MessageIterator> TRAP::DBus::Message::GetMessageIterator() const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    const auto dbus = GetLibDBus().ReadLock();

    if(!IsValid() || !dbus->AreSymbolsLoaded())
        return TRAP::NullOpt;

    INTERNAL::DBusMessageIter msgIt{};

    dbus->MessageIterInitAppend(m_DBusMsg, &msgIt);

    return TRAP::MakeOptional<MessageIterator>(std::move(msgIt));
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::DBus::Message::SendMessage(const bool flush) const
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    if(m_DBusMsg == nullptr)
        return false;

    return GetLibDBus().ReadLock()->SendMessage(*m_DBusMsg, flush);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::DBus::MessageIteratorContainer::~MessageIteratorContainer()
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    const auto dbus = GetLibDBus().ReadLock();

    if(dbus->AreSymbolsLoaded() && m_DBusMsgSubIter != INTERNAL::DBusMessageIter{})
        dbus->MessageIterCloseContainer(&m_DBusMsgMainIter.get(), &m_DBusMsgSubIter);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::DBus::MessageIteratorContainer::AppendData(const Type type, const void* const data)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    return ::AppendData(m_DBusMsgSubIter, type, data);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::DBus::MessageIteratorContainer::AppendDictData(const TRAP::DBus::Type keyType, const void* const keyData,
                                                          const TRAP::DBus::Type valueType,
                                                          const void* const valueData)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    return ::AppendDictData(m_DBusMsgSubIter, keyType, keyData, valueType, valueData);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<TRAP::DBus::MessageIteratorContainer> TRAP::DBus::MessageIteratorContainer::OpenContainer(const Type type,
                                                                                                                       const std::string& signature)
{
	ZoneNamedC(__tracy, tracy::Color::DarkOrange, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
                                                  (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    return ::OpenContainer(m_DBusMsgSubIter, type, signature);
}

#endif /*TRAP_PLATFORM_LINUX*/
