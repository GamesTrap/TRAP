#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Events/WindowEvent.h"

TEST_CASE("TRAP::Events::WindowResizeEvent", "[events][windowresizevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::WindowResizeEvent wresievent = TRAP::Events::WindowResizeEvent{1280u, 720u, WINDOW};

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Events::WindowResizeEvent>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Events::WindowResizeEvent>);
        STATIC_REQUIRE(std::movable<TRAP::Events::WindowResizeEvent>);
        STATIC_REQUIRE(std::derived_from<TRAP::Events::WindowResizeEvent, TRAP::Events::Event>);
    }

    SECTION("Constructor")
    {
        // const TRAP::Events::WindowResizeEvent wresieventCopyConstructor(wresievent); //Deleted

        // TRAP::Events::WindowResizeEvent wresievent1 = TRAP::Events::WindowResizeEvent{1280u, 720u, WINDOW};
        // const TRAP::Events::WindowResizeEvent wresievent1MoveConstructor = TRAP::Events::WindowResizeEvent(std::move(wresievent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::WindowResizeEvent wresieventAssignment = TRAP::Events::WindowResizeEvent{1280u, 720u, WINDOW};

        // const TRAP::Events::WindowResizeEvent wresieventCopyAssignment = wresievent; //Deleted
        // const TRAP::Events::WindowResizeEvent wresieventMoveAssignment = std::move(wresieventAssignment);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::WindowResizeEvent wresievent1 = TRAP::Events::WindowResizeEvent{1280u, 720u, WINDOW};

        // REQUIRE(wresievent1.GetWindow() == WINDOW);
    }

    SECTION("GetWidth()")
    {
        // const TRAP::Events::WindowResizeEvent wresievent1 = TRAP::Events::WindowResizeEvent{1280u, 720u, WINDOW};

        // REQUIRE(wresievent1.GetWidth() == 1280u);
    }

    SECTION("GetHeight()")
    {
        // const TRAP::Events::WindowResizeEvent wresievent1 = TRAP::Events::WindowResizeEvent{1280u, 720u, WINDOW};

        // REQUIRE(wresievent1.GetWidth() == 720u);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::WindowResizeEvent::GetStaticType() == TRAP::Events::EventType::WindowResize);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(wresievent.GetEventType() == TRAP::Events::EventType::WindowResize);
    }

    SECTION("GetName()")
    {
        // REQUIRE(wresievent.GetName() == "WindowResize");
    }

    SECTION("ToString()")
    {
        // REQUIRE(wresievent.ToString() == fmt::format("WindowResizeEvent: {}x{} (\"{}\")", wresievent.GetWidth(), wresievent.GetHeight(), wresievent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(wresievent.GetCategoryFlags() == TRAP::Events::EventCategory::Window);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(wresievent.IsInCategory(TRAP::Events::EventCategory::Window));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", wresievent) == fmt::format("WindowResizeEvent: {}x{} (\"{}\")", wresievent.GetWidth(), wresievent.GetHeight(), wresievent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::WindowMinimizeEvent", "[events][windowminimizeevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::WindowMinimizeEvent wmievent = TRAP::Events::WindowMinimizeEvent{WINDOW};

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Events::WindowMinimizeEvent>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Events::WindowMinimizeEvent>);
        STATIC_REQUIRE(std::movable<TRAP::Events::WindowMinimizeEvent>);
        STATIC_REQUIRE(std::derived_from<TRAP::Events::WindowMinimizeEvent, TRAP::Events::Event>);
    }

    SECTION("Constructor")
    {
        // const TRAP::Events::WindowMinimizeEvent wmieventCopyConstructor(wmievent); //Deleted

        // TRAP::Events::WindowMinimizeEvent wmievent1 = TRAP::Events::WindowMinimizeEvent{WINDOW};
        // const TRAP::Events::WindowMinimizeEvent wmievent1MoveConstructor = TRAP::Events::WindowMinimizeEvent(std::move(wmievent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::WindowMinimizeEvent wmieventAssignment = TRAP::Events::WindowMinimizeEvent{WINDOW};

        // const TRAP::Events::WindowMinimizeEvent wmieventCopyAssignment = wmievent; //Deleted
        // const TRAP::Events::WindowMinimizeEvent wmieventMoveAssignment = std::move(wmieventAssignment);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::WindowMinimizeEvent wmievent1 = TRAP::Events::WindowMinimizeEvent{WINDOW};

        // REQUIRE(wmievent1.GetWindow() == WINDOW);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::WindowMinimizeEvent::GetStaticType() == TRAP::Events::EventType::WindowMinimize);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(wmievent.GetEventType() == TRAP::Events::EventType::WindowMinimize);
    }

    SECTION("GetName()")
    {
        // REQUIRE(wmievent.GetName() == "WindowMinimize");
    }

    SECTION("ToString()")
    {
        // REQUIRE(wmievent.ToString() == fmt::format("WindowMinimizeEvent: (\"{}\")", wmievent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(wmievent.GetCategoryFlags() == TRAP::Events::EventCategory::Window);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(wmievent.IsInCategory(TRAP::Events::EventCategory::Window));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", wmievent) == fmt::format("WindowMinimizeEvent: (\"{}\")", wmievent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::WindowMaximizeEvent", "[events][windowmaximizeevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::WindowMaximizeEvent wmaevent = TRAP::Events::WindowMaximizeEvent{WINDOW};

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Events::WindowMaximizeEvent>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Events::WindowMaximizeEvent>);
        STATIC_REQUIRE(std::movable<TRAP::Events::WindowMaximizeEvent>);
        STATIC_REQUIRE(std::derived_from<TRAP::Events::WindowMaximizeEvent, TRAP::Events::Event>);
    }

    SECTION("Constructor")
    {
        // const TRAP::Events::WindowMaximizeEvent wmaeventCopyConstructor(wmaevent); //Deleted

        // TRAP::Events::WindowMaximizeEvent wmaevent1 = TRAP::Events::WindowMaximizeEvent{WINDOW};
        // const TRAP::Events::WindowMaximizeEvent wmaevent1MoveConstructor = TRAP::Events::WindowMaximizeEvent(std::move(wmaevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::WindowMaximizeEvent wmaeventAssignment = TRAP::Events::WindowMaximizeEvent{WINDOW};

        // const TRAP::Events::WindowMaximizeEvent wmaeventCopyAssignment = wmaevent; //Deleted
        // const TRAP::Events::WindowMaximizeEvent wmaeventMoveAssignment = std::move(wmaeventAssignment);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::WindowMaximizeEvent wmaevent1 = TRAP::Events::WindowMaximizeEvent{WINDOW};

        // REQUIRE(wmaevent1.GetWindow() == WINDOW);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::WindowMaximizeEvent::GetStaticType() == TRAP::Events::EventType::WindowMaximize);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(wmaevent.GetEventType() == TRAP::Events::EventType::WindowMaximize);
    }

    SECTION("GetName()")
    {
        // REQUIRE(wmaevent.GetName() == "WindowMaximize");
    }

    SECTION("ToString()")
    {
        // REQUIRE(wmaevent.ToString() == fmt::format("WindowMaximizeEvent: (\"{}\")", wmaevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(wmaevent.GetCategoryFlags() == TRAP::Events::EventCategory::Window);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(wmaevent.IsInCategory(TRAP::Events::EventCategory::Window));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", wmaevent) == fmt::format("WindowMaximizeEvent: (\"{}\")", wmaevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::WindowRestoreEvent", "[events][windowrestoreevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::WindowRestoreEvent wrestevent = TRAP::Events::WindowRestoreEvent{WINDOW};

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Events::WindowRestoreEvent>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Events::WindowRestoreEvent>);
        STATIC_REQUIRE(std::movable<TRAP::Events::WindowRestoreEvent>);
        STATIC_REQUIRE(std::derived_from<TRAP::Events::WindowRestoreEvent, TRAP::Events::Event>);
    }

    SECTION("Constructor")
    {
        // const TRAP::Events::WindowRestoreEvent wresteventCopyConstructor(wrestevent); //Deleted

        // TRAP::Events::WindowRestoreEvent wrestevent1 = TRAP::Events::WindowRestoreEvent{WINDOW};
        // const TRAP::Events::WindowRestoreEvent wrestevent1MoveConstructor = TRAP::Events::WindowRestoreEvent(std::move(wrestevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::WindowRestoreEvent wresteventAssignment = TRAP::Events::WindowRestoreEvent{WINDOW};

        // const TRAP::Events::WindowRestoreEvent wresteventCopyAssignment = wrestevent; //Deleted
        // const TRAP::Events::WindowRestoreEvent wresteventMoveAssignment = std::move(wresteventAssignment);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::WindowRestoreEvent wrestevent1 = TRAP::Events::WindowRestoreEvent{WINDOW};

        // REQUIRE(wrestevent1.GetWindow() == WINDOW);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::WindowRestoreEvent::GetStaticType() == TRAP::Events::EventType::WindowRestore);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(wrestevent.GetEventType() == TRAP::Events::EventType::WindowRestore);
    }

    SECTION("GetName()")
    {
        // REQUIRE(wrestevent.GetName() == "WindowRestore");
    }

    SECTION("ToString()")
    {
        // REQUIRE(wrestevent.ToString() == fmt::format("WindowRestoreEvent: (\"{}\")", wrestevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(wrestevent.GetCategoryFlags() == TRAP::Events::EventCategory::Window);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(wrestevent.IsInCategory(TRAP::Events::EventCategory::Window));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", wrestevent) == fmt::format("WindowRestoreEvent: (\"{}\")", wrestevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::WindowCloseEvent", "[events][windowcloseevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::WindowCloseEvent wcevent = TRAP::Events::WindowCloseEvent{WINDOW};

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Events::WindowCloseEvent>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Events::WindowCloseEvent>);
        STATIC_REQUIRE(std::movable<TRAP::Events::WindowCloseEvent>);
        STATIC_REQUIRE(std::derived_from<TRAP::Events::WindowCloseEvent, TRAP::Events::Event>);
    }

    SECTION("Constructor")
    {
        // const TRAP::Events::WindowCloseEvent wceventCopyConstructor(wcevent); //Deleted

        // TRAP::Events::WindowCloseEvent wcevent1 = TRAP::Events::WindowCloseEvent{WINDOW};
        // const TRAP::Events::WindowCloseEvent wcevent1MoveConstructor = TRAP::Events::WindowCloseEvent(std::move(wcevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::WindowCloseEvent wceventAssignment = TRAP::Events::WindowCloseEvent{WINDOW};

        // const TRAP::Events::WindowCloseEvent wceventCopyAssignment = wcevent; //Deleted
        // const TRAP::Events::WindowCloseEvent wceventMoveAssignment = std::move(wceventAssignment);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::WindowCloseEvent wcevent1 = TRAP::Events::WindowCloseEvent{WINDOW};

        // REQUIRE(wcevent1.GetWindow() == WINDOW);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::WindowCloseEvent::GetStaticType() == TRAP::Events::EventType::WindowClose);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(wcevent.GetEventType() == TRAP::Events::EventType::WindowClose);
    }

    SECTION("GetName()")
    {
        // REQUIRE(wcevent.GetName() == "WindowClose");
    }

    SECTION("ToString()")
    {
        // REQUIRE(wcevent.ToString() == fmt::format("WindowCloseEvent: (\"{}\")", wcevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(wcevent.GetCategoryFlags() == TRAP::Events::EventCategory::Window);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(wcevent.IsInCategory(TRAP::Events::EventCategory::Window));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", wcevent) == fmt::format("WindowCloseEvent: (\"{}\")", wcevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::WindowMoveEvent", "[events][windowmoveevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::WindowMoveEvent wmevent = TRAP::Events::WindowMoveEvent{10, 100, WINDOW};

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Events::WindowMoveEvent>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Events::WindowMoveEvent>);
        STATIC_REQUIRE(std::movable<TRAP::Events::WindowMoveEvent>);
        STATIC_REQUIRE(std::derived_from<TRAP::Events::WindowMoveEvent, TRAP::Events::Event>);
    }

    SECTION("Constructor")
    {
        // const TRAP::Events::WindowMoveEvent wmeventCopyConstructor(wmevent); //Deleted

        // TRAP::Events::WindowMoveEvent wmevent1 = TRAP::Events::WindowMoveEvent{10, 100, WINDOW};
        // const TRAP::Events::WindowMoveEvent wmevent1MoveConstructor = TRAP::Events::WindowMoveEvent(std::move(wmevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::WindowMoveEvent wmeventAssignment = TRAP::Events::WindowMoveEvent{10, 100, WINDOW};

        // const TRAP::Events::WindowMoveEvent wmeventCopyAssignment = wmevent; //Deleted
        // const TRAP::Events::WindowMoveEvent wmeventMoveAssignment = std::move(wmeventAssignment);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::WindowMoveEvent wmevent1 = TRAP::Events::WindowMoveEvent{10, 100, WINDOW};

        // REQUIRE(wmevent1.GetWindow() == WINDOW);
    }

    SECTION("GetX()")
    {
        // const TRAP::Events::WindowMoveEvent wmevent1 = TRAP::Events::WindowMoveEvent{10, 100, WINDOW};

        // REQUIRE(wmevent1.GetX() == 10);
    }

    SECTION("GetY()")
    {
        // const TRAP::Events::WindowMoveEvent wmevent1 = TRAP::Events::WindowMoveEvent{10, 100, WINDOW};

        // REQUIRE(wmevent1.GetY() == 100);
    }

    SECTION("GetPosition()")
    {
        // const TRAP::Events::WindowMoveEvent wmevent1 = TRAP::Events::WindowMoveEvent{10, 100, WINDOW};

        // REQUIRE(wmevent1.GetPosition() == TRAP::Math::Vec2i(10, 100));
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::WindowMoveEvent::GetStaticType() == TRAP::Events::EventType::WindowMove);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(wmevent.GetEventType() == TRAP::Events::EventType::WindowMove);
    }

    SECTION("GetName()")
    {
        // REQUIRE(wmevent.GetName() == "WindowMove");
    }

    SECTION("ToString()")
    {
        // REQUIRE(wmevent.ToString() == fmt::format("WindowMoveEvent: {}, {} (\"{}\")", wmevent.GetX(), wmevent.GetY(), wmevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(wmevent.GetCategoryFlags() == TRAP::Events::EventCategory::Window);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(wmevent.IsInCategory(TRAP::Events::EventCategory::Window));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", wmevent) == fmt::format("WindowMoveEvent: {}, {} (\"{}\")", wmevent.GetX(), wmevent.GetY(), wmevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::WindowFocusEvent", "[events][windowfocusevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::WindowFocusEvent wfevent = TRAP::Events::WindowFocusEvent{WINDOW};

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Events::WindowFocusEvent>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Events::WindowFocusEvent>);
        STATIC_REQUIRE(std::movable<TRAP::Events::WindowFocusEvent>);
        STATIC_REQUIRE(std::derived_from<TRAP::Events::WindowFocusEvent, TRAP::Events::Event>);
    }

    SECTION("Constructor")
    {
        // const TRAP::Events::WindowFocusEvent wfeventCopyConstructor(wfevent); //Deleted

        // TRAP::Events::WindowFocusEvent wfevent1 = TRAP::Events::WindowFocusEvent{WINDOW};
        // const TRAP::Events::WindowFocusEvent wfevent1MoveConstructor = TRAP::Events::WindowFocusEvent(std::move(wfevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::WindowFocusEvent wfeventAssignment = TRAP::Events::WindowFocusEvent{WINDOW};

        // const TRAP::Events::WindowFocusEvent wfeventCopyAssignment = wfevent; //Deleted
        // const TRAP::Events::WindowFocusEvent wfeventMoveAssignment = std::move(wfeventAssignment);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::WindowFocusEvent wfevent1 = TRAP::Events::WindowFocusEvent{WINDOW};

        // REQUIRE(wfevent1.GetWindow() == WINDOW);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::WindowFocusEvent::GetStaticType() == TRAP::Events::EventType::WindowFocus);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(wfevent.GetEventType() == TRAP::Events::EventType::WindowFocus);
    }

    SECTION("GetName()")
    {
        // REQUIRE(wfevent.GetName() == "WindowFocus");
    }

    SECTION("ToString()")
    {
        // REQUIRE(wfevent.ToString() == fmt::format("WindowFocusEvent: (\"{}\")", wfevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(wfevent.GetCategoryFlags() == TRAP::Events::EventCategory::Window);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(wfevent.IsInCategory(TRAP::Events::EventCategory::Window));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", wfevent) == fmt::format("WindowFocusEvent: (\"{}\")", wfevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::WindowLostFocusEvent", "[events][windowlostfocusevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::WindowLostFocusEvent wlfevent = TRAP::Events::WindowLostFocusEvent{WINDOW};

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Events::WindowLostFocusEvent>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Events::WindowLostFocusEvent>);
        STATIC_REQUIRE(std::movable<TRAP::Events::WindowLostFocusEvent>);
        STATIC_REQUIRE(std::derived_from<TRAP::Events::WindowLostFocusEvent, TRAP::Events::Event>);
    }

    SECTION("Constructor")
    {
        // const TRAP::Events::WindowLostFocusEvent wlfeventCopyConstructor(wlfevent); //Deleted

        // TRAP::Events::WindowLostFocusEvent wlfevent1 = TRAP::Events::WindowLostFocusEvent{WINDOW};
        // const TRAP::Events::WindowLostFocusEvent wlfevent1MoveConstructor = TRAP::Events::WindowLostFocusEvent(std::move(wlfevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::WindowLostFocusEvent wlfeventAssignment = TRAP::Events::WindowLostFocusEvent{WINDOW};

        // const TRAP::Events::WindowLostFocusEvent wlfeventCopyAssignment = wlfevent; //Deleted
        // const TRAP::Events::WindowLostFocusEvent wlfeventMoveAssignment = std::move(wlfeventAssignment);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::WindowLostFocusEvent wlfevent1 = TRAP::Events::WindowLostFocusEvent{WINDOW};

        // REQUIRE(wlfevent1.GetWindow() == WINDOW);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::WindowLostFocusEvent::GetStaticType() == TRAP::Events::EventType::WindowLostFocus);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(wlfevent.GetEventType() == TRAP::Events::EventType::WindowLostFocus);
    }

    SECTION("GetName()")
    {
        // REQUIRE(wlfevent.GetName() == "WindowLostFocus");
    }

    SECTION("ToString()")
    {
        // REQUIRE(wlfevent.ToString() == fmt::format("WindowLostFocusEvent: (\"{}\")", wlfevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(wlfevent.GetCategoryFlags() == TRAP::Events::EventCategory::Window);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(wlfevent.IsInCategory(TRAP::Events::EventCategory::Window));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", wlfevent) == fmt::format("WindowLostFocusEvent: (\"{}\")", wlfevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::WindowDropEvent", "[events][windowdropevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    std::vector<std::string> paths{"someFile.txt", "someFolder/someOtherFile.txt"};
    // const TRAP::Events::WindowDropEvent wdevent = TRAP::Events::WindowDropEvent{paths, WINDOW};

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Events::WindowDropEvent>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Events::WindowDropEvent>);
        STATIC_REQUIRE(std::movable<TRAP::Events::WindowDropEvent>);
        STATIC_REQUIRE(std::derived_from<TRAP::Events::WindowDropEvent, TRAP::Events::Event>);
    }

    SECTION("Constructor")
    {
        // const TRAP::Events::WindowDropEvent wdeventCopyConstructor(wdevent); //Deleted

        // TRAP::Events::WindowDropEvent wdevent1 = TRAP::Events::WindowDropEvent{paths, WINDOW};
        // const TRAP::Events::WindowDropEvent wdevent1MoveConstructor = TRAP::Events::WindowDropEvent(std::move(wdevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::WindowDropEvent wdeventAssignment = TRAP::Events::WindowDropEvent{paths, WINDOW};

        // const TRAP::Events::WindowDropEvent wdeventCopyAssignment = wdevent; //Deleted
        // const TRAP::Events::WindowDropEvent wdeventMoveAssignment = std::move(wdeventAssignment);
    }

    SECTION("GetPaths()")
    {
        // const TRAP::Events::WindowDropEvent wdevent1 = TRAP::Events::WindowDropEvent{paths, WINDOW};

        // REQUIRE(wdevent1.GetPaths() == paths);
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::WindowDropEvent wdevent1 = TRAP::Events::WindowDropEvent{paths, WINDOW};

        // REQUIRE(wdevent1.GetWindow() == WINDOW);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::WindowDropEvent::GetStaticType() == TRAP::Events::EventType::WindowDrop);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(wdevent.GetEventType() == TRAP::Events::EventType::WindowDrop);
    }

    SECTION("GetName()")
    {
        // REQUIRE(wdevent.GetName() == "WindowDrop");
    }

    SECTION("ToString()")
    {
        // const TRAP::Events::WindowDropEvent wdevent1 = TRAP::Events::WindowDropEvent{{}, WINDOW};
        // REQUIRE(wdevent1.ToString() == fmt::format("WindowDropEvent: (\"{}\")", wdevent1.GetWindow().GetTitle()));

        // REQUIRE(wdevent.ToString() == fmt::format("WindowDropEvent: Path(s): \"{}\" (\"{}\")", fmt::join(wdevent.GetPaths(), "\", \""), wdevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(wdevent.GetCategoryFlags() == (TRAP::Events::EventCategory::Window | TRAP::Events::EventCategory::Input));
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(wdevent.IsInCategory(TRAP::Events::EventCategory::Window));
        // REQUIRE(wdevent.IsInCategory(TRAP::Events::EventCategory::Input));
    }

    SECTION("fmt specialization")
    {
        // const TRAP::Events::WindowDropEvent wdevent1 = TRAP::Events::WindowDropEvent{{}, WINDOW};
        // REQUIRE(fmt::format("{}", wdevent1) == fmt::format("WindowDropEvent: (\"{}\")", wdevent1.GetWindow().GetTitle()));

        // REQUIRE(fmt::format("{}", wdevent) == fmt::format("WindowDropEvent: Path(s): \"{}\" (\"{}\")", fmt::join(wdevent.GetPaths(), "\", \""), wdevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::WindowContentScaleEvent", "[events][windowcontentscaleevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::WindowContentScaleEvent wcsevent = TRAP::Events::WindowContentScaleEvent{2.0f, 1.0f, WINDOW};

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Events::WindowContentScaleEvent>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Events::WindowContentScaleEvent>);
        STATIC_REQUIRE(std::movable<TRAP::Events::WindowContentScaleEvent>);
        STATIC_REQUIRE(std::derived_from<TRAP::Events::WindowContentScaleEvent, TRAP::Events::Event>);
    }

    SECTION("Constructor")
    {
        // const TRAP::Events::WindowContentScaleEvent wcseventCopyConstructor(wcsevent); //Deleted

        // TRAP::Events::WindowContentScaleEvent wcsevent1 = TRAP::Events::WindowContentScaleEvent{2.0f, 1.0f, WINDOW};
        // const TRAP::Events::WindowContentScaleEvent wcsevent1MoveConstructor = TRAP::Events::WindowContentScaleEvent(std::move(wcsevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::WindowContentScaleEvent wcseventAssignment = TRAP::Events::WindowContentScaleEvent{2.0f, 1.0f, WINDOW};

        // const TRAP::Events::WindowContentScaleEvent wcseventCopyAssignment = wcsevent; //Deleted
        // const TRAP::Events::WindowContentScaleEvent wcseventMoveAssignment = std::move(wcseventAssignment);
    }

    SECTION("GetXScale()")
    {
        // const TRAP::Events::WindowContentScaleEvent wcsevent1 = TRAP::Events::WindowContentScaleEvent{2.0f, 1.0f, WINDOW};

        // REQUIRE(wcsevent1.GetXScale() == 2.0f);
    }

    SECTION("GetYScale()")
    {
        // const TRAP::Events::WindowContentScaleEvent wcsevent1 = TRAP::Events::WindowContentScaleEvent{2.0f, 1.0f, WINDOW};

        // REQUIRE(wcsevent1.GetYScale() == 1.0f);
    }

    SECTION("GetScale()")
    {
        // const TRAP::Events::WindowContentScaleEvent wcsevent1 = TRAP::Events::WindowContentScaleEvent{2.0f, 1.0f, WINDOW};

        // REQUIRE(wcsevent1.GetScale() == TRAP::Math::Vec2(2.0f, 1.0f));
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::WindowContentScaleEvent wcsevent1 = TRAP::Events::WindowContentScaleEvent{2.0f, 1.0f, WINDOW};

        // REQUIRE(wcsevent1.GetWindow() == WINDOW);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::WindowContentScaleEvent::GetStaticType() == TRAP::Events::EventType::WindowContentScale);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(wcsevent.GetEventType() == TRAP::Events::EventType::WindowContentScale);
    }

    SECTION("GetName()")
    {
        // REQUIRE(wcsevent.GetName() == "WindowContentScale");
    }

    SECTION("ToString()")
    {
        // REQUIRE(wcsevent.ToString() == fmt::format("WindowContentScaleEvent: {}x{} (\"{}\")", wcsevent.GetXScale(), wcsevent.GetYScale(), wcsevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(wcsevent.GetCategoryFlags() == TRAP::Events::EventCategory::Window);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(wcsevent.IsInCategory(TRAP::Events::EventCategory::Window));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", wcsevent) == fmt::format("WindowContentScaleEvent: {}x{} (\"{}\")", wcsevent.GetXScale(), wcsevent.GetYScale(), wcsevent.GetWindow().GetTitle()));
    }
}

