#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Utils/Win.h"
#include "TRAP/src/Events/ControllerEvent.h"
#include "TRAP/src/Events/Event.h"

TEST_CASE("TRAP::Events::EventDispatcher", "[events][eventdispatcher]")
{
    SECTION("Constructor")
    {
        TRAP::Events::ControllerConnectEvent cevent = TRAP::Events::ControllerConnectEvent{TRAP::Input::Controller::One};

        const TRAP::Events::EventDispatcher eventDispatcher{cevent};
        const TRAP::Events::EventDispatcher eventDispatcherCopyConstructor(eventDispatcher);

        TRAP::Events::EventDispatcher eventDispatcher1 = TRAP::Events::EventDispatcher{cevent};
        const TRAP::Events::EventDispatcher eventDispatcher1MoveConstructor = TRAP::Events::EventDispatcher(std::move(eventDispatcher1));
    }

    SECTION("Assignment")
    {
        TRAP::Events::ControllerConnectEvent cevent = TRAP::Events::ControllerConnectEvent{TRAP::Input::Controller::One};

        TRAP::Events::EventDispatcher eventDispatcherAssignment = TRAP::Events::EventDispatcher{cevent};

        const TRAP::Events::EventDispatcher eventDispatcherCopyAssignment = eventDispatcherAssignment;
        const TRAP::Events::EventDispatcher eventDispatcherMoveAssignment = std::move(eventDispatcherAssignment);
    }

    SECTION("Dispatch()")
    {
        TRAP::Events::ControllerConnectEvent cevent = TRAP::Events::ControllerConnectEvent{TRAP::Input::Controller::One};
        const TRAP::Events::EventDispatcher eventDispatcher{cevent};

        static constexpr auto Callback = [](const TRAP::Events::ControllerConnectEvent& conEvent) -> bool
        {
            REQUIRE(conEvent.GetController() == TRAP::Input::Controller::One);

            return true; //Event was handled
        };

        REQUIRE(eventDispatcher.Dispatch<TRAP::Events::ControllerConnectEvent>(Callback));
        REQUIRE_FALSE(eventDispatcher.Dispatch<TRAP::Events::ControllerConnectEvent>(Callback)); //Event was handled
    }
}
