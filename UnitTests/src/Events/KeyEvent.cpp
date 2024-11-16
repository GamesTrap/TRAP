#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Events/KeyEvent.h"

TEST_CASE("TRAP::Events::KeyPressEvent", "[events][keypressevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::KeyPressEvent kpevent = TRAP::Events::KeyPressEvent{TRAP::Input::Key::W, WINDOW};

    SECTION("Constructor")
    {
        // const TRAP::Events::KeyPressEvent kpeventCopyConstructor(kpevent); //Deleted

        // TRAP::Events::KeyPressEvent kpevent1 = TRAP::Events::KeyPressEvent{TRAP::Input::Key::W, WINDOW};
        // const TRAP::Events::KeyPressEvent kpevent1MoveConstructor = TRAP::Events::KeyPressEvent(std::move(kpevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::KeyPressEvent kpeventAssignment = TRAP::Events::KeyPressEvent{TRAP::Input::Key::W, WINDOW};

        // const TRAP::Events::KeyPressEvent kpeventCopyAssignment = kpevent; //Deleted
        // const TRAP::Events::KeyPressEvent kpeventMoveAssignment = std::move(kpeventAssignment);
    }

    SECTION("GetKey()")
    {
        // const TRAP::Events::KeyPressEvent kpevent1 = TRAP::Events::KeyPressEvent{TRAP::Input::Key::W, WINDOW};

        // REQUIRE(kpevent1.GetKey() == TRAP::Input::Key::W);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::KeyPressEvent kpevent1 = TRAP::Events::KeyPressEvent{TRAP::Input::Key::W, WINDOW};

        // REQUIRE(kpevent1.GetWindow() == WINDOW);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::KeyPressEvent::GetStaticType() == TRAP::Events::EventType::KeyPress);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(kpevent.GetEventType() == TRAP::Events::EventType::KeyPress);
    }

    SECTION("GetName()")
    {
        // REQUIRE(kpevent.GetName() == "KeyPress");
    }

    SECTION("ToString()")
    {
        // REQUIRE(kpevent.ToString() == fmt::format("KeyPressEvent: {}({}) (\"{}\")", TRAP::Input::GetKeyName(kpevent.GetKey()), std::to_underlying(kpevent.GetKey()), kpevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(kpevent.GetCategoryFlags() == (TRAP::Events::EventCategory::Keyboard | TRAP::Events::EventCategory::Input));
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(kpevent.IsInCategory(TRAP::Events::EventCategory::Keyboard));
        // REQUIRE(kpevent.IsInCategory(TRAP::Events::EventCategory::Input));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", kpevent) == fmt::format("KeyPressEvent: {}({}) (\"{}\")", TRAP::Input::GetKeyName(kpevent.GetKey()), std::to_underlying(kpevent.GetKey()), kpevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::KeyRepeatEvent", "[events][keyrepeatevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::KeyRepeatEvent krpevent = TRAP::Events::KeyRepeatEvent{TRAP::Input::Key::W, WINDOW};

    SECTION("Constructor")
    {
        // const TRAP::Events::KeyRepeatEvent krpeventCopyConstructor(krpevent); //Deleted

        // TRAP::Events::KeyRepeatEvent krpevent1 = TRAP::Events::KeyRepeatEvent{TRAP::Input::Key::W, WINDOW};
        // const TRAP::Events::KeyRepeatEvent krpevent1MoveConstructor = TRAP::Events::KeyRepeatEvent(std::move(krpevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::KeyRepeatEvent krpeventAssignment = TRAP::Events::KeyRepeatEvent{TRAP::Input::Key::W, WINDOW};

        // const TRAP::Events::KeyRepeatEvent krpeventCopyAssignment = krpevent; //Deleted
        // const TRAP::Events::KeyRepeatEvent krpeventMoveAssignment = std::move(krpeventAssignment);
    }

    SECTION("GetKey()")
    {
        // const TRAP::Events::KeyRepeatEvent krpevent1 = TRAP::Events::KeyRepeatEvent{TRAP::Input::Key::W, WINDOW};

        // REQUIRE(krpevent1.GetKey() == TRAP::Input::Key::W);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::KeyRepeatEvent krpevent1 = TRAP::Events::KeyRepeatEvent{TRAP::Input::Key::W, WINDOW};

        // REQUIRE(krpevent1.GetWindow() == WINDOW);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::KeyRepeatEvent::GetStaticType() == TRAP::Events::EventType::KeyRepeat);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(krpevent.GetEventType() == TRAP::Events::EventType::KeyRepeat);
    }

    SECTION("GetName()")
    {
        // REQUIRE(krpevent.GetName() == "KeyRepeat");
    }

    SECTION("ToString()")
    {
        // REQUIRE(krpevent.ToString() == fmt::format("KeyRepeatEvent: {}({}) (\"{}\")", TRAP::Input::GetKeyName(krpevent.GetKey()), std::to_underlying(krpevent.GetKey()), krpevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(krpevent.GetCategoryFlags() == (TRAP::Events::EventCategory::Keyboard | TRAP::Events::EventCategory::Input));
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(krpevent.IsInCategory(TRAP::Events::EventCategory::Keyboard));
        // REQUIRE(krpevent.IsInCategory(TRAP::Events::EventCategory::Input));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", krpevent) == fmt::format("KeyRepeatEvent: {}({}) (\"{}\")", TRAP::Input::GetKeyName(krpevent.GetKey()), std::to_underlying(krpevent.GetKey()), krpevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::KeyReleaseEvent", "[events][keyreleaseevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::KeyReleaseEvent krevent = TRAP::Events::KeyReleaseEvent{TRAP::Input::Key::W, WINDOW};

    SECTION("Constructor")
    {
        // const TRAP::Events::KeyReleaseEvent kreventCopyConstructor(krevent); //Deleted

        // TRAP::Events::KeyReleaseEvent krevent1 = TRAP::Events::KeyReleaseEvent{TRAP::Input::Key::W, WINDOW};
        // const TRAP::Events::KeyReleaseEvent krevent1MoveConstructor = TRAP::Events::KeyReleaseEvent(std::move(krevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::KeyReleaseEvent kreventAssignment = TRAP::Events::KeyReleaseEvent{TRAP::Input::Key::W, WINDOW};

        // const TRAP::Events::KeyReleaseEvent kreventCopyAssignment = krevent; //Deleted
        // const TRAP::Events::KeyReleaseEvent kreventMoveAssignment = std::move(kreventAssignment);
    }

    SECTION("GetKey()")
    {
        // const TRAP::Events::KeyReleaseEvent krevent1 = TRAP::Events::KeyReleaseEvent{TRAP::Input::Key::W, WINDOW};

        // REQUIRE(krevent1.GetKey() == TRAP::Input::Key::W);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::KeyReleaseEvent krevent1 = TRAP::Events::KeyReleaseEvent{TRAP::Input::Key::W, WINDOW};

        // REQUIRE(krevent1.GetWindow() == WINDOW);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::KeyReleaseEvent::GetStaticType() == TRAP::Events::EventType::KeyRelease);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(krevent.GetEventType() == TRAP::Events::EventType::KeyRelease);
    }

    SECTION("GetName()")
    {
        // REQUIRE(krevent.GetName() == "KeyRelease");
    }

    SECTION("ToString()")
    {
        // REQUIRE(krevent.ToString() == fmt::format("KeyReleaseEvent: {}({}) (\"{}\")", TRAP::Input::GetKeyName(krevent.GetKey()), std::to_underlying(krevent.GetKey()), krevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(krevent.GetCategoryFlags() == (TRAP::Events::EventCategory::Keyboard | TRAP::Events::EventCategory::Input));
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(krevent.IsInCategory(TRAP::Events::EventCategory::Keyboard));
        // REQUIRE(krevent.IsInCategory(TRAP::Events::EventCategory::Input));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", krevent) == fmt::format("KeyReleaseEvent: {}({}) (\"{}\")", TRAP::Input::GetKeyName(krevent.GetKey()), std::to_underlying(krevent.GetKey()), krevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::KeyTypeEvent", "[events][keytypeevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::KeyTypeEvent ktevent = TRAP::Events::KeyTypeEvent{0x0001F600u, WINDOW};

    SECTION("Constructor")
    {
        // const TRAP::Events::KeyTypeEvent kteventCopyConstructor(ktevent); //Deleted

        // TRAP::Events::KeyTypeEvent ktevent1 = TRAP::Events::KeyTypeEvent{0x0001F600u, WINDOW};
        // const TRAP::Events::KeyTypeEvent ktevent1MoveConstructor = TRAP::Events::KeyTypeEvent(std::move(ktevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::KeyTypeEvent kteventAssignment = TRAP::Events::KeyTypeEvent{0x0001F600u, WINDOW};

        // const TRAP::Events::KeyTypeEvent kteventCopyAssignment = ktevent; //Deleted
        // const TRAP::Events::KeyTypeEvent kteventMoveAssignment = std::move(kteventAssignment);
    }

    SECTION("GetCodePoint()")
    {
        // const TRAP::Events::KeyTypeEvent ktevent1 = TRAP::Events::KeyTypeEvent{0x0001F600u, WINDOW};

        // REQUIRE(ktevent1.GetCodePoint() == 0x0001F600u);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::KeyTypeEvent ktevent1 = TRAP::Events::KeyTypeEvent{0x0001F600u, WINDOW};

        // REQUIRE(ktevent1.GetWindow() == WINDOW);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::KeyTypeEvent::GetStaticType() == TRAP::Events::EventType::KeyType);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(ktevent.GetEventType() == TRAP::Events::EventType::KeyType);
    }

    SECTION("GetName()")
    {
        // REQUIRE(ktevent.GetName() == "KeyType");
    }

    SECTION("ToString()")
    {
        // REQUIRE(ktevent.ToString() == fmt::format("KeyTypeEvent: {}({}) (\"{}\")", TRAP::Utils::String::EncodeUTF8(ktevent.GetCodePoint()), ktevent.GetCodePoint(), ktevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(ktevent.GetCategoryFlags() == (TRAP::Events::EventCategory::Keyboard | TRAP::Events::EventCategory::Input));
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(ktevent.IsInCategory(TRAP::Events::EventCategory::Keyboard));
        // REQUIRE(ktevent.IsInCategory(TRAP::Events::EventCategory::Input));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", ktevent) == fmt::format("KeyTypeEvent: {}({}) (\"{}\")", TRAP::Utils::String::EncodeUTF8(ktevent.GetCodePoint()), ktevent.GetCodePoint(), ktevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::KeyLayoutEvent", "[events][keylayoutevent]")
{
    const TRAP::Events::KeyLayoutEvent klevent = TRAP::Events::KeyLayoutEvent{"English (US)"};

    SECTION("Constructor")
    {
        // const TRAP::Events::KeyLayoutEvent kleventCopyConstructor(klevent); //Deleted

        TRAP::Events::KeyLayoutEvent klevent1 = TRAP::Events::KeyLayoutEvent{"English (US)"};
        const TRAP::Events::KeyLayoutEvent klevent1MoveConstructor = TRAP::Events::KeyLayoutEvent(std::move(klevent1));
    }

    SECTION("Assignment")
    {
        TRAP::Events::KeyLayoutEvent kleventAssignment = TRAP::Events::KeyLayoutEvent{"English (US)"};

        // const TRAP::Events::KeyLayoutEvent kleventCopyAssignment = klevent; //Deleted
        const TRAP::Events::KeyLayoutEvent kleventMoveAssignment = std::move(kleventAssignment);
    }

    SECTION("GetCodePoint()")
    {
        const TRAP::Events::KeyLayoutEvent klevent1 = TRAP::Events::KeyLayoutEvent{"English (US)"};

        REQUIRE(klevent1.GetLayout() == "English (US)");
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::KeyLayoutEvent::GetStaticType() == TRAP::Events::EventType::KeyLayout);
    }

    SECTION("GetEventType()")
    {
        REQUIRE(klevent.GetEventType() == TRAP::Events::EventType::KeyLayout);
    }

    SECTION("GetName()")
    {
        REQUIRE(klevent.GetName() == "KeyLayout");
    }

    SECTION("ToString()")
    {
        REQUIRE(klevent.ToString() == fmt::format("KeyLayoutEvent: {}", klevent.GetLayout()));
    }

    SECTION("GetCategoryFlags()")
    {
        REQUIRE(klevent.GetCategoryFlags() == (TRAP::Events::EventCategory::Keyboard | TRAP::Events::EventCategory::Input));
    }

    SECTION("IsInCategory()")
    {
        REQUIRE(klevent.IsInCategory(TRAP::Events::EventCategory::Keyboard));
        REQUIRE(klevent.IsInCategory(TRAP::Events::EventCategory::Input));
    }

    SECTION("fmt specialization")
    {
        REQUIRE(fmt::format("{}", klevent) == fmt::format("KeyLayoutEvent: {}", klevent.GetLayout()));
    }
}
