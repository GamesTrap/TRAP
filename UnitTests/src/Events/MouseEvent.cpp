#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Events/MouseEvent.h"

TEST_CASE("TRAP::Events::MouseMoveEvent", "[events][mousemoveevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::MouseMoveEvent mmevent = TRAP::Events::MouseMoveEvent{-10.0f, 100.0f, WINDOW};

    SECTION("Constructor")
    {
        // const TRAP::Events::MouseMoveEvent mmeventCopyConstructor(mmevent); //Deleted

        // TRAP::Events::MouseMoveEvent mmevent1 = TRAP::Events::MouseMoveEvent{-10.0f, 100.0f, WINDOW};
        // const TRAP::Events::MouseMoveEvent mmevent1MoveConstructor = TRAP::Events::MouseMoveEvent(std::move(mmevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::MouseMoveEvent mmeventAssignment = TRAP::Events::MouseMoveEvent{-10.0f, 100.0f, WINDOW};

        // const TRAP::Events::MouseMoveEvent mmeventCopyAssignment = mmevent; //Deleted
        // const TRAP::Events::MouseMoveEvent mmeventMoveAssignment = std::move(mmeventAssignment);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::MouseMoveEvent mmevent1 = TRAP::Events::MouseMoveEvent{-10.0f, 100.0f, WINDOW};

        // REQUIRE(mmevent1.GetWindow() == WINDOW);
    }

    SECTION("GetX()")
    {
        // const TRAP::Events::MouseMoveEvent mmevent1 = TRAP::Events::MouseMoveEvent{-10.0f, 100.0f, WINDOW};

        // REQUIRE(mmevent1.GetX() == -10.0f);
    }

    SECTION("GetY()")
    {
        // const TRAP::Events::MouseMoveEvent mmevent1 = TRAP::Events::MouseMoveEvent{-10.0f, 100.0f, WINDOW};

        // REQUIRE(mmevent1.GetY() == 100.0f);
    }

    SECTION("GetPosition()")
    {
        // const TRAP::Events::MouseMoveEvent mmevent1 = TRAP::Events::MouseMoveEvent{-10.0f, 100.0f, WINDOW};

        // REQUIRE(mmevent1.GetPosition() == TRAP::Math::Vec2(mmevent.GetX(), mmevent.GetY()));
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::MouseMoveEvent::GetStaticType() == TRAP::Events::EventType::MouseMove);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(mmevent.GetEventType() == TRAP::Events::EventType::MouseMove);
    }

    SECTION("GetName()")
    {
        // REQUIRE(mmevent.GetName() == "MouseMove");
    }

    SECTION("ToString()")
    {
        // REQUIRE(mmevent.ToString() == fmt::format("MouseMoveEvent: {}, {} (\"{}\")", mmevent.GetX(), mmevent.GetY(), mmevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(mmevent.GetCategoryFlags() == TRAP::Events::EventCategory::Mouse);
        // REQUIRE(mmevent.GetCategoryFlags() == TRAP::Events::EventCategory::Input);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(mmevent.IsInCategory(TRAP::Events::EventCategory::Mouse));
        // REQUIRE(mmevent.IsInCategory(TRAP::Events::EventCategory::Input));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", mmevent) == fmt::format("MouseMoveEvent: {}, {} (\"{}\")", mmevent.GetX(), mmevent.GetY(), mmevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::MouseScrollEvent", "[events][mousescrollevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::MouseScrollEvent msevent = TRAP::Events::MouseScrollEvent{-10.0f, 100.0f, WINDOW};

    SECTION("Constructor")
    {
        // const TRAP::Events::MouseScrollEvent mseventCopyConstructor(msevent); //Deleted

        // TRAP::Events::MouseScrollEvent msevent1 = TRAP::Events::MouseScrollEvent{-10.0f, 100.0f, WINDOW};
        // const TRAP::Events::MouseScrollEvent msevent1MoveConstructor = TRAP::Events::MouseScrollEvent(std::move(msevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::MouseScrollEvent mseventAssignment = TRAP::Events::MouseScrollEvent{-10.0f, 100.0f, WINDOW};

        // const TRAP::Events::MouseScrollEvent mseventCopyAssignment = msevent; //Deleted
        // const TRAP::Events::MouseScrollEvent mseventMoveAssignment = std::move(mseventAssignment);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::MouseScrollEvent msevent1 = TRAP::Events::MouseScrollEvent{-10.0f, 100.0f, WINDOW};

        // REQUIRE(msevent1.GetWindow() == WINDOW);
    }

    SECTION("GetXOffset()")
    {
        // const TRAP::Events::MouseScrollEvent msevent1 = TRAP::Events::MouseScrollEvent{-10.0f, 100.0f, WINDOW};

        // REQUIRE(msevent1.GetXOffset() == -10.0f);
    }

    SECTION("GetYOffset()")
    {
        // const TRAP::Events::MouseScrollEvent msevent1 = TRAP::Events::MouseScrollEvent{-10.0f, 100.0f, WINDOW};

        // REQUIRE(msevent1.GetYOffset() == 100.0f);
    }

    SECTION("GetOffset()")
    {
        // const TRAP::Events::MouseScrollEvent msevent1 = TRAP::Events::MouseScrollEvent{-10.0f, 100.0f, WINDOW};

        // REQUIRE(msevent1.GetOffset() == TRAP::Math::Vec2(msevent.GetX(), msevent.GetY()));
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::MouseScrollEvent::GetStaticType() == TRAP::Events::EventType::MouseScroll);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(msevent.GetEventType() == TRAP::Events::EventType::MouseScroll);
    }

    SECTION("GetName()")
    {
        // REQUIRE(msevent.GetName() == "MouseScroll");
    }

    SECTION("ToString()")
    {
        // REQUIRE(msevent.ToString() == fmt::format("MouseScrollEvent: {}, {} (\"{}\")", msevent.GetXOffset(), msevent.GetYOffset(), msevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(msevent.GetCategoryFlags() == TRAP::Events::EventCategory::Mouse);
        // REQUIRE(msevent.GetCategoryFlags() == TRAP::Events::EventCategory::Input);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(msevent.IsInCategory(TRAP::Events::EventCategory::Mouse));
        // REQUIRE(msevent.IsInCategory(TRAP::Events::EventCategory::Input));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", msevent) == fmt::format("MouseScrollEvent: {}, {} (\"{}\")", msevent.GetXOffset(), msevent.GetYOffset(), msevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::MouseButtonPressEvent", "[events][mousebuttonpressevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::MouseButtonPressEvent mbpevent = TRAP::Events::MouseButtonPressEvent{TRAP::Input::MouseButton::Four, WINDOW};

    SECTION("Constructor")
    {
        // const TRAP::Events::MouseButtonPressEvent mbpeventCopyConstructor(mbpevent); //Deleted

        // TRAP::Events::MouseButtonPressEvent mbpevent1 = TRAP::Events::MouseButtonPressEvent{TRAP::Input::MouseButton::Four, WINDOW};
        // const TRAP::Events::MouseButtonPressEvent mbpevent1MoveConstructor = TRAP::Events::MouseButtonPressEvent(std::move(mbpevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::MouseButtonPressEvent mbpeventAssignment = TRAP::Events::MouseButtonPressEvent{TRAP::Input::MouseButton::Four, WINDOW};

        // const TRAP::Events::MouseButtonPressEvent mbpeventCopyAssignment = mbpevent; //Deleted
        // const TRAP::Events::MouseButtonPressEvent mbpeventMoveAssignment = std::move(mbpeventAssignment);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::MouseButtonPressEvent mbpevent1 = TRAP::Events::MouseButtonPressEvent{TRAP::Input::MouseButton::Four, WINDOW};

        // REQUIRE(mbpevent1.GetWindow() == WINDOW);
    }

    SECTION("GetMouseButton()")
    {
        // const TRAP::Events::MouseButtonPressEvent mbpevent1 = TRAP::Events::MouseButtonPressEvent{TRAP::Input::MouseButton::Four, WINDOW};

        // REQUIRE(mbpevent1.GetMouseButton() == TRAP::Input::MouseButton::Four);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::MouseButtonPressEvent::GetStaticType() == TRAP::Events::EventType::MouseButtonPress);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(mbpevent.GetEventType() == TRAP::Events::EventType::MouseButtonPress);
    }

    SECTION("GetName()")
    {
        // REQUIRE(mbpevent.GetName() == "MouseButtonPress");
    }

    SECTION("ToString()")
    {
        // REQUIRE(mbpevent.ToString() == fmt::format("MouseButtonPressEvent: {}({}) (\"{}\")", mbpevent.GetMouseButton(), std::to_underlying(mbpevent.GetMouseButton()), mbpevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(mbpevent.GetCategoryFlags() == TRAP::Events::EventCategory::Mouse);
        // REQUIRE(mbpevent.GetCategoryFlags() == TRAP::Events::EventCategory::Input);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(mbpevent.IsInCategory(TRAP::Events::EventCategory::Mouse));
        // REQUIRE(mbpevent.IsInCategory(TRAP::Events::EventCategory::Input));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", mbpevent) == fmt::format("MouseButtonPressEvent: {}({}) (\"{}\")", mbpevent.GetMouseButton(), std::to_underlying(mbpevent.GetMouseButton()), mbpevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::MouseButtonReleaseEvent", "[events][mousebuttonreleaseevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::MouseButtonReleaseEvent mbrevent = TRAP::Events::MouseButtonReleaseEvent{TRAP::Input::MouseButton::Four, WINDOW};

    SECTION("Constructor")
    {
        // const TRAP::Events::MouseButtonReleaseEvent mbreventCopyConstructor(mbrevent); //Deleted

        // TRAP::Events::MouseButtonReleaseEvent mbrevent1 = TRAP::Events::MouseButtonReleaseEvent{TRAP::Input::MouseButton::Four, WINDOW};
        // const TRAP::Events::MouseButtonReleaseEvent mbrevent1MoveConstructor = TRAP::Events::MouseButtonReleaseEvent(std::move(mbrevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::MouseButtonReleaseEvent mbreventAssignment = TRAP::Events::MouseButtonReleaseEvent{TRAP::Input::MouseButton::Four, WINDOW};

        // const TRAP::Events::MouseButtonReleaseEvent mbreventCopyAssignment = mbrevent; //Deleted
        // const TRAP::Events::MouseButtonReleaseEvent mbreventMoveAssignment = std::move(mbreventAssignment);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::MouseButtonReleaseEvent mbrevent1 = TRAP::Events::MouseButtonReleaseEvent{TRAP::Input::MouseButton::Four, WINDOW};

        // REQUIRE(mbrevent1.GetWindow() == WINDOW);
    }

    SECTION("GetMouseButton()")
    {
        // const TRAP::Events::MouseButtonReleaseEvent mbrevent1 = TRAP::Events::MouseButtonReleaseEvent{TRAP::Input::MouseButton::Four, WINDOW};

        // REQUIRE(mbrevent1.GetMouseButton() == TRAP::Input::MouseButton::Four);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::MouseButtonReleaseEvent::GetStaticType() == TRAP::Events::EventType::MouseButtonRelease);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(mbrevent.GetEventType() == TRAP::Events::EventType::MouseButtonRelease);
    }

    SECTION("GetName()")
    {
        // REQUIRE(mbrevent.GetName() == "MouseButtonRelease");
    }

    SECTION("ToString()")
    {
        // REQUIRE(mbrevent.ToString() == fmt::format("MouseButtonReleaseEvent: {}({}) (\"{}\")", mbpevent.GetMouseButton(), std::to_underlying(mbpevent.GetMouseButton()), mbpevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(mbrevent.GetCategoryFlags() == TRAP::Events::EventCategory::Mouse);
        // REQUIRE(mbrevent.GetCategoryFlags() == TRAP::Events::EventCategory::Input);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(mbrevent.IsInCategory(TRAP::Events::EventCategory::Mouse));
        // REQUIRE(mbrevent.IsInCategory(TRAP::Events::EventCategory::Input));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", mbrevent) == fmt::format("MouseButtonReleaseEvent: {}({}) (\"{}\")", mbpevent.GetMouseButton(), std::to_underlying(mbpevent.GetMouseButton()), mbpevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::MouseEnterEvent", "[events][mouseenterevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::MouseEnterEvent meevent = TRAP::Events::MouseEnterEvent{WINDOW};

    SECTION("Constructor")
    {
        // const TRAP::Events::MouseEnterEvent meeventCopyConstructor(meevent); //Deleted

        // TRAP::Events::MouseEnterEvent meevent1 = TRAP::Events::MouseEnterEvent{WINDOW};
        // const TRAP::Events::MouseEnterEvent meevent1MoveConstructor = TRAP::Events::MouseEnterEvent(std::move(meevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::MouseEnterEvent meeventAssignment = TRAP::Events::MouseEnterEvent{WINDOW};

        // const TRAP::Events::MouseEnterEvent meeventCopyAssignment = meevent; //Deleted
        // const TRAP::Events::MouseEnterEvent meeventMoveAssignment = std::move(meeventAssignment);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::MouseEnterEvent meevent1 = TRAP::Events::MouseEnterEvent{WINDOW};

        // REQUIRE(meevent1.GetWindow() == WINDOW);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::MouseEnterEvent::GetStaticType() == TRAP::Events::EventType::MouseEnter);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(meevent.GetEventType() == TRAP::Events::EventType::MouseEnter);
    }

    SECTION("GetName()")
    {
        // REQUIRE(meevent.GetName() == "MouseEnter");
    }

    SECTION("ToString()")
    {
        // REQUIRE(meevent.ToString() == fmt::format("MouseEnterEvent: (\"{}\")", meevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(meevent.GetCategoryFlags() == TRAP::Events::EventCategory::Mouse);
        // REQUIRE(meevent.GetCategoryFlags() == TRAP::Events::EventCategory::Input);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(meevent.IsInCategory(TRAP::Events::EventCategory::Mouse));
        // REQUIRE(meevent.IsInCategory(TRAP::Events::EventCategory::Input));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", meevent) == fmt::format("MouseEnterEvent: (\"{}\")", meevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::MouseLeaveEvent", "[events][mouseleaveevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::MouseLeaveEvent mlevent = TRAP::Events::MouseLeaveEvent{WINDOW};

    SECTION("Constructor")
    {
        // const TRAP::Events::MouseLeaveEvent mleventCopyConstructor(mlevent); //Deleted

        // TRAP::Events::MouseLeaveEvent mlevent1 = TRAP::Events::MouseLeaveEvent{WINDOW};
        // const TRAP::Events::MouseLeaveEvent mlevent1MoveConstructor = TRAP::Events::MouseLeaveEvent(std::move(mlevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::MouseLeaveEvent mleventAssignment = TRAP::Events::MouseLeaveEvent{WINDOW};

        // const TRAP::Events::MouseLeaveEvent mleventCopyAssignment = mlevent; //Deleted
        // const TRAP::Events::MouseLeaveEvent mleventMoveAssignment = std::move(mleventAssignment);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::MouseLeaveEvent mlevent1 = TRAP::Events::MouseLeaveEvent{WINDOW};

        // REQUIRE(mlevent1.GetWindow() == WINDOW);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::MouseLeaveEvent::GetStaticType() == TRAP::Events::EventType::MouseLeave);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(mlevent.GetEventType() == TRAP::Events::EventType::MouseLeave);
    }

    SECTION("GetName()")
    {
        // REQUIRE(mlevent.GetName() == "MouseLeave");
    }

    SECTION("ToString()")
    {
        // REQUIRE(mlevent.ToString() == fmt::format("MouseLeaveEvent: (\"{}\")", mlevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(mlevent.GetCategoryFlags() == TRAP::Events::EventCategory::Mouse);
        // REQUIRE(mlevent.GetCategoryFlags() == TRAP::Events::EventCategory::Input);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(mlevent.IsInCategory(TRAP::Events::EventCategory::Mouse));
        // REQUIRE(mlevent.IsInCategory(TRAP::Events::EventCategory::Input));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", mlevent) == fmt::format("MouseLeaveEvent: (\"{}\")", mlevent.GetWindow().GetTitle()));
    }
}
