#include <catch2/catch_test_macros.hpp>

#include "Core/PlatformDetection.h"

#ifdef TRAP_PLATFORM_LINUX

#include "Utils/DBus/DBus.h"
#include "Utils/DBus/DBus.cpp"

namespace
{
    [[nodiscard]] bool IsLibDBusAvailable()
    {
        void* const libdbus = TRAP::Utils::DynamicLoading::LoadLibrary("libdbus-1.so.3");
        if(libdbus != nullptr)
            TRAP::Utils::DynamicLoading::FreeLibrary(libdbus);

        return libdbus != nullptr;
    }

    bool LibDBusAvailable = IsLibDBusAvailable();
}

TEST_CASE("TRAP::DBus::Message", "[utils][dbus][message]")
{
    if(!LibDBusAvailable)
        SKIP("LibDBus is not available");

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::DBus::Message>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::DBus::Message>);
        STATIC_REQUIRE_FALSE(std::movable<TRAP::DBus::Message>);
    }

    SECTION("Constructors")
    {
        const TRAP::DBus::Message invalidMsg{"", "TestSignal"};
        REQUIRE_FALSE(invalidMsg.IsValid());
        const TRAP::DBus::Message invalidMsg1{"org.freedesktop.DBus.Testing", ""};
        REQUIRE_FALSE(invalidMsg1.IsValid());
        const TRAP::DBus::Message invalidMsg2{"", ""};
        REQUIRE_FALSE(invalidMsg2.IsValid());

        const TRAP::DBus::Message message{"org.freedesktop.DBus.Testing", "TestSignal"};

        // const TRAP::DBus::Message messageCopy{message}; //Deleted

        // TRAP::DBus::Message messageMoveSource{"org.freedesktop.DBus.Testing", "TestSignal"};
        // const TRAP::DBus::Message messageMoveTarget{std::move(messageMoveSource)}; //Deleted
    }

    SECTION("Assignments")
    {
        const TRAP::DBus::Message message{"org.freedesktop.DBus.Testing", "TestSignal"};

        // const TRAP::DBus::Message messageCopy = message; //Deleted

        // TRAP::DBus::Message messageMoveSource{"org.freedesktop.DBus.Testing", "TestSignal"};
        // const TRAP::DBus::Message messageMoveTarget = std::move(messageMoveSource); //Deleted
    }

    SECTION("GetMessageIterator()")
    {
        const TRAP::DBus::Message message{"org.freedesktop.DBus.Testing", "TestSignal"};
        REQUIRE(message.GetMessageIterator() != TRAP::NullOpt);
    }

    SECTION("IsValid()")
    {
        const TRAP::DBus::Message message{"org.freedesktop.DBus.Testing", "TestSignal"};
        REQUIRE(message.IsValid());
    }

    SECTION("SendMessage()")
    {
        const TRAP::DBus::Message message{"org.freedesktop.DBus.Testing", "TestSignal"};
        REQUIRE(message.SendMessage());
        REQUIRE(message.SendMessage(true));
    }
}

