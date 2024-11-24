#include <catch2/catch_test_macros.hpp>

#include "Utils/Concurrency/LockFreeQueue.h"

TEST_CASE("TRAP::Utils::LockFreeQueue", "[utils][concurrency][lockfreequeue]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Utils::LockFreeQueue<std::string>>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Utils::LockFreeQueue<std::string>>);
        STATIC_REQUIRE(std::movable<TRAP::Utils::LockFreeQueue<std::string>>);
    }

    SECTION("Constructors")
    {
        TRAP::Utils::LockFreeQueue<std::string> queue{};

        //Copy constructor
        // const TRAP::Utils::LockFreeQueue<std::string> queueCopy(queue); //Deleted

        //Move constructor
        const TRAP::Utils::LockFreeQueue<std::string> queueMove(std::move(queue));
    }

    SECTION("Assignments")
    {
        TRAP::Utils::LockFreeQueue<std::string> queue{};

        //Copy assignment operator
        // const TRAP::Utils::LockFreeQueue<std::string> queueCopy = queue; //Deleted

        //Move assignment operator
        const TRAP::Utils::LockFreeQueue<std::string> queueMove = std::move(queue);
    }

    SECTION("Push()")
    {
        TRAP::Utils::LockFreeQueue<std::string> queue{};

        queue.Push("Hello World!");

        const auto val = queue.TryPop();
        REQUIRE(val);
        REQUIRE(*val == "Hello World!");
    }

    SECTION("TryPop()")
    {
        TRAP::Utils::LockFreeQueue<std::string> queue{};

        REQUIRE_FALSE(queue.TryPop());

        queue.Push("Hello World!");

        const auto val = queue.TryPop();
        REQUIRE(val);
        REQUIRE(*val == "Hello World!");
    }
}
