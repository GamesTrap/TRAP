#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Events/HotReloadEvent.h"

TEST_CASE("TRAP::Events::TextureReloadEvent", "[events][texturereloadevent]")
{
    const TRAP::Events::TextureReloadEvent tevent = TRAP::Events::TextureReloadEvent{nullptr};

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Events::TextureReloadEvent>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Events::TextureReloadEvent>);
        STATIC_REQUIRE(std::movable<TRAP::Events::TextureReloadEvent>);
        STATIC_REQUIRE(std::derived_from<TRAP::Events::TextureReloadEvent, TRAP::Events::Event>);
    }

    SECTION("Constructor")
    {
        // const TRAP::Events::TextureReloadEvent teventCopyConstructor(tevent); //Deleted

        TRAP::Events::TextureReloadEvent tevent1 = TRAP::Events::TextureReloadEvent{nullptr};
        const TRAP::Events::TextureReloadEvent tevent1MoveConstructor = TRAP::Events::TextureReloadEvent(std::move(tevent1));
    }

    SECTION("Assignment")
    {
        TRAP::Events::TextureReloadEvent teventAssignment = TRAP::Events::TextureReloadEvent{nullptr};

        // const TRAP::Events::TextureReloadEvent teventCopyAssignment = tevent; //Deleted
        const TRAP::Events::TextureReloadEvent teventMoveAssignment = std::move(teventAssignment);
    }

    SECTION("GetTexture()")
    {
        //TODO Add test with a valid Texture object.

        const TRAP::Events::TextureReloadEvent tevent1 = TRAP::Events::TextureReloadEvent{nullptr};

        REQUIRE_FALSE(tevent1.GetTexture());
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::TextureReloadEvent::GetStaticType() == TRAP::Events::EventType::TextureReload);
    }

    SECTION("GetEventType()")
    {
        REQUIRE(tevent.GetEventType() == TRAP::Events::EventType::TextureReload);
    }

    SECTION("GetName()")
    {
        REQUIRE(tevent.GetName() == "TextureReload");
    }

    SECTION("ToString()")
    {
        REQUIRE(tevent.ToString() == fmt::format("TextureReloadEvent: nullptr"));

        //TODO Add test with a valid Texture object.
    }

    SECTION("GetCategoryFlags()")
    {
        REQUIRE(tevent.GetCategoryFlags() == TRAP::Events::EventCategory::HotReload);
    }

    SECTION("IsInCategory()")
    {
        REQUIRE(tevent.IsInCategory(TRAP::Events::EventCategory::HotReload));
    }

    SECTION("fmt specialization")
    {
        REQUIRE(fmt::format("{}", tevent) == fmt::format("TextureReloadEvent: nullptr"));

        //TODO Add test with a valid Texture object.
    }
}

TEST_CASE("TRAP::Events::ShaderReloadEvent", "[events][shaderreloadevent]")
{
    const TRAP::Events::ShaderReloadEvent sevent = TRAP::Events::ShaderReloadEvent{nullptr};

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Events::ShaderReloadEvent>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Events::ShaderReloadEvent>);
        STATIC_REQUIRE(std::movable<TRAP::Events::ShaderReloadEvent>);
        STATIC_REQUIRE(std::derived_from<TRAP::Events::ShaderReloadEvent, TRAP::Events::Event>);
    }

    SECTION("Constructor")
    {
        // const TRAP::Events::ShaderReloadEvent seventCopyConstructor(sevent); //Deleted

        TRAP::Events::ShaderReloadEvent sevent1 = TRAP::Events::ShaderReloadEvent{nullptr};
        const TRAP::Events::ShaderReloadEvent sevent1MoveConstructor = TRAP::Events::ShaderReloadEvent(std::move(sevent1));
    }

    SECTION("Assignment")
    {
        TRAP::Events::ShaderReloadEvent seventAssignment = TRAP::Events::ShaderReloadEvent{nullptr};

        // const TRAP::Events::ShaderReloadEvent seventCopyAssignment = sevent; //Deleted
        const TRAP::Events::ShaderReloadEvent seventMoveAssignment = std::move(seventAssignment);
    }

    SECTION("GetShader()")
    {
        //TODO Add test with a valid Shader object.

        const TRAP::Events::ShaderReloadEvent sevent1 = TRAP::Events::ShaderReloadEvent{nullptr};

        REQUIRE_FALSE(sevent1.GetShader());
    }

    SECTION("GetStaticType()")
    {
        STATIC_REQUIRE(TRAP::Events::ShaderReloadEvent::GetStaticType() == TRAP::Events::EventType::ShaderReload);
    }

    SECTION("GetEventType()")
    {
        REQUIRE(sevent.GetEventType() == TRAP::Events::EventType::ShaderReload);
    }

    SECTION("GetName()")
    {
        REQUIRE(sevent.GetName() == "ShaderReload");
    }

    SECTION("ToString()")
    {
        REQUIRE(sevent.ToString() == fmt::format("ShaderReloadEvent: nullptr"));

        //TODO Add test with a valid Shader object.
    }

    SECTION("GetCategoryFlags()")
    {
        REQUIRE(sevent.GetCategoryFlags() == TRAP::Events::EventCategory::HotReload);
    }

    SECTION("IsInCategory()")
    {
        REQUIRE(sevent.IsInCategory(TRAP::Events::EventCategory::HotReload));
    }

    SECTION("fmt specialization")
    {
        REQUIRE(fmt::format("{}", sevent) == fmt::format("ShaderReloadEvent: nullptr"));

        //TODO Add test with a valid Shader object.
    }
}
