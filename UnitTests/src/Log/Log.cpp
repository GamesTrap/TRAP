#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/benchmark/catch_constructor.hpp>

#include <barrier>

#include "Log/Log.h"

namespace
{
     constexpr std::string_view LogFilePath = "unused/path";
     constexpr TRAP::LogLevel LogLevelAll = TRAP::LogLevel::Trace | TRAP::LogLevel::Debug |
                                            TRAP::LogLevel::Info | TRAP::LogLevel::Warn | TRAP::LogLevel::Error |
                                            TRAP::LogLevel::Critical;
}

// TEST_CASE("TRAP::Log (Concurrency)", "[log][concurrency]")
// {
//     BENCHMARK_ADVANCED("Single threaded")(Catch::Benchmark::Chronometer meter)
//     {
//         std::vector<std::unique_ptr<TRAP::Log>> storage{};
//         for(i32 i = 0; i < meter.runs(); ++i)
//         {
//             storage.push_back(std::make_unique<TRAP::Log>("unused/path"));
//             storage.back()->SetImportance(static_cast<TRAP::LogLevel>(0));
//         }

//         meter.measure([&storage](const i32 i)
//         {
//             auto& logger = *storage[i];

//             logger.Trace("Test Trace");
//             logger.Info("Test Info");
//             logger.Warn("Test Warning");
//             logger.Error("Test Error");
//             logger.Critical("Test Critical error");
//         });
//     };

//     struct LoggerData
//     {
//         std::unique_ptr<TRAP::Log> Logger = nullptr;
//         std::barrier<> Barrier = std::barrier(std::thread::hardware_concurrency() + 1);
//         std::vector<std::jthread> Threads = std::vector<std::jthread>(std::thread::hardware_concurrency());
//     };

//     BENCHMARK_ADVANCED("Multi threaded (high contention)")(Catch::Benchmark::Chronometer meter)
//     {
//         const auto Work = [](TRAP::Log& logger, std::barrier<>& barrier)
//         {
//             barrier.arrive_and_wait();

//             for(i32 j = 0; j < 100; ++j)
//                 logger.Error(std::this_thread::get_id(), " Test ", j);
//         };

//         std::vector<LoggerData> storage(meter.runs());
//         for(auto& loggerData : storage)
//         {
//             loggerData.Logger = std::make_unique<TRAP::Log>("unused/path");
//             loggerData.Logger->SetImportance(static_cast<TRAP::LogLevel>(0));
//             for(auto& thread : loggerData.Threads)
//                 thread = std::jthread(Work, std::ref(*loggerData.Logger), std::ref(loggerData.Barrier));
//         }

//         meter.measure([&storage](const i32 i)
//         {
//             [[maybe_unused]] const auto _ = storage[i].Barrier.arrive();

//             for(auto& thread : storage[i].Threads)
//             {
//                 if(thread.joinable())
//                     thread.join();
//             }
//         });

//         for(auto& loggerData : storage)
//             REQUIRE(loggerData.Logger->GetBufferAndFlush().size() == (loggerData.Threads.size() * 100u));
//     };
// }