TEST_CASE("TRAP::DBus::MessageIterator", "[utils][dbus][messageiter]")
{
    if(!LibDBusAvailable)
        SKIP("LibDBus is not available");

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::DBus::MessageIterator>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::DBus::MessageIterator>);
        STATIC_REQUIRE_FALSE(std::movable<TRAP::DBus::MessageIterator>);
    }

    SECTION("Constructors")
    {
        const TRAP::DBus::Message message{"org.freedesktop.DBus.Testing", "TestSignal"};
        auto msgIt = message.GetMessageIterator();
        REQUIRE(msgIt != TRAP::NullOpt);

        // static constexpr TRAP::DBus::MessageIterator messageIteratorCopy{*msgIt}; //Deleted

        // const TRAP::DBus::MessageIterator messageIteratorMoveTarget{std::move(*msgIt)}; //Deleted
    }

    SECTION("Assignments")
    {
        const TRAP::DBus::Message message{"org.freedesktop.DBus.Testing", "TestSignal"};
        auto msgIt = message.GetMessageIterator();
        REQUIRE(msgIt != TRAP::NullOpt);

        // static constexpr TRAP::DBus::MessageIterator messageIteratorCopy = *msgIt; //Deleted

        // const TRAP::DBus::MessageIterator messageIteratorMoveTarget = std::move(*msgIt); //Deleted
    }

    SECTION("AppendData()")
    {
        const TRAP::DBus::Message message{"org.freedesktop.DBus.Testing", "TestSignal"};
        auto msgIt = message.GetMessageIterator();
        REQUIRE(msgIt != TRAP::NullOpt);

        //Valid - basic types
        static constexpr std::string_view testStr = "Hello World!";
        static constexpr const char* const testCStr = testStr.data();
        REQUIRE(msgIt->AppendData(TRAP::DBus::Type::String, static_cast<const void*>(&testCStr)));

        static constexpr u32 testBool = static_cast<u32>(true);
        REQUIRE(msgIt->AppendData(TRAP::DBus::Type::Boolean, static_cast<const void*>(&testBool)));

        static constexpr double testDouble = 130.0;
        REQUIRE(msgIt->AppendData(TRAP::DBus::Type::Double, static_cast<const void*>(&testDouble)));

        static constexpr i16 testi16 = 2000;
        REQUIRE(msgIt->AppendData(TRAP::DBus::Type::Int16, static_cast<const void*>(&testi16)));

        static constexpr u16 testu16 = -2000;
        REQUIRE(msgIt->AppendData(TRAP::DBus::Type::UInt16, static_cast<const void*>(&testu16)));

        static constexpr i32 testi32 = 70000;
        REQUIRE(msgIt->AppendData(TRAP::DBus::Type::Int32, static_cast<const void*>(&testi32)));

        static constexpr u32 testu32 = -70000;
        REQUIRE(msgIt->AppendData(TRAP::DBus::Type::UInt32, static_cast<const void*>(&testu32)));

        static constexpr i64 testi64 = 50'000'000'000;
        REQUIRE(msgIt->AppendData(TRAP::DBus::Type::Int64, static_cast<const void*>(&testi64)));

        static constexpr u64 testu64 = -50'000'000'000;
        REQUIRE(msgIt->AppendData(TRAP::DBus::Type::UInt64, static_cast<const void*>(&testu64)));

        static constexpr u8 testu8 = 128;
        REQUIRE(msgIt->AppendData(TRAP::DBus::Type::Byte, static_cast<const void*>(&testu8)));

        static constexpr std::string_view testObjectPath = "/com/trappedgames/example/object";
        static constexpr const char* const testObjectPathCStr = testObjectPath.data();
        REQUIRE(msgIt->AppendData(TRAP::DBus::Type::ObjectPath, static_cast<const void*>(&testObjectPathCStr)));

        static constexpr std::string_view testSignature = "s";
        static constexpr const char* const testSignatureCStr = testSignature.data();
        REQUIRE(msgIt->AppendData(TRAP::DBus::Type::Signature, static_cast<const void*>(&testSignatureCStr)));

        //Invalid - container types
        const std::array<std::string, 2u> testArray{"Hello", "World"};
        REQUIRE_FALSE(msgIt->AppendData(TRAP::DBus::Type::Array, static_cast<const void*>(&testArray)));

        REQUIRE_FALSE(msgIt->AppendData(TRAP::DBus::Type::Variant, static_cast<const void*>(&testi16)));

        REQUIRE_FALSE(msgIt->AppendData(TRAP::DBus::Type::DictEntry, static_cast<const void*>(&testi16)));

        //Invalid - nullptr data
        REQUIRE_FALSE(msgIt->AppendData(TRAP::DBus::Type::Int16, nullptr));
    }

    SECTION("AppendDictData()")
    {
        const TRAP::DBus::Message message{"org.freedesktop.DBus.Testing", "TestSignal"};
        auto msgIt = message.GetMessageIterator();
        REQUIRE(msgIt != TRAP::NullOpt);

        //Valid - basic type
        static constexpr std::string_view testStr = "Hello World!";
        static constexpr const char* const testCStr = testStr.data();
        REQUIRE(msgIt->AppendDictData(TRAP::DBus::Type::String, static_cast<const void*>(&testCStr), TRAP::DBus::Type::String, static_cast<const void*>(&testCStr)));

        static constexpr u32 testBool = static_cast<u32>(true);
        REQUIRE(msgIt->AppendDictData(TRAP::DBus::Type::Boolean, static_cast<const void*>(&testBool), TRAP::DBus::Type::Boolean, static_cast<const void*>(&testBool)));

        static constexpr double testDouble = 130.0;
        REQUIRE(msgIt->AppendDictData(TRAP::DBus::Type::Double, static_cast<const void*>(&testDouble), TRAP::DBus::Type::Double, static_cast<const void*>(&testDouble)));

        static constexpr i16 testi16 = 2000;
        REQUIRE(msgIt->AppendDictData(TRAP::DBus::Type::Int16, static_cast<const void*>(&testi16), TRAP::DBus::Type::Int16, static_cast<const void*>(&testi16)));

        static constexpr u16 testu16 = -2000;
        REQUIRE(msgIt->AppendDictData(TRAP::DBus::Type::UInt16, static_cast<const void*>(&testu16), TRAP::DBus::Type::UInt16, static_cast<const void*>(&testu16)));

        static constexpr i32 testi32 = 70000;
        REQUIRE(msgIt->AppendDictData(TRAP::DBus::Type::Int32, static_cast<const void*>(&testi32), TRAP::DBus::Type::Int32, static_cast<const void*>(&testi32)));

        static constexpr u32 testu32 = -70000;
        REQUIRE(msgIt->AppendDictData(TRAP::DBus::Type::UInt32, static_cast<const void*>(&testu32), TRAP::DBus::Type::UInt32, static_cast<const void*>(&testu32)));

        static constexpr i64 testi64 = 50'000'000'000;
        REQUIRE(msgIt->AppendDictData(TRAP::DBus::Type::Int64, static_cast<const void*>(&testi64), TRAP::DBus::Type::Int64, static_cast<const void*>(&testi64)));

        static constexpr u64 testu64 = -50'000'000'000;
        REQUIRE(msgIt->AppendDictData(TRAP::DBus::Type::UInt64, static_cast<const void*>(&testu64), TRAP::DBus::Type::UInt64, static_cast<const void*>(&testu64)));

        static constexpr u8 testu8 = 128;
        REQUIRE(msgIt->AppendDictData(TRAP::DBus::Type::Byte, static_cast<const void*>(&testu8), TRAP::DBus::Type::Byte, static_cast<const void*>(&testu8)));

        static constexpr std::string_view testObjectPath = "/com/trappedgames/example/object";
        static constexpr const char* const testObjectPathCStr = testObjectPath.data();
        REQUIRE(msgIt->AppendDictData(TRAP::DBus::Type::ObjectPath, static_cast<const void*>(&testObjectPathCStr), TRAP::DBus::Type::ObjectPath, static_cast<const void*>(&testObjectPathCStr)));

        static constexpr std::string_view testSignature = "s";
        static constexpr const char* const testSignatureCStr = testSignature.data();
        REQUIRE(msgIt->AppendDictData(TRAP::DBus::Type::Signature, static_cast<const void*>(&testSignatureCStr), TRAP::DBus::Type::Signature, static_cast<const void*>(&testSignatureCStr)));

        //Invalid - container type
        const std::array<std::string, 2u> testArray{"Hello", "World"};
        REQUIRE_FALSE(msgIt->AppendDictData(TRAP::DBus::Type::Array, static_cast<const void*>(&testArray), TRAP::DBus::Type::Array, static_cast<const void*>(&testArray)));

        REQUIRE_FALSE(msgIt->AppendDictData(TRAP::DBus::Type::Variant, static_cast<const void*>(&testi16), TRAP::DBus::Type::Variant, static_cast<const void*>(&testi16)));

        REQUIRE_FALSE(msgIt->AppendDictData(TRAP::DBus::Type::DictEntry, static_cast<const void*>(&testi16), TRAP::DBus::Type::DictEntry, static_cast<const void*>(&testi16)));

        //Invalid - nullptr data
        REQUIRE_FALSE(msgIt->AppendDictData(TRAP::DBus::Type::UInt16, nullptr, TRAP::DBus::Type::UInt16, &testu16));
        REQUIRE_FALSE(msgIt->AppendDictData(TRAP::DBus::Type::UInt16, &testu16, TRAP::DBus::Type::UInt16, nullptr));
        REQUIRE_FALSE(msgIt->AppendDictData(TRAP::DBus::Type::UInt16, nullptr, TRAP::DBus::Type::UInt16, nullptr));
    }

    SECTION("OpenContainer()")
    {
        const TRAP::DBus::Message message{"org.freedesktop.DBus.Testing", "TestSignal"};
        auto msgIt = message.GetMessageIterator();
        REQUIRE(msgIt != TRAP::NullOpt);

        //Valid
        REQUIRE(msgIt->OpenContainer(TRAP::DBus::Type::Array, "{sv}") != TRAP::NullOpt);
        REQUIRE(msgIt->OpenContainer(TRAP::DBus::Type::DictEntry, "") != TRAP::NullOpt);
        REQUIRE(msgIt->OpenContainer(TRAP::DBus::Type::Variant, "s") != TRAP::NullOpt);
        REQUIRE(msgIt->OpenContainer(TRAP::DBus::Type::Struct, "") != TRAP::NullOpt);

        //Imvalid
        REQUIRE_FALSE(msgIt->OpenContainer(TRAP::DBus::Type::Variant, ""));
        REQUIRE_FALSE(msgIt->OpenContainer(TRAP::DBus::Type::String, "s"));
    }
}

