#ifndef TRAP_DBUS_H
#define TRAP_DBUS_H

#include "Core/Base.h"

#ifdef TRAP_PLATFORM_LINUX

#include <string>

#include "Utils/Optional.h"

namespace TRAP::DBus
{
    class MessageIterator;
    class MessageIteratorContainer;

    using dbus_bool_t = u32;
    using dbus_uint32_t = u32;

    enum class BusType
    {
        Session,
        System,
        Starter
    };

    enum class Type : i32
    {
        String = 's',
        Array = 'a',
        DictEntry = 'e',
        Variant = 'v',
        Boolean = 'b',
        Double = 'd',
        Int16 = 'n',
        UInt16 = 'q',
        Int32 = 'i',
        UInt32 = 'u',
        Int64 = 'x',
        UInt64 = 't',
        StructOpen = '(',
        StructClose = ')',
        Byte = 'y',
        ObjectPath = 'o',
        Signature = 'g'
    };

    struct DBusConnection
    {
        constexpr DBusConnection() = default;
        constexpr ~DBusConnection() = default;

        consteval DBusConnection(const DBusConnection&) = delete;
        constexpr DBusConnection(DBusConnection&&) = default;
        consteval DBusConnection& operator=(const DBusConnection&) = delete;
        constexpr DBusConnection& operator=(DBusConnection&&) = default;
    };

    struct DBusError
    {
        constexpr DBusError() = default;
        constexpr ~DBusError() = default;

        consteval DBusError(const DBusError&) = delete;
        constexpr DBusError(DBusError&&) = default;
        consteval DBusError& operator=(const DBusError&) = delete;
        constexpr DBusError& operator=(DBusError&&) = default;

        const char* Name;
        const char* Message;
        u32 Dummy1 : 1;
        u32 Dummy2 : 1;
        u32 Dummy3 : 1;
        u32 Dummy4 : 1;
        u32 Dummy5 : 1;
        void* Padding1;
    };

    struct DBusMessage
    {
        constexpr DBusMessage() = default;
        constexpr ~DBusMessage() = default;

        consteval DBusMessage(const DBusMessage&) = delete;
        constexpr DBusMessage(DBusMessage&&) = default;
        consteval DBusMessage& operator=(const DBusMessage&) = delete;
        constexpr DBusMessage& operator=(DBusMessage&&) = default;
    };

    struct DBusMessageIter
    {
        constexpr DBusMessageIter() = default;
        constexpr ~DBusMessageIter() = default;

        consteval DBusMessageIter(const DBusMessageIter&) = delete;
        constexpr DBusMessageIter(DBusMessageIter&&) = default;
        consteval DBusMessageIter& operator=(const DBusMessageIter&) = delete;
        constexpr DBusMessageIter& operator=(DBusMessageIter&&) = default;

        void* Dummy1;
        void* Dummy2;
        dbus_uint32_t Dummy3;
        i32 Dummy4, Dummy5, Dummy6, Dummy7, Dummy8, Dummy9, Dummy10, Dummy11;
        i32 Pad1;
        void* Pad2;
        void* Pad3;

        constexpr bool operator==(const DBusMessageIter& dbusMsgIter) const = default;
    };

    //-------------------------------------------------------------------------------------------------------------------//

    class Message final
    {
    public:
        Message(std::string_view objectPath, std::string_view interfaceName);
        ~Message();

        consteval Message(const Message&) = delete;
        constexpr Message(Message&&) = default;

        consteval Message& operator=(const Message&) = delete;
        Message& operator=(Message&&) = default;

        [[nodiscard]] TRAP::Optional<MessageIterator> GetMessageIterator() const;

        [[nodiscard]] constexpr bool IsValid() const noexcept;

        DBusMessage* DBusMsg = nullptr;
    };

    //-------------------------------------------------------------------------------------------------------------------//

    class MessageIterator final
    {
    public:
        constexpr explicit MessageIterator(DBusMessageIter msgIter);
        constexpr ~MessageIterator() = default;

        consteval MessageIterator(const MessageIterator&) = delete;
        constexpr MessageIterator(MessageIterator&&) = default;
        consteval MessageIterator& operator=(const MessageIterator&) = delete;
        constexpr MessageIterator& operator=(MessageIterator&&) = default;

        bool AppendData(Type type, const void* data);
        bool AppendDictData(Type keyType, const void* keyData, Type valueType, const void* valueData);

        [[nodiscard]] TRAP::Optional<MessageIteratorContainer> OpenContainer(Type type, std::string_view signature);

        DBusMessageIter DBusMsgIter{};
    };

    //-------------------------------------------------------------------------------------------------------------------//

    class MessageIteratorContainer final
    {
    private:
        std::reference_wrapper<DBusMessageIter> DBusMsgMainIter;

    public:
        constexpr explicit MessageIteratorContainer(DBusMessageIter& mainIter, DBusMessageIter subIter);