TEST_CASE("TRAP::Events::FrameBufferResizeEvent", "[events][framebufferresizeevent]")
{
    //TODO Implement tests when TRAP::Window can be used in tests.

    // const TRAP::Events::FrameBufferResizeEvent frevent = TRAP::Events::FrameBufferResizeEvent{1280u, 720u, WINDOW};

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Events::FrameBufferResizeEvent>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Events::FrameBufferResizeEvent>);
        STATIC_REQUIRE(std::movable<TRAP::Events::FrameBufferResizeEvent>);
        STATIC_REQUIRE(std::derived_from<TRAP::Events::FrameBufferResizeEvent, TRAP::Events::Event>);
    }

    SECTION("Constructor")
    {
        // const TRAP::Events::FrameBufferResizeEvent freventCopyConstructor(frevent); //Deleted

        // TRAP::Events::FrameBufferResizeEvent frevent1 = TRAP::Events::FrameBufferResizeEvent{1280u, 720u, WINDOW};
        // const TRAP::Events::FrameBufferResizeEvent frevent1MoveConstructor = TRAP::Events::FrameBufferResizeEvent(std::move(frevent1));
    }

    SECTION("Assignment")
    {
        // TRAP::Events::FrameBufferResizeEvent freventAssignment = TRAP::Events::FrameBufferResizeEvent{1280u, 720u, WINDOW};

        // const TRAP::Events::FrameBufferResizeEvent freventCopyAssignment = frevent; //Deleted
        // const TRAP::Events::FrameBufferResizeEvent freventMoveAssignment = std::move(freventAssignment);
    }

    SECTION("GetWidth()")
    {
        // const TRAP::Events::FrameBufferResizeEvent frevent1 = TRAP::Events::FrameBufferResizeEvent{1280u, 720u, WINDOW};

        // REQUIRE(frevent1.GetWidth() == 1280u);
    }

    SECTION("GetHeight()")
    {
        // const TRAP::Events::FrameBufferResizeEvent frevent1 = TRAP::Events::FrameBufferResizeEvent{1280u, 720u, WINDOW};

        // REQUIRE(frevent1.GetHeight() == 720u);
    }

    SECTION("GetSize()")
    {
        // const TRAP::Events::FrameBufferResizeEvent frevent1 = TRAP::Events::FrameBufferResizeEvent{1280u, 720u, WINDOW};

        // REQUIRE(frevent1.GetSize() == TRAP::Math::Vec2ui(1280u, 720u));
    }

    SECTION("GetAspectRatio()")
    {
        // const TRAP::Events::FrameBufferResizeEvent frevent1 = TRAP::Events::FrameBufferResizeEvent{1280u, 720u, WINDOW};

        // REQUIRE(frevent1.GetAspectRatio() == (1280.0f / 720.0f));
    }

    SECTION("GetWindow()")
    {
        // const TRAP::Events::FrameBufferResizeEvent frevent1 = TRAP::Events::FrameBufferResizeEvent{1280u, 720u, WINDOW};

        // REQUIRE(frevent1.GetWindow() == WINDOW);
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::FrameBufferResizeEvent::GetStaticType() == TRAP::Events::EventType::FrameBufferResize);
    }

    SECTION("GetEventType()")
    {
        // REQUIRE(frevent.GetEventType() == TRAP::Events::EventType::FrameBufferResize);
    }

    SECTION("GetName()")
    {
        // REQUIRE(frevent.GetName() == "FrameBufferResize");
    }

    SECTION("ToString()")
    {
        // REQUIRE(frevent.ToString() == fmt::format("FrameBufferResizeEvent: {}x{} (\"{}\")", frevent.GetWidth(), frevent.GetHeight(), wcsevent.GetWindow().GetTitle()));
    }

    SECTION("GetCategoryFlags()")
    {
        // REQUIRE(frevent.GetCategoryFlags() == TRAP::Events::EventCategory::Window);
    }

    SECTION("IsInCategory()")
    {
        // REQUIRE(frevent.IsInCategory(TRAP::Events::EventCategory::Window));
    }

    SECTION("fmt specialization")
    {
        // REQUIRE(fmt::format("{}", frevent) == fmt::format("FrameBufferResizeEvent: {}x{} (\"{}\")", frevent.GetWidth(), frevent.GetHeight(), wcsevent.GetWindow().GetTitle()));
    }
}
