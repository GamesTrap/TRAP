#include <catch2/catch_test_macros.hpp>

#include <future>
#include <thread>

#include "Core/Types.h"
#include "Utils/Concurrency/Futures/WaitForAll.h"

TEST_CASE("TRAP::Utils::WaitForAll", "[utils][concurrency][futures][waitforall]")
{
    SECTION("std::future")
    {
        std::promise<i32> p1{};
        std::promise<i32> p2{};

        std::future<i32> f1 = p1.get_future();
        std::future<i32> f2 = p2.get_future();

        const std::jthread setValues([](std::promise<i32>& p1_, std::promise<i32>& p2_)
        {
            p1_.set_value(42);
            p2_.set_value(43);
        }, std::ref(p1), std::ref(p2));

        TRAP::Utils::WaitForAll(f1, f2);

        REQUIRE((f1.valid() && f1.get() == 42));
        REQUIRE((f2.valid() && f2.get() == 43));
    }

    SECTION("std::shared_future")
    {
        std::promise<i32> p1{};
        std::promise<i32> p2{};

        std::shared_future<i32> f1 = p1.get_future().share();
        std::shared_future<i32> f2 = p2.get_future().share();

        const std::jthread setValues([](std::promise<i32>& p1_, std::promise<i32>& p2_)
        {
            p1_.set_value(42);
            p2_.set_value(43);
        }, std::ref(p1), std::ref(p2));

        TRAP::Utils::WaitForAll(f1, f2);

        REQUIRE((f1.valid() && f1.get() == 42));
        REQUIRE((f2.valid() && f2.get() == 43));
    }

    SECTION("std::future & std::shared_future")
    {
        std::promise<i32> p1{};
        std::promise<i32> p2{};

        std::future<i32> f1 = p1.get_future();
        std::shared_future<i32> f2 = p2.get_future().share();

        const std::jthread setValues([](std::promise<i32>& p1_, std::promise<i32>& p2_)
        {
            p1_.set_value(42);
            p2_.set_value(43);
        }, std::ref(p1), std::ref(p2));

        TRAP::Utils::WaitForAll(f1, f2);

        REQUIRE((f1.valid() && f1.get() == 42));
        REQUIRE((f2.valid() && f2.get() == 43));
    }

    SECTION("Iterator of std::futures")
    {
        std::promise<i32> p1{};
        std::promise<i32> p2{};

        std::vector<std::future<i32>> futures{};
        futures.emplace_back(p1.get_future());
        futures.emplace_back(p2.get_future());

        const std::jthread setValues([](std::promise<i32>& p1_, std::promise<i32>& p2_)
        {
            p1_.set_value(42);
            p2_.set_value(43);
        }, std::ref(p1), std::ref(p2));

        TRAP::Utils::WaitForAll(futures.begin(), futures.end());

        REQUIRE((futures.at(0).valid() && futures.at(0).get() == 42));
        REQUIRE((futures.at(1).valid() && futures.at(1).get() == 43));
    }

    SECTION("Iterator of std::shared_futures")
    {
        std::promise<i32> p1{};
        std::promise<i32> p2{};

        std::vector<std::shared_future<i32>> futures{};
        futures.emplace_back(p1.get_future());
        futures.emplace_back(p2.get_future());

        const std::jthread setValues([](std::promise<i32>& p1_, std::promise<i32>& p2_)
        {
            p1_.set_value(42);
            p2_.set_value(43);
        }, std::ref(p1), std::ref(p2));

        TRAP::Utils::WaitForAll(futures.begin(), futures.end());

        REQUIRE((futures.at(0).valid() && futures.at(0).get() == 42));
        REQUIRE((futures.at(1).valid() && futures.at(1).get() == 43));
    }

    SECTION("Range of std::futures")
    {
        std::promise<i32> p1{};
        std::promise<i32> p2{};

        std::vector<std::future<i32>> futures{};
        futures.emplace_back(p1.get_future());
        futures.emplace_back(p2.get_future());

        const std::jthread setValues([](std::promise<i32>& p1_, std::promise<i32>& p2_)
        {
            p1_.set_value(42);
            p2_.set_value(43);
        }, std::ref(p1), std::ref(p2));

        TRAP::Utils::WaitForAll(futures);

        REQUIRE((futures.at(0).valid() && futures.at(0).get() == 42));
        REQUIRE((futures.at(1).valid() && futures.at(1).get() == 43));
    }

    SECTION("Range of std::shared_futures")
    {
        std::promise<i32> p1{};
        std::promise<i32> p2{};

        std::vector<std::shared_future<i32>> futures{};
        futures.emplace_back(p1.get_future());
        futures.emplace_back(p2.get_future());

        const std::jthread setValues([](std::promise<i32>& p1_, std::promise<i32>& p2_)
        {
            p1_.set_value(42);
            p2_.set_value(43);
        }, std::ref(p1), std::ref(p2));

        TRAP::Utils::WaitForAll(futures);

        REQUIRE((futures.at(0).valid() && futures.at(0).get() == 42));
        REQUIRE((futures.at(1).valid() && futures.at(1).get() == 43));
    }
}
