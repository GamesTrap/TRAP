#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Utils/Win.h"
#include "TRAP/src/Events/ControllerEvent.h"

TEST_CASE("TRAP::Events::ControllerConnectEvent", "[events][controllerconnectevent]")
{
    static constexpr TRAP::Events::ControllerConnectEvent cevent = TRAP::Events::ControllerConnectEvent{TRAP::Input::Controller::One};

    SECTION("Constructor")
    {
        // static constexpr TRAP::Events::ControllerConnectEvent ceventCopyConstructor(cevent); //Deleted

        TRAP::Events::ControllerConnectEvent cevent1 = TRAP::Events::ControllerConnectEvent{TRAP::Input::Controller::One};
        const TRAP::Events::ControllerConnectEvent cevent1MoveConstructor = TRAP::Events::ControllerConnectEvent(std::move(cevent1));
    }

    SECTION("Assignment")
    {
        TRAP::Events::ControllerConnectEvent ceventAssignment = TRAP::Events::ControllerConnectEvent{TRAP::Input::Controller::One};

        // const TRAP::Events::ControllerConnectEvent ceventCopyAssignment = cevent; //Deleted
        const TRAP::Events::ControllerConnectEvent ceventMoveAssignment = std::move(ceventAssignment);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::ControllerConnectEvent::GetStaticType() == TRAP::Events::EventType::ControllerConnect);
    }

    SECTION("GetEventType()")
    {
        STATIC_REQUIRE(cevent.GetEventType() == TRAP::Events::EventType::ControllerConnect);
    }

    SECTION("GetName()")
    {
        STATIC_REQUIRE(cevent.GetName() == "ControllerConnect");
    }

    SECTION("ToString()")
    {
        REQUIRE(cevent.ToString() == fmt::format("ControllerConnectEvent: {}", (std::to_underlying(TRAP::Input::Controller::One) + 1u)));
    }

    SECTION("GetController()")
    {
        STATIC_REQUIRE(cevent.GetController() == TRAP::Input::Controller::One);
    }

    SECTION("GetCategoryFlags()")
    {
        STATIC_REQUIRE(cevent.GetCategoryFlags() == (TRAP::Events::EventCategory::Controller | TRAP::Events::EventCategory::Input));
    }

    SECTION("IsInCategory()")
    {
        STATIC_REQUIRE(cevent.IsInCategory(TRAP::Events::EventCategory::Controller));
        STATIC_REQUIRE(cevent.IsInCategory(TRAP::Events::EventCategory::Input));
    }

    SECTION("fmt specialization")
    {
        REQUIRE(fmt::format("{}", cevent) == fmt::format("ControllerConnectEvent: {}", (std::to_underlying(TRAP::Input::Controller::One) + 1u)));
    }
}

TEST_CASE("TRAP::Events::ControllerDisconnectEvent", "[events][controllerdisconnectevent]")
{
    static constexpr TRAP::Events::ControllerDisconnectEvent cevent = TRAP::Events::ControllerDisconnectEvent{TRAP::Input::Controller::One};

    SECTION("Constructor")
    {
        // static constexpr TRAP::Events::ControllerDisconnectEvent ceventCopyConstructor(cevent); //Deleted

        TRAP::Events::ControllerDisconnectEvent cevent1 = TRAP::Events::ControllerDisconnectEvent{TRAP::Input::Controller::One};
        const TRAP::Events::ControllerDisconnectEvent cevent1MoveConstructor = TRAP::Events::ControllerDisconnectEvent(std::move(cevent1));
    }

    SECTION("Assignment")
    {
        TRAP::Events::ControllerDisconnectEvent ceventAssignment = TRAP::Events::ControllerDisconnectEvent{TRAP::Input::Controller::One};

        // const TRAP::Events::ControllerDisconnectEvent ceventCopyAssignment = cevent; //Deleted
        const TRAP::Events::ControllerDisconnectEvent ceventMoveAssignment = std::move(ceventAssignment);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::ControllerDisconnectEvent::GetStaticType() == TRAP::Events::EventType::ControllerDisconnect);
    }

    SECTION("GetEventType()")
    {
        STATIC_REQUIRE(cevent.GetEventType() == TRAP::Events::EventType::ControllerDisconnect);
    }

    SECTION("GetName()")
    {
        STATIC_REQUIRE(cevent.GetName() == "ControllerDisconnect");
    }

    SECTION("ToString()")
    {
        REQUIRE(cevent.ToString() == fmt::format("ControllerDisconnectEvent: {}", (std::to_underlying(TRAP::Input::Controller::One) + 1u)));
    }

    SECTION("GetController()")
    {
        STATIC_REQUIRE(cevent.GetController() == TRAP::Input::Controller::One);
    }

    SECTION("GetCategoryFlags()")
    {
        STATIC_REQUIRE(cevent.GetCategoryFlags() == (TRAP::Events::EventCategory::Controller | TRAP::Events::EventCategory::Input));
    }

    SECTION("IsInCategory()")
    {
        STATIC_REQUIRE(cevent.IsInCategory(TRAP::Events::EventCategory::Controller));
        STATIC_REQUIRE(cevent.IsInCategory(TRAP::Events::EventCategory::Input));
    }

    SECTION("fmt specialization")
    {
        REQUIRE(fmt::format("{}", cevent) == fmt::format("ControllerDisconnectEvent: {}", (std::to_underlying(TRAP::Input::Controller::One) + 1u)));
    }
}
