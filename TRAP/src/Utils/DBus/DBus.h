#ifndef TRAP_DBUS_H
#define TRAP_DBUS_H

#include "Core/Base.h"

#ifdef TRAP_PLATFORM_LINUX

#include <string>

#include "Utils/Optional.h"

/// @brief DBus communication API (wrapper based around libdbus).
/// @note All DBus related objects must be destroyed before TRAP::Application's
///       destructor is called as it unloads the function symbols of libdbus.
namespace TRAP::DBus
{
    namespace INTERNAL
    {
        using dbus_bool_t = u32;
        using dbus_uint32_t = u32;

        /// @brief Opaque data type representing a message received from or to be sent to another application.
        struct DBusMessage;

        /// @brief Opaque type representing a message iterator
        struct DBusMessageIter final
        {
            constexpr DBusMessageIter() = default;
            constexpr ~DBusMessageIter() = default;

            consteval DBusMessageIter(const DBusMessageIter&) = delete;
            constexpr DBusMessageIter(DBusMessageIter&&) = default;
            consteval DBusMessageIter& operator=(const DBusMessageIter&) = delete;
            constexpr DBusMessageIter& operator=(DBusMessageIter&&) = default;

            [[nodiscard]] constexpr bool operator==(const DBusMessageIter& dbusMsgIter) const = default;

        private:
            void* Dummy1 = nullptr;
            void* Dummy2 = nullptr;
            dbus_uint32_t Dummy3 = 0u;
            i32 Dummy4 = 0, Dummy5 = 0, Dummy6 = 0, Dummy7 = 0, Dummy8 = 0, Dummy9 = 0, Dummy10 = 0, Dummy11 = 0;
            i32 Pad1 = 0;
            void* Pad2 = nullptr;
            void* Pad3 = nullptr;
        };

        /// @brief Unload the libdbus symbols.
        /// @threadsafe
        void UnloadSymbols();
    }

    //-------------------------------------------------------------------------------------------------------------------//

    class MessageIterator;
    class MessageIteratorContainer;

    /// @brief DBus types.
    enum class Type : i32 //Must be i32, used in libdbus.so
    {
        String = 's', //Type code marking a UTF-8 encoded, null-terminated Unicode string.
        Array = 'a', //Type code marking a DBus array type.
        DictEntry = 'e', //Type code used to represent a dict entry.
        Variant = 'v', //Type code marking a DBus variant type.
        Boolean = 'b', //Type code marking a boolean.
        Double = 'd', //Type code marking an 8-byte double in IEEE 754 format.
        Int16 = 'n', //Type code marking a 16-bit signed integer.
        UInt16 = 'q', //Type code marking a 16-bit unsigned integer.
        Int32 = 'i', //Type code marking a 32-bit signed integer.
        UInt32 = 'u', //Type code marking a 32-bit unsigned integer.
        Int64 = 'x', //Type code marking a 64-bit signed integer.
        UInt64 = 't', //Type code marking a 64-bit unsigned integer.
        Struct = 'r', //Type code marking a struct.
        Byte = 'y', //Type code marking an 8-bit unsigned integer.
        ObjectPath = 'o', //Type code marking a DBus object path.
        Signature = 'g' //Type code marking a DBus type signature.
    };

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief DBus message.
    class Message final
    {
    public:
        /// @brief Constructs a new message representing a signal emission.
        ///        A signal is identified by its originating object path, interface, and the name of the signal.
        ///        objectPath and interfaceName must all be valid (the DBus specification defines the syntax of these fields).
        /// @param objectPath Path of the object emitting the signal.
        /// @param interfaceName The name of the interface the signal is emitted from.
        /// @threadsafe
        Message(const std::string& objectPath, const std::string& interfaceName);

        /// @brief Destructor. Frees the DBus message.
        /// @threadsafe
        ~Message();

        consteval Message(const Message&) = delete;
        constexpr Message(Message&&) = delete;

        consteval Message& operator=(const Message&) = delete;
        Message& operator=(Message&&) = delete;

        /// @brief Retrieve a MessageIterator for appending arguments to the end of the message.
        /// @return MessageIterator on success, TRAP::NullOpt otherwise.
        /// @note The Message must outlive the MessageIterator!
        /// @threadsafe
        [[nodiscard]] TRAP::Optional<MessageIterator> GetMessageIterator() const;

        /// @brief Query whether the DBus message is in a valid state or not.
        /// @return True if DBus message is valid, false otherwise.
        /// @threadsafe
        [[nodiscard]] constexpr bool IsValid() const noexcept;

        /// @brief Adds the DBus message to the outgoing message queue.
        ///        Optionally blocks until the message was send if flush is set to true.
        /// @param flush Whether to block until the message was send or not.
        /// @return True if message was sent successfully, false otherwise.
        /// @threadsafe
        bool SendMessage(bool flush = false) const;

