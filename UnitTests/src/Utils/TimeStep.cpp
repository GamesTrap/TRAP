#include <catch2/catch_test_macros.hpp>

#include "Utils/Time/TimeStep.h"

TEST_CASE("TRAP::Utils::TimeStep", "[utils][timestep]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Utils::TimeStep>);
        STATIC_REQUIRE(std::is_copy_constructible_v<TRAP::Utils::TimeStep>);
        STATIC_REQUIRE(std::is_copy_assignable_v<TRAP::Utils::TimeStep>);
        STATIC_REQUIRE_FALSE(std::movable<TRAP::Utils::TimeStep>);
    }

    SECTION("Constructors")
    {
        static constexpr TRAP::Utils::TimeStep timeStep{0.0f};

        //Copy constructor
        static constexpr TRAP::Utils::TimeStep timeStepCopy(timeStep);

        //Move constructor
        // TRAP::Utils::TimeStep timeStepMoveSource{0.0f};
        // const TRAP::Utils::TimeStep timeStepMoveTarget(std::move(timeStepMoveSource)); //Deleted
    }

    SECTION("Assignments")
    {
        //Copy assignment operator
        static constexpr TRAP::Utils::TimeStep timeStep{0.0f};
        static constexpr TRAP::Utils::TimeStep timeStepCopy = timeStep;

        //Move assignment operator
        // TRAP::Utils::TimeStep timeStepMoveSource{0.0f};
        // const TRAP::Utils::TimeStep timeStepMoveTarget = std::move(timeStepMoveSource); //Deleted
    }

    SECTION("operator f32()")
    {
        static constexpr TRAP::Utils::TimeStep timeStep{10.0f};

        STATIC_REQUIRE(f32(timeStep) == 10.0f);
    }

    SECTION("GetSeconds()")
    {
        static constexpr TRAP::Utils::TimeStep timeStep{10.0f};

        STATIC_REQUIRE(timeStep.GetSeconds() == 10.0f);
    }

    SECTION("GetMilliseconds()")
    {
        static constexpr TRAP::Utils::TimeStep timeStep{10.0f};

        STATIC_REQUIRE(timeStep.GetMilliseconds() == 10000.0f);
    }
}
