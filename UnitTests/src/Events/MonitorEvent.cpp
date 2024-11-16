#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Events/MonitorEvent.h"

TEST_CASE("TRAP::Events::MonitorConnectEvent", "[events][monitorconnectevent]")
{
    //TODO Implement tests when TRAP::Monitor can be used in tests.

    // const TRAP::Events::MonitorConnectEvent mcevent = TRAP::Events::MonitorConnectEvent{MONITOR};

    SECTION("Constructor")
    {
        // const TRAP::Events::MonitorConnectEvent mceventCopyConstructor(mcpevent); //Deleted

        // TRAP::Events::MonitorConnectEvent mcevent1 = TRAP::Events::MonitorConnectEvent{MONITOR};
        // const TRAP::Events::MonitorConnectEvent mcevent1MoveConstructor = TRAP::Events::MonitorConnectEvent(std::move(mcevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::MonitorConnectEvent mceventAssignment = TRAP::Events::MonitorConnectEvent{MONITOR};

        // const TRAP::Events::MonitorConnectEvent mceventCopyAssignment = mcevent; //Deleted
        // const TRAP::Events::MonitorConnectEvent mceventMoveAssignment = std::move(mceventAssignment);
    }

    SECTION("GetMonitor()")
    {
        // const TRAP::Events::MonitorConnectEvent mcevent1 = TRAP::Events::MonitorConnectEvent{MONITOR};

        // REQUIRE(mcevent1.GetMonitor() == MONITOR);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::MonitorConnectEvent::GetStaticType() == TRAP::Events::EventType::MonitorConnect);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(mcevent.GetEventType() == TRAP::Events::EventType::MonitorConnect);
    }

    SECTION("GetName()")
    {
        // REQUIRE(mcevent.GetName() == "MonitorConnect");
    }

    SECTION("ToString()")
    {
        // REQUIRE(mcevent.ToString() == fmt::format("MonitorConnectEvent: {} ({})", mcevent.GetMonitor().GetName(), mcevent.GetMonitor().GetID()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(mcevent.GetCategoryFlags() == TRAP::Events::EventCategory::Monitor);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(mcevent.IsInCategory(TRAP::Events::EventCategory::Monitor));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", mcevent) == fmt::format("MonitorConnectEvent: {} ({})", mcevent.GetMonitor().GetName(), mcevent.GetMonitor().GetID()));
    }
}

TEST_CASE("TRAP::Events::MonitorDisconnectEvent", "[events][monitordisconnectevent]")
{
    //TODO Implement tests when TRAP::Monitor can be used in tests.

    // const TRAP::Events::MonitorDisconnectEvent mdcevent = TRAP::Events::MonitorDisconnectEvent{MONITOR};

    SECTION("Constructor")
    {
        // const TRAP::Events::MonitorDisconnectEvent mdceventCopyConstructor(mdcpevent); //Deleted

        // TRAP::Events::MonitorDisconnectEvent mdcevent1 = TRAP::Events::MonitorDisconnectEvent{MONITOR};
        // const TRAP::Events::MonitorDisconnectEvent mdcevent1MoveConstructor = TRAP::Events::MonitorDisconnectEvent(std::move(mdcevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::MonitorDisconnectEvent mdceventAssignment = TRAP::Events::MonitorDisconnectEvent{MONITOR};

        // const TRAP::Events::MonitorDisconnectEvent mdceventCopyAssignment = mdcevent; //Deleted
        // const TRAP::Events::MonitorDisconnectEvent mdceventMoveAssignment = std::move(mdceventAssignment);
    }

    SECTION("GetMonitor()")
    {
        // const TRAP::Events::MonitorDisconnectEvent mdcevent1 = TRAP::Events::MonitorDisconnectEvent{MONITOR};

        // REQUIRE(mdcevent1.GetMonitor() == MONITOR);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::MonitorDisconnectEvent::GetStaticType() == TRAP::Events::EventType::MonitorDisconnect);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(mdcevent.GetEventType() == TRAP::Events::EventType::MonitorDisconnect);
    }

    SECTION("GetName()")
    {
        // REQUIRE(mdcevent.GetName() == "MonitorDisconnect");
    }

    SECTION("ToString()")
    {
        // REQUIRE(mdcevent.ToString() == fmt::format("MonitorDisconnectEvent: {} ({})", mdcevent.GetMonitor().GetName(), mdcevent.GetMonitor().GetID()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(mdcevent.GetCategoryFlags() == TRAP::Events::EventCategory::Monitor);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(mdcevent.IsInCategory(TRAP::Events::EventCategory::Monitor));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", mdcevent) == fmt::format("MonitorDisconnectEvent: {} ({})", mdcevent.GetMonitor().GetName(), mdcevent.GetMonitor().GetID()));
    }
}
