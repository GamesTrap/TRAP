#include <catch2/catch_test_macros.hpp>

#include "Layers/Layer.h"
#include "Events/FileEvent.h"

TEST_CASE("TRAP::Layer", "[layer]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE_FALSE(std::is_final_v<TRAP::Layer>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Layer>);
        STATIC_REQUIRE(std::movable<TRAP::Layer>);
    }

    SECTION("Constructor")
    {
        //Default constructor
        const TRAP::Layer layer{};

        //Copy constructor
        // static constexpr TRAP::Layer copy(layer); //Deleted

        //Move constructor
        TRAP::Layer moveSrcLayer{};
        const TRAP::Layer moveTargetLayer(std::move(moveSrcLayer));
    }

    SECTION("Assignment")
    {
        //Copy assignment
        // static constexpr TRAP::Layer copySrc{};
        // static constexpr TRAP::Layer copy = layer; //Deleted

        //Move assignment
        TRAP::Layer moveSrcLayer{};
        const TRAP::Layer moveTargetLayer = std::move(moveSrcLayer);
    }

    SECTION("OnAttach()")
    {
        TRAP::Layer layer{};
        layer.OnAttach();
    }

    SECTION("OnDetach()")
    {
        TRAP::Layer layer{};
        layer.OnDetach();
    }

    SECTION("OnUpdate()")
    {
        TRAP::Layer layer{};
        layer.OnUpdate(TRAP::Utils::TimeStep(0.0f));
    }

    SECTION("OnTick()")
    {
        TRAP::Layer layer{};
        layer.OnTick(TRAP::Utils::TimeStep(0.0f));
    }

    SECTION("OnImGuiRender()")
    {
        TRAP::Layer layer{};
        layer.OnImGuiRender();
    }

    SECTION("OnEvent()")
    {
        TRAP::Events::FileSystemChangeEvent fsEvent(TRAP::FileSystem::FileSystemStatus::Created, "Somefile.txt");

        TRAP::Layer layer{};
        layer.OnEvent(fsEvent);
    }

    SECTION("GetName()")
    {
        TRAP::Layer layer{};
        REQUIRE(layer.GetName() == "Layer");

        TRAP::Layer layer1{"TestLayer"};
        REQUIRE(layer1.GetName() == "TestLayer");
    }
}