TEST_CASE("TRAP::Log", "[log]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Log>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Log>);
        STATIC_REQUIRE_FALSE(std::movable<TRAP::Log>);
    }

    SECTION("TRAP::Log::GetFilePath()")
    {
        TRAP::Log log(LogFilePath);

        REQUIRE(log.GetFilePath() == LogFilePath);
    }

    SECTION("TRAP::Log::SetFilePath()")
    {
        TRAP::Log log("some/path");

        log.SetFilePath(LogFilePath);
        REQUIRE(log.GetFilePath() == LogFilePath);

        log.SetFilePath("");
        REQUIRE(log.GetFilePath() == LogFilePath);
    }

    SECTION("TRAP::Log::GetImportanceLevel()")
    {
        TRAP::Log log(LogFilePath);

        REQUIRE_FALSE(log.GetImportance() == static_cast<TRAP::LogLevel>(0u));
    }

    SECTION("TRAP::Log::SetImportanceLevel()")
    {
        TRAP::Log log(LogFilePath);

        log.SetImportance(TRAP::LogLevel::Debug);
        REQUIRE(log.GetImportance() == TRAP::LogLevel::Debug);
    }

    SECTION("TRAP::Log::Trace()")
    {
        TRAP::Log log(LogFilePath);
        log.SetImportance(LogLevelAll);

        log.Trace("Test");

        const auto loggedMsgs = log.GetBufferAndFlush();
        REQUIRE_FALSE(loggedMsgs.empty());
        REQUIRE(loggedMsgs.front().Level == TRAP::LogLevel::Trace);
        REQUIRE_FALSE(loggedMsgs.front().Message.empty());
    }

    SECTION("TRAP::Log::Debug()")
    {
        TRAP::Log log(LogFilePath);
        log.SetImportance(LogLevelAll);

        log.Debug("Test");

        const auto loggedMsgs = log.GetBufferAndFlush();
        REQUIRE_FALSE(loggedMsgs.empty());
        REQUIRE(loggedMsgs.front().Level == TRAP::LogLevel::Debug);
        REQUIRE_FALSE(loggedMsgs.front().Message.empty());
    }

    SECTION("TRAP::Log::Info()")
    {
        TRAP::Log log(LogFilePath);
        log.SetImportance(LogLevelAll);

        log.Info("Test");

        const auto loggedMsgs = log.GetBufferAndFlush();
        REQUIRE_FALSE(loggedMsgs.empty());
        REQUIRE(loggedMsgs.front().Level == TRAP::LogLevel::Info);
        REQUIRE_FALSE(loggedMsgs.front().Message.empty());
    }

    SECTION("TRAP::Log::Warn()")
    {
        TRAP::Log log(LogFilePath);
        log.SetImportance(LogLevelAll);

        log.Warn("Test");

        const auto loggedMsgs = log.GetBufferAndFlush();
        REQUIRE_FALSE(loggedMsgs.empty());
        REQUIRE(loggedMsgs.front().Level == TRAP::LogLevel::Warn);
        REQUIRE_FALSE(loggedMsgs.front().Message.empty());
    }

    SECTION("TRAP::Log::Error()")
    {
        TRAP::Log log(LogFilePath);
        log.SetImportance(LogLevelAll);

        log.Error("Test");

        const auto loggedMsgs = log.GetBufferAndFlush();
        REQUIRE_FALSE(loggedMsgs.empty());
        REQUIRE(loggedMsgs.front().Level == TRAP::LogLevel::Error);
        REQUIRE_FALSE(loggedMsgs.front().Message.empty());
    }

    SECTION("TRAP::Log::Critical()")
    {
        TRAP::Log log(LogFilePath);
        log.SetImportance(LogLevelAll);

        log.Critical("Test");

        const auto loggedMsgs = log.GetBufferAndFlush();
        REQUIRE_FALSE(loggedMsgs.empty());
        REQUIRE(loggedMsgs.front().Level == TRAP::LogLevel::Critical);
        REQUIRE_FALSE(loggedMsgs.front().Message.empty());
    }

    SECTION("TRAP::Log::LogMessage()")
    {
        TRAP::Log log(LogFilePath);
        log.SetImportance(LogLevelAll);

        log.LogMessage(TRAP::LogLevel::Trace, "Test");
        log.LogMessage(TRAP::LogLevel::Debug, "Test");
        log.LogMessage(TRAP::LogLevel::Info, "Test");
        log.LogMessage(TRAP::LogLevel::Warn, "Test");
        log.LogMessage(TRAP::LogLevel::Error, "Test");
        log.LogMessage(TRAP::LogLevel::Critical, "Test");

        const auto loggedMsgs = log.GetBufferAndFlush();
        REQUIRE(loggedMsgs.size() == 6u);
        REQUIRE(loggedMsgs[0].Level == TRAP::LogLevel::Trace);
        REQUIRE(loggedMsgs[1].Level == TRAP::LogLevel::Debug);
        REQUIRE(loggedMsgs[2].Level == TRAP::LogLevel::Info);
        REQUIRE(loggedMsgs[3].Level == TRAP::LogLevel::Warn);
        REQUIRE(loggedMsgs[4].Level == TRAP::LogLevel::Error);
        REQUIRE(loggedMsgs[5].Level == TRAP::LogLevel::Critical);
        for(const auto& msg : loggedMsgs)
            REQUIRE_FALSE(msg.Message.empty());

        REQUIRE(std::ranges::is_sorted(loggedMsgs, std::less{})); //Retrieved messages must be ordered by log time
    }

    SECTION("TRAP::Log::Clear()")
    {
        TRAP::Log log(LogFilePath);
        log.SetImportance(LogLevelAll);

        log.Trace("Test");

        log.Clear();
        REQUIRE(log.GetBufferAndFlush().empty());
    }

    SECTION("TRAP::Log::GetBufferAndFlush()")
    {
        TRAP::Log log(LogFilePath);
        log.SetImportance(LogLevelAll);

        log.Trace("Test");

        REQUIRE_FALSE(log.GetBufferAndFlush().empty());
        REQUIRE(log.GetBufferAndFlush().empty());
    }
}
