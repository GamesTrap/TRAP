#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "Utils/Concurrency/ContentionFreeSharedMutex.h"

#include <shared_mutex>

TEST_CASE("TRAP::Utils::ContentionFreeSharedMutex", "[utils][concurrency][contentionfreesharedmutex]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Utils::ContentionFreeSharedMutex<>>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Utils::ContentionFreeSharedMutex<>>);
        STATIC_REQUIRE_FALSE(std::movable<TRAP::Utils::ContentionFreeSharedMutex<>>);
    }

    SECTION("Implicit CTAD")
    {
        TRAP::Utils::ContentionFreeSharedMutex m;
        {
            std::shared_lock lock(m);
            STATIC_REQUIRE(std::same_as<decltype(lock), std::shared_lock<TRAP::Utils::ContentionFreeSharedMutex<>>>);
        }
    }

    // SECTION("Assignment operator")
    // {
    //     TRAP::Utils::ContentionFreeSharedMutex m0;
    //     TRAP::Utils::ContentionFreeSharedMutex m1;

    //     m1 = m0; //Expected error (overload resolution selected deleted operator '=')
    // }

    // SECTION("Copy constructor")
    // {
    //     TRAP::Utils::ContentionFreeSharedMutex m0;
    //     TRAP::Utils::ContentionFreeSharedMutex m1(m0); //Expected error (call to deleted constructor of `TRAP::Utils::ContentionFreeSharedMutex`)
    // }

    constexpr auto waitTime = std::chrono::milliseconds(250);
    constexpr auto tolerance = std::chrono::milliseconds(50);

    SECTION("LockShared")
    {
        TRAP::Utils::ContentionFreeSharedMutex<> mtx;

        auto f = [&mtx, &waitTime, &tolerance]()
        {
            const auto t0 = std::chrono::system_clock::now();
            mtx.lock_shared();
            const auto t1 = std::chrono::system_clock::now();
            mtx.unlock_shared();
            const auto d = t1 - t0 - waitTime;

            if(d >= tolerance)
                throw std::runtime_error("Out of time");
        };

        auto g = [&mtx, &tolerance]()
        {
            const auto t0 = std::chrono::system_clock::now();
            mtx.lock_shared();
            const auto t1 = std::chrono::system_clock::now();
            mtx.unlock_shared();
            const auto d = t1 - t0;

            if(d >= tolerance)
                throw std::runtime_error("Out of time");
        };

        mtx.lock();
        std::vector<std::jthread> v{};
        for(u32 i = 0; i < 5; ++i)
            v.emplace_back(f);
        std::this_thread::sleep_for(waitTime);
        mtx.unlock();
        v.clear();
        mtx.lock_shared();
        for(u32 i = 0; i < 5; ++i)
            v.emplace_back(g);
        const std::jthread q = std::jthread(f);
        std::this_thread::sleep_for(waitTime);
        mtx.unlock_shared();
        v.clear();
    }

    SECTION("Lock")
    {
        TRAP::Utils::ContentionFreeSharedMutex m;

        auto f = [&m, &waitTime, &tolerance]()
        {
            const auto t0 = std::chrono::system_clock::now();
            m.lock();
            const auto t1 = std::chrono::system_clock::now();
            m.unlock();
            const auto d = t1 - t0 - waitTime;

            if(d >= tolerance)
                throw std::runtime_error("Out of time");
        };

        m.lock();
        const std::jthread q = std::jthread(f);
        std::this_thread::sleep_for(waitTime);
        m.unlock();
    }

    SECTION("TryLockShared")
    {
        TRAP::Utils::ContentionFreeSharedMutex m;

        auto f = [&m, &waitTime, &tolerance]()
        {
            const auto t0 = std::chrono::system_clock::now();
            if(m.try_lock_shared())
                throw std::runtime_error("Successfully locked an already locked mutex");
            if(m.try_lock_shared())
                throw std::runtime_error("Successfully locked an already locked mutex");
            if(m.try_lock_shared())
                throw std::runtime_error("Successfully locked an already locked mutex");
            while(!m.try_lock_shared())
                std::this_thread::yield();
            const auto t1 = std::chrono::system_clock::now();
            m.unlock_shared();
            const auto d = t1 - t0 - waitTime;

            if(d >= std::chrono::milliseconds(200))
                throw std::runtime_error("Out of time");
        };

        m.lock();
        std::vector<std::jthread> v{};
        for(u32 i = 0; i < 5; ++i)
            v.emplace_back(f);
        std::this_thread::sleep_for(waitTime);
        m.unlock();
        v.clear();
    }

    SECTION("TryLock")
    {
        TRAP::Utils::ContentionFreeSharedMutex m;

        auto f = [&m, &waitTime, &tolerance]()
        {
            const auto t0 = std::chrono::system_clock::now();
            if(m.try_lock())
                throw std::runtime_error("Successfully locked an already locked mutex");
            if(m.try_lock())
                throw std::runtime_error("Successfully locked an already locked mutex");
            if(m.try_lock())
                throw std::runtime_error("Successfully locked an already locked mutex");
            while(!m.try_lock())
                ;
            const auto t1 = std::chrono::system_clock::now();
            m.unlock();
            const auto d = t1 - t0 - waitTime;

            if(d >= std::chrono::milliseconds(200))
                throw std::runtime_error("Out of time");
        };

        m.lock();
        const std::jthread t = std::jthread(f);
        std::this_thread::sleep_for(waitTime);
        m.unlock();
    }

    // SECTION("ContentionFreeSharedMutex Benchmark")
    // {
    //     BENCHMARK("ContentionFreeSharedMutex")
    //     {
    //         i32 globalValue = 42;
    //         TRAP::Utils::ContentionFreeSharedMutex cfsm{};

    //         auto Benchmark_SharedMutex = [&cfsm, &globalValue]()
    //         {
    //             std::shared_lock<TRAP::Utils::ContentionFreeSharedMutex<>> lock(cfsm);
    //             (void)globalValue;
    //         };

    //         std::vector<std::jthread> threads{};

    //         {
    //             threads.emplace_back(Benchmark_SharedMutex);
    //             threads.clear();
    //         }
    //         {
    //             for(u32 i = 0; i < 2; ++i)
    //                 threads.emplace_back(Benchmark_SharedMutex);
    //             threads.clear();
    //         }
    //         {
    //             for(u32 i = 0; i < 4; ++i)
    //                 threads.emplace_back(Benchmark_SharedMutex);
    //             threads.clear();
    //         }
    //         {
    //             for(u32 i = 0; i < 8; ++i)
    //                 threads.emplace_back(Benchmark_SharedMutex);
    //             threads.clear();
    //         }
    //         {
    //             for(u32 i = 0; i < 32; ++i)
    //                 threads.emplace_back(Benchmark_SharedMutex);
    //             threads.clear();
    //         }
    //     };
    // }
}