TEST_CASE("TRAP::DBus::MessageIteratorContainer", "[utils][dbus][messageiteratorcontainer]")
{
    if(!LibDBusAvailable)
        SKIP("LibDBus is not available");

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::DBus::MessageIteratorContainer>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::DBus::MessageIteratorContainer>);
        STATIC_REQUIRE_FALSE(std::movable<TRAP::DBus::MessageIteratorContainer>);
    }

    SECTION("Constructors")
    {
        const TRAP::DBus::Message message{"org.freedesktop.DBus.Testing", "TestSignal"};
        auto msgIt = message.GetMessageIterator();
        REQUIRE(msgIt != TRAP::NullOpt);
        auto containerIt = msgIt->OpenContainer(TRAP::DBus::Type::Array, "{}");
        REQUIRE(containerIt);

        // const TRAP::DBus::MessageIteratorContainer messageIteratorContainerCopy{*containerIt}; //Deleted

        // const TRAP::DBus::MessageIteratorContainer messageIteratorContainerMoveTarget{std::move(*containerIt)}; //Deleted
    }

    SECTION("Assignments")
    {
        const TRAP::DBus::Message message{"org.freedesktop.DBus.Testing", "TestSignal"};
        auto msgIt = message.GetMessageIterator();
        REQUIRE(msgIt != TRAP::NullOpt);
        auto containerIt = msgIt->OpenContainer(TRAP::DBus::Type::Array, "{}");
        REQUIRE(containerIt);

        // const TRAP::DBus::MessageIteratorContainer messageIteratorContainerCopy = *containerIt; //Deleted

        // const TRAP::DBus::MessageIteratorContainer messageIteratorContainerMoveTarget = std::move(*containerIt);
    }

    SECTION("AppendData()")
    {
        const TRAP::DBus::Message message{"org.freedesktop.DBus.Testing", "TestSignal"};
        auto msgIt = message.GetMessageIterator();
        REQUIRE(msgIt != TRAP::NullOpt);
        auto containerIt = msgIt->OpenContainer(TRAP::DBus::Type::Struct, "");
        REQUIRE(containerIt != TRAP::NullOpt);

        auto subContainerIt = containerIt->OpenContainer(TRAP::DBus::Type::Struct, "");
        REQUIRE(subContainerIt != TRAP::NullOpt);

        //Valid - basic types
        static constexpr std::string_view testStr = "Hello World!";
        static constexpr const char* const testCStr = testStr.data();
        REQUIRE(subContainerIt->AppendData(TRAP::DBus::Type::String, static_cast<const void*>(&testCStr)));

        static constexpr u32 testBool = static_cast<u32>(true);
        REQUIRE(subContainerIt->AppendData(TRAP::DBus::Type::Boolean, static_cast<const void*>(&testBool)));

        static constexpr double testDouble = 130.0;
        REQUIRE(subContainerIt->AppendData(TRAP::DBus::Type::Double, static_cast<const void*>(&testDouble)));

        static constexpr i16 testi16 = 2000;
        REQUIRE(subContainerIt->AppendData(TRAP::DBus::Type::Int16, static_cast<const void*>(&testi16)));

        static constexpr u16 testu16 = -2000;
        REQUIRE(subContainerIt->AppendData(TRAP::DBus::Type::UInt16, static_cast<const void*>(&testu16)));

        static constexpr i32 testi32 = 70000;
        REQUIRE(subContainerIt->AppendData(TRAP::DBus::Type::Int32, static_cast<const void*>(&testi32)));

        static constexpr u32 testu32 = -70000;
        REQUIRE(subContainerIt->AppendData(TRAP::DBus::Type::UInt32, static_cast<const void*>(&testu32)));

        static constexpr i64 testi64 = 50'000'000'000;
        REQUIRE(subContainerIt->AppendData(TRAP::DBus::Type::Int64, static_cast<const void*>(&testi64)));

        static constexpr u64 testu64 = -50'000'000'000;
        REQUIRE(subContainerIt->AppendData(TRAP::DBus::Type::UInt64, static_cast<const void*>(&testu64)));

        static constexpr u8 testu8 = 128;
        REQUIRE(subContainerIt->AppendData(TRAP::DBus::Type::Byte, static_cast<const void*>(&testu8)));

        static constexpr std::string_view testObjectPath = "/com/trappedgames/example/object";
        static constexpr const char* const testObjectPathCStr = testObjectPath.data();
        REQUIRE(subContainerIt->AppendData(TRAP::DBus::Type::ObjectPath, static_cast<const void*>(&testObjectPathCStr)));

        static constexpr std::string_view testSignature = "s";
        static constexpr const char* const testSignatureCStr = testSignature.data();
        REQUIRE(subContainerIt->AppendData(TRAP::DBus::Type::Signature, static_cast<const void*>(&testSignatureCStr)));

        //Invalid - container types
        const std::array<std::string, 2u> testArray{"Hello", "World"};
        REQUIRE_FALSE(subContainerIt->AppendData(TRAP::DBus::Type::Array, static_cast<const void*>(&testArray)));

        REQUIRE_FALSE(subContainerIt->AppendData(TRAP::DBus::Type::Variant, static_cast<const void*>(&testi16)));

        REQUIRE_FALSE(subContainerIt->AppendData(TRAP::DBus::Type::DictEntry, static_cast<const void*>(&testi16)));

        //Invalid - nullptr data
        REQUIRE_FALSE(subContainerIt->AppendData(TRAP::DBus::Type::Int16, nullptr));
    }

    SECTION("AppendDictData()")
    {
        const TRAP::DBus::Message message{"org.freedesktop.DBus.Testing", "TestSignal"};
        auto msgIt = message.GetMessageIterator();
        REQUIRE(msgIt != TRAP::NullOpt);
        auto containerIt = msgIt->OpenContainer(TRAP::DBus::Type::Struct, "");
        REQUIRE(containerIt != TRAP::NullOpt);

        auto subContainerIt = containerIt->OpenContainer(TRAP::DBus::Type::Struct, "");
        REQUIRE(subContainerIt != TRAP::NullOpt);

                //Valid - basic type
        static constexpr std::string_view testStr = "Hello World!";
        static constexpr const char* const testCStr = testStr.data();
        REQUIRE(subContainerIt->AppendDictData(TRAP::DBus::Type::String, static_cast<const void*>(&testCStr), TRAP::DBus::Type::String, static_cast<const void*>(&testCStr)));

        static constexpr u32 testBool = static_cast<u32>(true);
        REQUIRE(subContainerIt->AppendDictData(TRAP::DBus::Type::Boolean, static_cast<const void*>(&testBool), TRAP::DBus::Type::Boolean, static_cast<const void*>(&testBool)));

        static constexpr double testDouble = 130.0;
        REQUIRE(subContainerIt->AppendDictData(TRAP::DBus::Type::Double, static_cast<const void*>(&testDouble), TRAP::DBus::Type::Double, static_cast<const void*>(&testDouble)));

        static constexpr i16 testi16 = 2000;
        REQUIRE(subContainerIt->AppendDictData(TRAP::DBus::Type::Int16, static_cast<const void*>(&testi16), TRAP::DBus::Type::Int16, static_cast<const void*>(&testi16)));

        static constexpr u16 testu16 = -2000;
        REQUIRE(subContainerIt->AppendDictData(TRAP::DBus::Type::UInt16, static_cast<const void*>(&testu16), TRAP::DBus::Type::UInt16, static_cast<const void*>(&testu16)));

        static constexpr i32 testi32 = 70000;
        REQUIRE(subContainerIt->AppendDictData(TRAP::DBus::Type::Int32, static_cast<const void*>(&testi32), TRAP::DBus::Type::Int32, static_cast<const void*>(&testi32)));

        static constexpr u32 testu32 = -70000;
        REQUIRE(subContainerIt->AppendDictData(TRAP::DBus::Type::UInt32, static_cast<const void*>(&testu32), TRAP::DBus::Type::UInt32, static_cast<const void*>(&testu32)));

        static constexpr i64 testi64 = 50'000'000'000;
        REQUIRE(subContainerIt->AppendDictData(TRAP::DBus::Type::Int64, static_cast<const void*>(&testi64), TRAP::DBus::Type::Int64, static_cast<const void*>(&testi64)));

        static constexpr u64 testu64 = -50'000'000'000;
        REQUIRE(subContainerIt->AppendDictData(TRAP::DBus::Type::UInt64, static_cast<const void*>(&testu64), TRAP::DBus::Type::UInt64, static_cast<const void*>(&testu64)));

        static constexpr u8 testu8 = 128;
        REQUIRE(subContainerIt->AppendDictData(TRAP::DBus::Type::Byte, static_cast<const void*>(&testu8), TRAP::DBus::Type::Byte, static_cast<const void*>(&testu8)));

        static constexpr std::string_view testObjectPath = "/com/trappedgames/example/object";
        static constexpr const char* const testObjectPathCStr = testObjectPath.data();
        REQUIRE(subContainerIt->AppendDictData(TRAP::DBus::Type::ObjectPath, static_cast<const void*>(&testObjectPathCStr), TRAP::DBus::Type::ObjectPath, static_cast<const void*>(&testObjectPathCStr)));

        static constexpr std::string_view testSignature = "s";
        static constexpr const char* const testSignatureCStr = testSignature.data();
        REQUIRE(subContainerIt->AppendDictData(TRAP::DBus::Type::Signature, static_cast<const void*>(&testSignatureCStr), TRAP::DBus::Type::Signature, static_cast<const void*>(&testSignatureCStr)));

        //Invalid - container type
        const std::array<std::string, 2u> testArray{"Hello", "World"};
        REQUIRE_FALSE(subContainerIt->AppendDictData(TRAP::DBus::Type::Array, static_cast<const void*>(&testArray), TRAP::DBus::Type::Array, static_cast<const void*>(&testArray)));

        REQUIRE_FALSE(subContainerIt->AppendDictData(TRAP::DBus::Type::Variant, static_cast<const void*>(&testi16), TRAP::DBus::Type::Variant, static_cast<const void*>(&testi16)));

        REQUIRE_FALSE(subContainerIt->AppendDictData(TRAP::DBus::Type::DictEntry, static_cast<const void*>(&testi16), TRAP::DBus::Type::DictEntry, static_cast<const void*>(&testi16)));

        //Invalid - nullptr data
        REQUIRE_FALSE(subContainerIt->AppendDictData(TRAP::DBus::Type::UInt16, nullptr, TRAP::DBus::Type::UInt16, &testu16));
        REQUIRE_FALSE(subContainerIt->AppendDictData(TRAP::DBus::Type::UInt16, &testu16, TRAP::DBus::Type::UInt16, nullptr));
        REQUIRE_FALSE(subContainerIt->AppendDictData(TRAP::DBus::Type::UInt16, nullptr, TRAP::DBus::Type::UInt16, nullptr));
    }

    SECTION("OpenContainer()")
    {
        const TRAP::DBus::Message message{"org.freedesktop.DBus.Testing", "TestSignal"};
        auto msgIt = message.GetMessageIterator();
        REQUIRE(msgIt != TRAP::NullOpt);
        auto containerIt = msgIt->OpenContainer(TRAP::DBus::Type::Array, "{}");
        REQUIRE(containerIt != TRAP::NullOpt);

        //Valid
        const auto subContainerIt = containerIt->OpenContainer(TRAP::DBus::Type::DictEntry, "");
        REQUIRE(subContainerIt != TRAP::NullOpt);

        //Imvalid
        REQUIRE_FALSE(containerIt->OpenContainer(TRAP::DBus::Type::Variant, ""));
        REQUIRE_FALSE(containerIt->OpenContainer(TRAP::DBus::Type::String, "s"));
    }
}

TEST_CASE("TRAP::DBus", "[utils][dbus]")
{
    if(!LibDBusAvailable)
        SKIP("LibDBus is not available");

    SECTION("GetApplicationURI()")
    {
        //The application URI gets set while loading the symbols of libdbus.so, so this is a requirement for the test.
        REQUIRE(GetLibDBus().WriteLock()->LoadSymbols());

        REQUIRE(TRAP::DBus::GetApplicationURI() == "application://UnitTests.desktop");
    }
}

#endif /*TRAP_PLATFORM_LINUX*/