    private:
        INTERNAL::DBusMessage* m_DBusMsg = nullptr;
    };

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief DBus message iterator.
    class MessageIterator final
    {
    public:
        /// @brief Construct a MessageIterator from a libdbus DBusMessageIter.
        /// @threadsafe
        constexpr explicit MessageIterator(INTERNAL::DBusMessageIter msgIter);
        constexpr ~MessageIterator() = default;

        consteval MessageIterator(const MessageIterator&) = delete;
        constexpr MessageIterator(MessageIterator&&) = delete;
        consteval MessageIterator& operator=(const MessageIterator&) = delete;
        constexpr MessageIterator& operator=(MessageIterator&&) = delete;

        /// @brief Append a basic-typed value to the message.
        /// @param type Type of the value to append.
        /// @param data The address of the value to append.
        /// @return True on success, false otherwise.
        /// @threadsafe
        [[nodiscard]] bool AppendData(Type type, const void* data);
        /// @brief Append a key-value entry to the message.
        /// @param keyType Type of the key to append.
        /// @param keyData The address of the key to append.
        /// @param valueType Type of the value to append.
        /// @param valueData The address of the value to append.
        /// @return True on success, false otherwise.
        /// @threadsafe
        [[nodiscard]] bool AppendDictData(Type keyType, const void* keyData, Type valueType, const void* valueData);

        /// @brief Open a container-typed message iterator to append values to.
        /// @param type Type of the container to open.
        /// @param signature Type of the container contents.
        /// @return MessageIteratorContainer on success, TRAP::NullOpt otherwise.
        /// @note The MessageIterator must outlive the newly opened MessageIteratorContainer object!
        /// @threadsafe
        [[nodiscard]] TRAP::Optional<MessageIteratorContainer> OpenContainer(Type type, const std::string& signature);

    private:
        INTERNAL::DBusMessageIter m_DBusMsgIter{};
    };

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief DBus message iterator container.
    class MessageIteratorContainer final
    {
    public:
        /// @brief Construct a MessageIteratorContainer.
        /// @threadsafe
        constexpr explicit MessageIteratorContainer(INTERNAL::DBusMessageIter& mainIter, INTERNAL::DBusMessageIter subIter);

        /// @brief Destructor. Closes the container message iterator.
        /// @threadsafe
        ~MessageIteratorContainer();

        consteval MessageIteratorContainer(const MessageIteratorContainer&) = delete;
        constexpr MessageIteratorContainer(MessageIteratorContainer&&) = delete;
        consteval MessageIteratorContainer& operator=(const MessageIteratorContainer&) = delete;
        MessageIteratorContainer& operator=(MessageIteratorContainer&&) = delete;

        /// @brief Appends a basic-typed value to the iterator.
        /// @param type Type of the value to append.
        /// @param data The address of the value to append.
        /// @return True on success, false otherwise.
        /// @threadsafe
        bool AppendData(Type type, const void* data);
        /// @brief Append a key-value entry to the iterator.
        /// @param keyType Type of the key to append.
        /// @param keyData The address of the key to append.
        /// @param valueType Type of the value to append.
        /// @param valueData The address of the value to append.
        /// @return True on success, false otherwise.
        /// @threadsafe
        bool AppendDictData(Type keyType, const void* keyData, Type valueType, const void* valueData);

        /// @brief Open a container-typed message iterator to append values to.
        /// @param type Type of the container to open.
        /// @param signature Type of the container contents.
        /// @return MessageIteratorContainer on success, TRAP::NullOpt otherwise.
        /// @note The MessageIteratorContainer must outlive the newly opened MessageIteratorContainer object!
        /// @threadsafe
        [[nodiscard]] TRAP::Optional<MessageIteratorContainer> OpenContainer(Type type, const std::string& signature);

    private:
        std::reference_wrapper<INTERNAL::DBusMessageIter> m_DBusMsgMainIter;
        INTERNAL::DBusMessageIter m_DBusMsgSubIter{};
    };

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Retrieve the URI of the running application.
    ///        Example: "application://Sandbox.desktop".
    /// @return URI of the running application.
    /// @threadsafe
    [[nodiscard]] std::string GetApplicationURI();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::DBus::Message::IsValid() const noexcept
{
    return m_DBusMsg != nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::DBus::MessageIterator::MessageIterator(INTERNAL::DBusMessageIter msgIter)
    : m_DBusMsgIter(std::move(msgIter))
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::DBus::MessageIteratorContainer::MessageIteratorContainer(INTERNAL::DBusMessageIter& mainIter,
                                                                         INTERNAL::DBusMessageIter subIter)
    : m_DBusMsgMainIter(mainIter), m_DBusMsgSubIter(std::move(subIter))
{
}

#endif /*TRAP_PLATFORM_LINUX*/

#endif /*TRAP_DBUS_H*/
