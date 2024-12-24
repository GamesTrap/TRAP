#include <catch2/catch_test_macros.hpp>

#include "Utils/Time/Timer.h"

TEST_CASE("TRAP::Utils::Timer", "[utils][timer]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Utils::Timer>);
        STATIC_REQUIRE(std::copyable<TRAP::Utils::Timer>);
        STATIC_REQUIRE(std::movable<TRAP::Utils::Timer>);
    }

    SECTION("Constructors")
    {
        const TRAP::Utils::Timer timer{};

        //Copy constructor
        [[maybe_unused]] const TRAP::Utils::Timer timerCopy(timer);

        //Move constructor
        TRAP::Utils::Timer timerMoveSource{};
        const TRAP::Utils::Timer timerMoveTarget(std::move(timerMoveSource));
    }

    SECTION("Assignments")
    {
        //Copy assignment operator
        const TRAP::Utils::Timer timer{};
        [[maybe_unused]] const TRAP::Utils::Timer timerCopy = timer;

        //Move assignment operator
        TRAP::Utils::Timer timerMoveSource{};
        const TRAP::Utils::Timer timerMoveTarget = std::move(timerMoveSource);
    }

    SECTION("Reset()")
    {
        TRAP::Utils::Timer timer{};

        while(timer.Elapsed() < 0.250f);

        timer.Reset();

        while(timer.Elapsed() < 0.1f);

        const f32 elapsed = timer.Elapsed();

        REQUIRE((elapsed > 0.1f && elapsed < 0.250f));
    }

    SECTION("Elapsed()")
    {
        TRAP::Utils::Timer timer{};

        while(timer.Elapsed() < 0.01f);

        REQUIRE(timer.Elapsed() > 0.0f);
    }

    SECTION("ElapsedMilliseconds()")
    {
        TRAP::Utils::Timer timer{};

        while(timer.Elapsed() < 0.01f);

        REQUIRE(timer.ElapsedMilliseconds() > 0.0f);
    }
}