        ~MessageIteratorContainer();

        consteval MessageIteratorContainer(const MessageIteratorContainer&) = delete;
        constexpr MessageIteratorContainer(MessageIteratorContainer&&) = default;
        consteval MessageIteratorContainer& operator=(const MessageIteratorContainer&) = delete;
        MessageIteratorContainer& operator=(MessageIteratorContainer&&) = default;

        bool AppendData(Type type, const void* data);
        bool AppendDictData(Type keyType, const void* keyData, Type valueType, const void* valueData);

        [[nodiscard]] TRAP::Optional<MessageIteratorContainer> OpenContainer(Type type, std::string_view signature);

        DBusMessageIter DBusMsgSubIter{};
    };

    //-------------------------------------------------------------------------------------------------------------------//

    bool SendMessage(const Message& msg, bool flush = false);

    bool LoadSymbols();
    void UnloadSymbols();
    [[nodiscard]] bool SymbolsLoaded() noexcept;

    [[nodiscard]] bool IsErrorSet();
    [[nodiscard]] TRAP::Optional<const DBusError&> GetError();

    [[nodiscard]] bool IsConnected() noexcept;
    [[nodiscard]] TRAP::Optional<const DBusConnection&> GetConnection();

    [[nodiscard]] std::string_view GetApplicationURI();

    //-------------------------------------------------------------------------------------------------------------------//

    using PFN_DBusErrorInit = void(*)(DBusError*);
    using PFN_DBusErrorIsSet = dbus_bool_t(*)(const DBusError*);
    using PFN_DBusErrorFree = void(*)(DBusError*);
    using PFN_DBusConnectionUnref = void(*)(DBusConnection*);
    using PFN_DBusConnectionSend = dbus_bool_t(*)(DBusConnection*, DBusMessage*, dbus_uint32_t*);
    using PFN_DBusConnectionFlush = void(*)(DBusConnection*);
    using PFN_DBusBusRequestName = i32(*)(DBusConnection*, const char*, u32, DBusError*);
    using PFN_DBusBusGet = DBusConnection*(*)(BusType, DBusError*);
    using PFN_DBusMessageUnref = void(*)(DBusMessage*);
    using PFN_DBusMessageNewSignal = DBusMessage*(*)(const char*, const char*, const char*);
    using PFN_DBusMessageIterInitAppend = void(*)(DBusMessage*, DBusMessageIter*);
    using PFN_DBusMessageIterAppendBasic = dbus_bool_t(*)(DBusMessageIter*, i32, const void*);
    using PFN_DBusMessageIterOpenContainer = dbus_bool_t(*)(DBusMessageIter*, i32, const char*, DBusMessageIter*);
    using PFN_DBusMessageIterCloseContainer = dbus_bool_t(*)(DBusMessageIter*, DBusMessageIter*);

    //-------------------------------------------------------------------------------------------------------------------//

    //Symbols
    inline PFN_DBusErrorInit ErrorInit{}; //dbus_error_init
    inline PFN_DBusErrorIsSet ErrorIsSet{}; //dbus_error_is_set
    inline PFN_DBusErrorFree ErrorFree{}; //dbus_error_free
    inline PFN_DBusConnectionUnref ConnectionUnref{}; //dbus_connection_unref
    inline PFN_DBusConnectionSend ConnectionSend{}; //dbus_connection_send
    inline PFN_DBusConnectionFlush ConnectionFlush{}; //dbus_connection_flush
    inline PFN_DBusBusRequestName BusRequestName{}; //dbus_bus_request_name
    inline PFN_DBusBusGet BusGet{}; //dbus_bus_get
    inline PFN_DBusMessageUnref MessageUnref{}; //dbus_message_unref
    inline PFN_DBusMessageNewSignal MessageNewSignal{}; //dbus_message_new_signal
    inline PFN_DBusMessageIterInitAppend MessageIterInitAppend{}; //dbus_message_iter_init_append
    inline PFN_DBusMessageIterAppendBasic MessageIterAppendBasic{}; //dbus_message_iter_append_basic
    inline PFN_DBusMessageIterOpenContainer MessageIterOpenContainer{}; //dbus_message_iter_open_container
    inline PFN_DBusMessageIterCloseContainer MessageIterCloseContainer{}; //dbus_message_iter_close_container
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::DBus::Message::IsValid() const noexcept
{
    return DBusMsg != nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::DBus::MessageIterator::MessageIterator(DBusMessageIter msgIter)
    : DBusMsgIter(std::move(msgIter))
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::DBus::MessageIteratorContainer::MessageIteratorContainer(DBusMessageIter& mainIter, DBusMessageIter subIter)
    : DBusMsgMainIter(mainIter), DBusMsgSubIter(std::move(subIter))
{
}

#endif /*TRAP_PLATFORM_LINUX*/

#endif /*TRAP_DBUS_H*/