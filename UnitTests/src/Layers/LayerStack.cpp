#include <catch2/catch_test_macros.hpp>

#include <ranges>

#include "Layers/LayerStack.h"

TEST_CASE("TRAP::LayerStack", "[layerstack]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::LayerStack>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::LayerStack>);
        STATIC_REQUIRE(std::movable<TRAP::LayerStack>);
    }

    SECTION("Constructors")
    {
        static constexpr TRAP::LayerStack layerStack{};

        //Copy constructor
        // static constexpr TRAP::LayerStack layerStack1{layerStack}; //Deleted

        //Move constructor
        TRAP::LayerStack moveSourceLayerStack{};
        const TRAP::LayerStack moveTargetLayerStack(std::move(moveSourceLayerStack));
    }

    SECTION("Assignments")
    {
        TRAP::LayerStack layerStack{};

        //Copy assignment operator
        // const TRAP::LayerStack copyLayerStack = layerStack; //Deleted

        //Move assignment operator
        const TRAP::LayerStack moveLayerStack = std::move(layerStack);
    }

    SECTION("Shutdown()")
    {
        TRAP::LayerStack layerStack{};

        REQUIRE(std::ranges::size(layerStack) == 0u);

        layerStack.PushLayer(std::make_unique<TRAP::Layer>());

        REQUIRE(std::ranges::size(layerStack) == 1u);

        layerStack.Shutdown();

        REQUIRE(std::ranges::size(layerStack) == 0u);
    }

    SECTION("PushLayer() - Valid")
    {
        TRAP::LayerStack layerStack{};

        REQUIRE(std::ranges::size(layerStack) == 0u);

        layerStack.PushLayer(std::make_unique<TRAP::Layer>());

        REQUIRE(std::ranges::size(layerStack) == 1u);
    }

    SECTION("PushLayer() - Invalid")
    {
        TRAP::LayerStack layerStack{};

        REQUIRE(std::ranges::size(layerStack) == 0u);

        layerStack.PushLayer(nullptr);

        REQUIRE(std::ranges::size(layerStack) == 0u);
    }

    SECTION("PushOverlay() - Valid")
    {
        TRAP::LayerStack layerStack{};

        REQUIRE(std::ranges::size(layerStack) == 0u);

        layerStack.PushOverlay(std::make_unique<TRAP::Layer>("Overlay"));
        layerStack.PushLayer(std::make_unique<TRAP::Layer>("Layer"));

        REQUIRE(std::ranges::size(layerStack) == 2u);

        REQUIRE((*layerStack.begin())->GetName() == "Layer");
        REQUIRE((*++layerStack.begin())->GetName() == "Overlay");
    }

    SECTION("PushOverlay() - Invalid")
    {
        TRAP::LayerStack layerStack{};

        REQUIRE(std::ranges::size(layerStack) == 0u);

        layerStack.PushOverlay(nullptr);

        REQUIRE(std::ranges::size(layerStack) == 0u);
    }

    SECTION("PopLayer() - Valid")
    {
        TRAP::LayerStack layerStack{};

        std::unique_ptr<TRAP::Layer> layer = std::make_unique<TRAP::Layer>();
        TRAP::Layer* layerPtr = layer.get();

        REQUIRE(std::ranges::size(layerStack) == 0u);

        layerStack.PushLayer(std::move(layer));

        REQUIRE(std::ranges::size(layerStack) == 1u);

        layerStack.PopLayer(*layerPtr);

        REQUIRE(std::ranges::size(layerStack) == 0u);
    }

    SECTION("PopLayer() - Invalid")
    {
        TRAP::LayerStack layerStack{};

        std::unique_ptr<TRAP::Layer> layer = std::make_unique<TRAP::Layer>();
        TRAP::Layer* layerPtr = layer.get();

        REQUIRE(std::ranges::size(layerStack) == 0u);

        layerStack.PushLayer(std::move(layer));

        REQUIRE(std::ranges::size(layerStack) == 1u);

        layerStack.PopOverlay(*layerPtr);

        REQUIRE(std::ranges::size(layerStack) == 1u);
    }

    SECTION("PopOverlay() - Valid")
    {
        TRAP::LayerStack layerStack{};

        std::unique_ptr<TRAP::Layer> layer = std::make_unique<TRAP::Layer>();
        TRAP::Layer* layerPtr = layer.get();

        REQUIRE(std::ranges::size(layerStack) == 0u);

        layerStack.PushOverlay(std::move(layer));

        REQUIRE(std::ranges::size(layerStack) == 1u);

        layerStack.PopOverlay(*layerPtr);

        REQUIRE(std::ranges::size(layerStack) == 0u);
    }

    SECTION("PopOverlay() - Invalid")
    {
        TRAP::LayerStack layerStack{};

        std::unique_ptr<TRAP::Layer> layer = std::make_unique<TRAP::Layer>();
        TRAP::Layer* layerPtr = layer.get();

        REQUIRE(std::ranges::size(layerStack) == 0u);

        layerStack.PushOverlay(std::move(layer));

        REQUIRE(std::ranges::size(layerStack) == 1u);

        layerStack.PopLayer(*layerPtr);

        REQUIRE(std::ranges::size(layerStack) == 1u);
    }


    SECTION("Iterators")
    {
        TRAP::LayerStack layerStack{};

        REQUIRE(std::ranges::size(layerStack) == 0u);

        layerStack.PushOverlay(std::make_unique<TRAP::Layer>("Overlay"));
        layerStack.PushLayer(std::make_unique<TRAP::Layer>("Layer"));

        for(auto it = layerStack.begin(); it != layerStack.end(); ++it)
        {
            REQUIRE(*it);
        }
        for(auto it = layerStack.rbegin(); it != layerStack.rend(); ++it)
        {
            REQUIRE(*it);
        }
    }
}
