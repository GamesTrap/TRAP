#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <unordered_set>
#include <array>

#include "Core/Types.h"
#include "Utils/Expected.h"

namespace
{
    struct TakesInitAndVariadic
    {
        std::vector<i32> v;
        std::tuple<i32, i32> t;
        template<typename... Args>
        TakesInitAndVariadic(std::initializer_list<i32> l, Args&&... args)
            : v(l), t(std::forward<Args>(args)...)
        {}
    };

    struct MoveDetector
    {
        constexpr MoveDetector() = default;
        constexpr ~MoveDetector() = default;
        constexpr MoveDetector(MoveDetector&& rhs)
        {
            rhs.BeenMoved = true;
        }
        constexpr MoveDetector(const MoveDetector&) = default;
        constexpr MoveDetector& operator=(const MoveDetector&) = default;
        constexpr MoveDetector& operator=(MoveDetector&&) = default;

        bool BeenMoved = false;
    };

    struct NoThrow
    {
        constexpr NoThrow(std::string i)
            : i(i)
        {}

        std::string i;
    };

    struct CanThrowMove
    {
        constexpr CanThrowMove(std::string i)
            : i(i)
        {}
        constexpr CanThrowMove() = default;
        constexpr ~CanThrowMove() = default;
        constexpr CanThrowMove(const CanThrowMove&) = default;
        constexpr CanThrowMove(CanThrowMove&& other) noexcept(false)
            : i(other.i)
        {}
        constexpr CanThrowMove& operator=(CanThrowMove&&) = default;
        constexpr CanThrowMove& operator=(const CanThrowMove&) = default;

        std::string i;
    };

    bool ShouldThrow = false;

    #if defined(__EXCEPTIONS) || defined(_HAS_EXCEPTIONS) || defined(__cpp_exceptions)
    struct WillThrowMove
    {
        constexpr WillThrowMove(std::string i)
            : i(i)
        {}
        constexpr ~WillThrowMove() = default;
        constexpr WillThrowMove(const WillThrowMove&) = default;
        constexpr WillThrowMove(WillThrowMove&& other)
            : i(other.i)
        {
            if(ShouldThrow)
                throw std::exception();
        }
        constexpr WillThrowMove& operator=(WillThrowMove&&) = default;
        constexpr WillThrowMove& operator=(const WillThrowMove&) = default;

        std::string i;
    };
    #endif

    static_assert(std::is_swappable_v<NoThrow>);

    template<typename T1, typename T2>
    void SwapTest()
    {
        const std::string s1 = "abcdefghijklmnopqrstuvwxyz";
        const std::string s2 = "zyxwvutsrqponmlkjihgfedcba";

        TRAP::Expected<T1, T2> a{s1};
        TRAP::Expected<T1, T2> b{s2};
        std::swap(a, b);
        REQUIRE(a->i == s2);
        REQUIRE(b->i == s1);

        a = s1;
        b = TRAP::Unexpected<T2>(s2);
        std::swap(a, b);
        REQUIRE(a.Error().i == s2);
        REQUIRE(b->i == s1);

        a = TRAP::Unexpected<T2>(s1);
        b = s2;
        std::swap(a, b);
        REQUIRE(a->i == s2);
        REQUIRE(b.Error().i == s1);

        a = TRAP::Unexpected<T2>(s1);
        b = TRAP::Unexpected<T2>(s2);
        std::swap(a, b);
        REQUIRE(a.Error().i == s2);
        REQUIRE(b.Error().i == s1);

        a = s1;
        b = s2;
        a.Swap(b);
        REQUIRE(a->i == s2);
        REQUIRE(b->i == s1);

        a = s1;
        b = TRAP::Unexpected<T2>(s2);
        a.Swap(b);
        REQUIRE(a.Error().i == s2);
        REQUIRE(b->i == s1);

        a = TRAP::Unexpected<T2>(s1);
        b = s2;
        a.Swap(b);
        REQUIRE(a->i == s2);
        REQUIRE(b.Error().i == s1);

        a = TRAP::Unexpected<T2>(s1);
        b = TRAP::Unexpected<T2>(s2);
        a.Swap(b);
        REQUIRE(a.Error().i == s2);
        REQUIRE(b.Error().i == s1);
    }
}

TEST_CASE("TRAP::Expected", "[utils][expected]")
{
    // SECTION("Assertions")
    // {
    //     static constexpr TRAP::Expected<i32, i32> o1 = 42;
    //     REQUIRE_THROWS_WITH(o1.Error(), "Assertion failure");

    //     static constexpr TRAP::Expected<i32, i32> o2{TRAP::Unexpect, 0};
    //     REQUIRE_THROWS_WITH(*o2, "Assertion failure");

    //     struct Foo
    //     {
    //         i32 Bar;
    //     };
    //     static constexpr TRAP::Expected<Foo, i32> o3{TRAP::Unexpect, 0};
    //     REQUIRE_THROWS_WITH(o3->Bar, "Assertion failure");
    // }

    SECTION("Assignment")
    {
        TRAP::Expected<i32, i32> e1 = 42;
        static constexpr TRAP::Expected<i32, i32> e2 = 17;
        static constexpr TRAP::Expected<i32, i32> e3 = 21;
        TRAP::Expected<i32, i32> e4 = TRAP::MakeUnexpected(42);
        static constexpr TRAP::Expected<i32, i32> e5 = TRAP::MakeUnexpected(17);
        static constexpr TRAP::Expected<i32, i32> e6 = TRAP::MakeUnexpected(21);

        e1 = e2;
        REQUIRE(e1);
        REQUIRE(*e1 == 17);
        STATIC_REQUIRE(e2);
        STATIC_REQUIRE(*e2 == 17);

        e1 = std::move(e2);
        REQUIRE(e1);
        REQUIRE(*e1 == 17);
        REQUIRE(e2);
        REQUIRE(*e2 == 17);

        e1 = 42;
        REQUIRE(e1);
        REQUIRE(*e1 == 42);

        static constexpr auto unexpected = TRAP::MakeUnexpected(12);
        e1 = unexpected;
        REQUIRE(!e1);
        REQUIRE(e1.Error() == 12);

        e1 = TRAP::MakeUnexpected(42);
        REQUIRE(!e1);
        REQUIRE(e1.Error() == 42);

        e1 = e3;
        REQUIRE(e1);
        REQUIRE(*e1 == 21);

        e4 = e5;
        REQUIRE(!e4);
        REQUIRE(e4.Error() == 17);

        e4 = std::move(e6);
        REQUIRE(!e4);
        REQUIRE(e4.Error() == 21);

        e4 = e1;
        REQUIRE(e4);
        REQUIRE(*e4 == 21);
    }

    SECTION("Assignment deletion")
    {
        struct HasAll
        {
            constexpr HasAll() = default;
            constexpr ~HasAll() = default;
            constexpr HasAll(const HasAll&) = default;
            constexpr HasAll(HasAll&&) noexcept = default;
            constexpr HasAll& operator=(const HasAll&) = default;
            constexpr HasAll& operator=(HasAll&&) = default;
        };

        TRAP::Expected<HasAll, HasAll> e1 = {};
        TRAP::Expected<HasAll, HasAll> e2 = {};
        e1 = e2;

        struct ExceptMove
        {
            constexpr ExceptMove() = default;
            constexpr ~ExceptMove() = default;
            constexpr ExceptMove(const ExceptMove&) = default;
            constexpr ExceptMove([[maybe_unused]] ExceptMove&& _) noexcept(false)
            {}
            constexpr ExceptMove& operator=(const ExceptMove&) = default;
            constexpr ExceptMove& operator=(ExceptMove&&) = default;
        };

        TRAP::Expected<ExceptMove, ExceptMove> e3 = {};
        TRAP::Expected<ExceptMove, ExceptMove> e4 = {};
        // e3 = e4;
    }

    SECTION("Triviality")
    {
        STATIC_REQUIRE(std::is_trivially_copy_constructible_v<TRAP::Expected<i32, i32>>);
        // STATIC_REQUIRE(std::is_trivially_copy_assignable_v<TRAP::Expected<i32, i32>>);
        STATIC_REQUIRE(std::is_trivially_move_constructible_v<TRAP::Expected<i32, i32>>);
        // STATIC_REQUIRE(std::is_trivially_move_assignable_v<TRAP::Expected<i32, i32>>);
        STATIC_REQUIRE(std::is_trivially_destructible_v<TRAP::Expected<i32, i32>>);

        STATIC_REQUIRE(std::is_trivially_copy_constructible_v<TRAP::Expected<void, i32>>);
        STATIC_REQUIRE(std::is_trivially_move_constructible_v<TRAP::Expected<void, i32>>);
        STATIC_REQUIRE(std::is_trivially_destructible_v<TRAP::Expected<void, i32>>);

        {
            struct T
            {
                constexpr T() = default;
                constexpr T(T&&) = default;
                constexpr T(const T&) = default;
                constexpr T& operator=(const T&) = default;
                constexpr T& operator=(T&&) = default;
                constexpr ~T() = default;
            };
            STATIC_REQUIRE(std::is_trivially_copy_constructible_v<TRAP::Expected<T, i32>>);
            // STATIC_REQUIRE(std::is_trivially_copy_assignable_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(std::is_trivially_move_constructible_v<TRAP::Expected<T, i32>>);
            // STATIC_REQUIRE(std::is_trivially_move_assignable_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(std::is_trivially_destructible_v<TRAP::Expected<T, i32>>);
        }

        {
            struct T
            {
                constexpr T()
                {}
                constexpr T([[maybe_unused]] T&& _)
                {}
                constexpr T(const T&) = default;
                constexpr T& operator=([[maybe_unused]] const T& _)
                {
                    return *this;
                }
                constexpr T& operator=([[maybe_unused]] T&& _)
                {
                    return *this;
                }
                constexpr ~T()
                {}
            };
            STATIC_REQUIRE(!std::is_trivially_copy_constructible_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(!std::is_trivially_copy_assignable_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(!std::is_trivially_move_constructible_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(!std::is_trivially_move_assignable_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(!std::is_trivially_destructible_v<TRAP::Expected<T, i32>>);
        }

        {
            STATIC_REQUIRE(!std::is_trivially_destructible_v<std::vector<i32>>);
            [[maybe_unused]] TRAP::Expected<void, std::vector<i32>> e = TRAP::MakeUnexpected(std::vector<i32>{1, 2, 3});
        }
    }

    SECTION("Deletion")
    {
        STATIC_REQUIRE(std::is_copy_constructible_v<TRAP::Expected<i32, i32>>);
        STATIC_REQUIRE(std::is_copy_assignable_v<TRAP::Expected<i32, i32>>);
        STATIC_REQUIRE(std::is_move_constructible_v<TRAP::Expected<i32, i32>>);
        STATIC_REQUIRE(std::is_move_assignable_v<TRAP::Expected<i32, i32>>);
        STATIC_REQUIRE(std::is_destructible_v<TRAP::Expected<i32, i32>>);

        {
            struct T
            {
                constexpr T() = default;
            };
            STATIC_REQUIRE(std::is_default_constructible_v<TRAP::Expected<T, i32>>);
        }

        {
            struct T
            {
                constexpr T([[maybe_unused]] i32 _)
                {}
            };
            REQUIRE(!std::is_default_constructible_v<TRAP::Expected<T, i32>>);
        }

        {
            struct T
            {
                constexpr T(const T&) = default;
                constexpr T(T&&) = default;
                T& operator=(const T&) = default;
                T& operator=(T&&) = default;
                constexpr ~T() = default;
            };
            STATIC_REQUIRE(std::is_copy_constructible_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(std::is_copy_assignable_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(std::is_move_constructible_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(std::is_move_assignable_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(std::is_destructible_v<TRAP::Expected<T, i32>>);
        }

        {
            struct T
            {
                consteval T(const T&) = delete;
                consteval T(T&&) = delete;
                consteval T& operator=(const T&) = delete;
                consteval T& operator=(T&&) = delete;
                constexpr ~T() = default;
            };
            STATIC_REQUIRE(!std::is_copy_constructible_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(!std::is_copy_assignable_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(!std::is_move_constructible_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(!std::is_move_assignable_v<TRAP::Expected<T, i32>>);
        }

        {
            struct T
            {
                consteval T(const T&) = delete;
                constexpr T(T&&) = default;
                consteval T& operator=(const T&) = delete;
                T& operator=(T&&) = default;
                constexpr ~T() = default;
            };
            STATIC_REQUIRE(!std::is_copy_constructible_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(!std::is_copy_assignable_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(std::is_move_constructible_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(std::is_move_assignable_v<TRAP::Expected<T, i32>>);
        }

        {
            struct T
            {
                consteval T(const T&) = default;
                consteval T(T&&) = delete;
                T& operator=(const T&) = default;
                consteval T& operator=(T&&) = delete;
                constexpr ~T() = default;
            };
            STATIC_REQUIRE(std::is_copy_constructible_v<TRAP::Expected<T, i32>>);
            STATIC_REQUIRE(std::is_copy_assignable_v<TRAP::Expected<T, i32>>);
        }

        {
            TRAP::Expected<i32, i32> e;
            STATIC_REQUIRE(std::is_default_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_trivially_copy_constructible_v<decltype(e)>);
            // STATIC_REQUIRE(std::is_trivially_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_trivially_move_constructible_v<decltype(e)>);
            // STATIC_REQUIRE(std::is_trivially_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_nothrow_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_nothrow_move_assignable_v<decltype(e)>);
        }

        {
            TRAP::Expected<i32, std::string> e;
            STATIC_REQUIRE(std::is_default_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_nothrow_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_nothrow_move_assignable_v<decltype(e)>);
        }

        {
            TRAP::Expected<std::string, i32> e;
            STATIC_REQUIRE(std::is_default_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_nothrow_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_nothrow_move_assignable_v<decltype(e)>);
        }

        {
            TRAP::Expected<std::string, std::string> e;
            STATIC_REQUIRE(std::is_default_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_nothrow_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_nothrow_move_assignable_v<decltype(e)>);
        }

        {
            TRAP::Expected<i32, CanThrowMove> e;
            STATIC_REQUIRE(std::is_default_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_nothrow_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_nothrow_move_assignable_v<decltype(e)>);
        }

        {
            TRAP::Expected<CanThrowMove, i32> e;
            STATIC_REQUIRE(std::is_default_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_nothrow_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_nothrow_move_assignable_v<decltype(e)>);
        }

        {
            TRAP::Expected<CanThrowMove, CanThrowMove> e;
            STATIC_REQUIRE(std::is_default_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_constructible_v<decltype(e)>);
            // STATIC_REQUIRE(std::is_copy_assignable_v<decltype(e)>);
            // STATIC_REQUIRE(std::is_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_nothrow_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_nothrow_move_assignable_v<decltype(e)>);
        }

        {
            TRAP::Expected<void, i32> e;
            STATIC_REQUIRE(std::is_default_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_trivially_copy_constructible_v<decltype(e)>);
            // STATIC_REQUIRE(std::is_trivially_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_trivially_move_constructible_v<decltype(e)>);
            // STATIC_REQUIRE(std::is_trivially_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_nothrow_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_nothrow_move_assignable_v<decltype(e)>);
        }

        {
            TRAP::Expected<void, std::string> e;
            STATIC_REQUIRE(std::is_default_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_nothrow_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_nothrow_move_assignable_v<decltype(e)>);
        }

        {
            TRAP::Expected<void, CanThrowMove> e;
            STATIC_REQUIRE(std::is_default_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(std::is_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(std::is_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_copy_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_trivially_move_assignable_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_nothrow_move_constructible_v<decltype(e)>);
            STATIC_REQUIRE(!std::is_nothrow_move_assignable_v<decltype(e)>);
        }
    }

    SECTION("Constructors")
    {
        {
            static constexpr TRAP::Expected<i32, i32> e;
            STATIC_REQUIRE(e);
            STATIC_REQUIRE(e == 0);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = TRAP::MakeUnexpected(0);
            STATIC_REQUIRE(!e);
            STATIC_REQUIRE(e.Error() == 0);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 0);
            STATIC_REQUIRE(!e);
            STATIC_REQUIRE(e.Error() == 0);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(std::in_place, 42);
            STATIC_REQUIRE(e);
            STATIC_REQUIRE(e == 42);
        }

        {
            const TRAP::Expected<std::vector<i32>, i32> e(std::in_place, {0, 1});
            REQUIRE(e);
            REQUIRE((*e)[0] == 0);
            REQUIRE((*e)[1] == 1);
        }

        {
            static constexpr TRAP::Expected<std::tuple<i32, i32>, i32> e(std::in_place, 0, 1);
            STATIC_REQUIRE(e);
            STATIC_REQUIRE(std::get<0>(*e) == 0);
            STATIC_REQUIRE(std::get<1>(*e) == 1);
        }

        {
            const TRAP::Expected<TakesInitAndVariadic, i32> e(std::in_place, {0, 1}, 2, 3);
            REQUIRE(e);
            REQUIRE(e->v[0] == 0);
            REQUIRE(e->v[1] == 1);
            REQUIRE(std::get<0>(e->t) == 2);
            REQUIRE(std::get<1>(e->t) == 3);
        }

        {
            TRAP::Expected<void, i32> e;
            REQUIRE(e);
        }

        {
            static constexpr TRAP::Expected<void, i32> e(TRAP::Unexpect, 42);
            STATIC_REQUIRE(!e);
            STATIC_REQUIRE(e.Error() == 42);
        }
    }

    SECTION("Emplace")
    {
        {
            TRAP::Expected<std::unique_ptr<i32>, i32> e;
            e.Emplace(new int{42});
            REQUIRE(e);
            REQUIRE(**e == 42);
        }

        // {
        //     TRAP::Expected<std::vector<i32>, i32> e;
        //     e.Emplace({0, 1});
        //     REQUIRE(e);
        //     REQUIRE((*e)[0] == 0);
        //     REQUIRE((*e)[1] == 1);
        // }

        {
            TRAP::Expected<std::tuple<i32, i32>, i32> e;
            e.Emplace(2, 3);
            REQUIRE(e);
            REQUIRE(std::get<0>(*e) == 2);
            REQUIRE(std::get<1>(*e) == 3);
        }

        // {
        //     TRAP::Expected<TakesInitAndVariadic, i32> e = TRAP::MakeUnexpected(0);
        //     e.Emplace({0, 1}, 2, 3);
        //     REQUIRE(e);
        //     REQUIRE(e->v[0] == 0);
        //     REQUIRE(e->v[1] == 1);
        //     REQUIRE(std::get<0>(e->t) == 2);
        //     REQUIRE(std::get<1>(e->t) == 3);
        // }

        {
            TRAP::Expected<i32, i32> e = TRAP::MakeUnexpected(10);
            e.Emplace(2);
            REQUIRE(e);
            REQUIRE(*e == 2);
        }
    }

    SECTION("Observers")
    {
        TRAP::Expected<i32, i32> o1 = 42;
        TRAP::Expected<i32, i32> o2{TRAP::Unexpect, 0};
        const TRAP::Expected<i32, i32> o3 = 42;

        REQUIRE(*o1 == 42);
        REQUIRE(*o1 == o1.Value());
        REQUIRE(o1.ValueOrElse([](){return 12;}) == 42);
        REQUIRE(o2.ValueOr(42) == 42);
        REQUIRE(o2.ValueOrElse([](){return 12;}) == 12);
        REQUIRE(o2.Error() == 0);
        REQUIRE(o3.Value() == 42);
        STATIC_REQUIRE(std::same_as<decltype(o1.Value()), i32&>);
        STATIC_REQUIRE(std::same_as<decltype(o3.Value()), const i32&>);
        STATIC_REQUIRE(std::same_as<decltype(std::move(o1).Value()), i32&&>);
        STATIC_REQUIRE(std::same_as<decltype(std::move(o3).Value()), const i32&&>);

        TRAP::Expected<MoveDetector, i32> o4{std::in_place};
        const MoveDetector o5 = std::move(o4).Value();
        REQUIRE(o4->BeenMoved);
        REQUIRE(!o5.BeenMoved);

        TRAP::Expected<i32, i32> o6{TRAP::Unexpect, 0};
        REQUIRE_THROWS_AS(o6.Value(), TRAP::BadExpectedAccess<i32>);
        REQUIRE_THROWS_AS(std::move(o6).Value(), TRAP::BadExpectedAccess<i32>);
        static constexpr TRAP::Expected<i32, i32> o7{TRAP::Unexpect, 0};
        REQUIRE_THROWS_AS(o7.Value(), TRAP::BadExpectedAccess<i32>);
        REQUIRE_THROWS_AS(std::move(o7.Value()), TRAP::BadExpectedAccess<i32>);

        const TRAP::Expected<void, i32> o8{TRAP::Unexpect, 0};
        REQUIRE(o8.Error() == 0);
    }

    SECTION("Relational operators")
    {
        static constexpr TRAP::Expected<i32, i32> o1 = 42;
        static constexpr TRAP::Expected<i32, i32> o2{TRAP::Unexpect, 0};
        static constexpr TRAP::Expected<i32, i32> o3 = 42;

        STATIC_REQUIRE(o1 == o1);
        STATIC_REQUIRE(o1 != o2);
        STATIC_REQUIRE(o1 == o3);
        STATIC_REQUIRE(o3 == o3);

        const TRAP::Expected<void, i32> o6;
        REQUIRE(o6 == o6);

        const TRAP::Expected<void, i32> o7(TRAP::Unexpect, 0);
        REQUIRE(o7 == o7);
    }

#if defined(__EXCEPTIONS) || defined(_HAS_EXCEPTIONS) || defined(__cpp_exceptions)
    SECTION("Swap")
    {
        SwapTest<NoThrow, NoThrow>();
        SwapTest<NoThrow, CanThrowMove>();
        SwapTest<CanThrowMove, NoThrow>();

        const std::string s1 = "abcdefghijklmnopqrstuvwxyz";
        const std::string s2 = "zyxwvutsrqponmlkjihgfedcbaxxx";
        TRAP::Expected<NoThrow, WillThrowMove> a{s1};
        TRAP::Expected<NoThrow, WillThrowMove> b{TRAP::Unexpect, s2};
        ShouldThrow = true;

#ifdef _MSC_VER
        REQUIRE_THROWS(std::swap(a, b));
#else
        REQUIRE(a->i == s1);
        REQUIRE(b.Error().i == s2);
#endif /*_MSC_VER*/
    }
#endif

    SECTION("Transform")
    {
        static constexpr auto mul2 = [](const i32 a){return a * 2;};
        static constexpr auto retVoid = []([[maybe_unused]] const i32 a){};

        {
            TRAP::Expected<i32, i32> e = 21;
            const auto ret = e.Transform(mul2);
            REQUIRE(ret);
            REQUIRE(*ret == 42);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 21;
            static constexpr auto ret = e.Transform(mul2);
            STATIC_REQUIRE(ret);
            STATIC_REQUIRE(*ret == 42);
        }

        {
            TRAP::Expected<i32, i32> e = 21;
            const auto ret = std::move(e).Transform(mul2);
            REQUIRE(ret);
            REQUIRE(*ret == 42);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 21;
            static constexpr auto ret = std::move(e).Transform(mul2);
            STATIC_REQUIRE(ret);
            STATIC_REQUIRE(*ret == 42);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            const auto ret = e.Transform(mul2);
            REQUIRE(!ret);
            REQUIRE(ret.Error() == 21);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            static constexpr auto ret = e.Transform(mul2);
            STATIC_REQUIRE(!ret);
            STATIC_REQUIRE(ret.Error() == 21);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            const auto ret = std::move(e).Transform(mul2);
            REQUIRE(!ret);
            REQUIRE(ret.Error() == 21);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            static constexpr auto ret = std::move(e).Transform(mul2);
            STATIC_REQUIRE(!ret);
            STATIC_REQUIRE(ret.Error() == 21);
        }

        {
            TRAP::Expected<i32, i32> e = 21;
            const auto ret = e.Transform(retVoid);
            REQUIRE(ret);
            STATIC_REQUIRE((std::same_as<decltype(ret), const TRAP::Expected<void, i32>>));
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 21;
            static constexpr auto ret = e.Transform(retVoid);
            STATIC_REQUIRE(ret);
            STATIC_REQUIRE((std::same_as<decltype(ret), const TRAP::Expected<void, i32>>));
        }

        {
            TRAP::Expected<i32, i32> e = 21;
            const auto ret = std::move(e).Transform(retVoid);
            REQUIRE(ret);
            STATIC_REQUIRE((std::same_as<decltype(ret), const TRAP::Expected<void, i32>>));
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 21;
            static constexpr auto ret = std::move(e).Transform(retVoid);
            STATIC_REQUIRE(ret);
            STATIC_REQUIRE((std::same_as<decltype(ret), const TRAP::Expected<void, i32>>));
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            const auto ret = e.Transform(retVoid);
            REQUIRE(!ret);
            STATIC_REQUIRE((std::same_as<decltype(ret), const TRAP::Expected<void, i32>>));
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            static constexpr auto ret = e.Transform(retVoid);
            STATIC_REQUIRE(!ret);
            STATIC_REQUIRE((std::same_as<decltype(ret), const TRAP::Expected<void, i32>>));
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            const auto ret = std::move(e).Transform(retVoid);
            REQUIRE(!ret);
            STATIC_REQUIRE((std::same_as<decltype(ret), const TRAP::Expected<void, i32>>));
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            static constexpr auto ret = std::move(e).Transform(retVoid);
            STATIC_REQUIRE(!ret);
            STATIC_REQUIRE((std::same_as<decltype(ret), const TRAP::Expected<void, i32>>));
        }

        // {
        //     TRAP::Expected<i32, i32> e(42);
        //     const auto ret = e.Transform([](i32& i) -> i32& {return i;});
        //     REQUIRE(ret);
        //     REQUIRE(ret == 42);
        // }
    }

    SECTION("TransformError")
    {
        static constexpr auto mul2 = [](const i32 a){return a * 2;};
        static constexpr auto retVoid  = []([[maybe_unused]] const i32 a){};

        {
            TRAP::Expected<i32, i32> e = 21;
            const auto ret = e.TransformError(mul2);
            REQUIRE(ret);
            REQUIRE(*ret == 21);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 21;
            static constexpr auto ret = e.TransformError(mul2);
            STATIC_REQUIRE(ret);
            STATIC_REQUIRE(*ret == 21);
        }

        {
            TRAP::Expected<i32, i32> e = 21;
            const auto ret = std::move(e).TransformError(mul2);
            REQUIRE(ret);
            REQUIRE(*ret == 21);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 21;
            static constexpr auto ret = std::move(e).TransformError(mul2);
            STATIC_REQUIRE(ret);
            STATIC_REQUIRE(*ret == 21);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            const auto ret = e.TransformError(mul2);
            REQUIRE(!ret);
            REQUIRE(ret.Error() == 42);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            static constexpr auto ret = e.TransformError(mul2);
            STATIC_REQUIRE(!ret);
            STATIC_REQUIRE(ret.Error() == 42);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            const auto ret = std::move(e).TransformError(mul2);
            REQUIRE(!ret);
            REQUIRE(ret.Error() == 42);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            static constexpr auto ret = std::move(e).TransformError(mul2);
            STATIC_REQUIRE(!ret);
            STATIC_REQUIRE(ret.Error() == 42);
        }

        // {
        //     TRAP::Expected<i32, i32> e = 21;
        //     const auto ret = e.TransformError(retVoid);
        //     REQUIRE(ret);
        // }

        // {
        //     static constexpr TRAP::Expected<i32, i32> e = 21;
        //     static constexpr auto ret = e.TransformError(retVoid);
        //     STATIC_REQUIRE(ret);
        // }

        // {
        //     TRAP::Expected<i32, i32> e = 21;
        //     const auto ret = std::move(e).TransformError(retVoid);
        //     REQUIRE(ret);
        // }

        // {
        //     static constexpr TRAP::Expected<i32, i32> e = 21;
        //     static constexpr auto ret = std::move(e).TransformError(retVoid);
        //     STATIC_REQUIRE(ret);
        // }

        // {
        //     TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
        //     const auto ret = e.TransformError(retVoid);
        //     REQUIRE(!ret);
        // }

        // {
        //     static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
        //     static constexpr auto ret = e.TransformError(retVoid);
        //     STATIC_REQUIRE(!ret);
        // }

        // {
        //     TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
        //     const auto ret = std::move(e).TransformError(retVoid);
        //     REQUIRE(!ret);
        // }

        // {
        //     static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
        //     static constexpr auto ret = std::move(e).TransformError(retVoid);
        //     STATIC_REQUIRE(!ret);
        // }
    }

    SECTION("AndThen")
    {
        static constexpr auto succeed = []([[maybe_unused]] const i32 a) {return TRAP::Expected<i32, i32>(21 * 2);};
        static constexpr auto fail = []([[maybe_unused]] const i32 a){return TRAP::Expected<i32, i32>(TRAP::Unexpect, 17);};

        {
            TRAP::Expected<i32, i32> e = 21;
            const auto ret = e.AndThen(succeed);
            REQUIRE(ret);
            REQUIRE(*ret == 42);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 21;
            static constexpr auto ret = e.AndThen(succeed);
            STATIC_REQUIRE(ret);
            STATIC_REQUIRE(*ret == 42);
        }

        {
            TRAP::Expected<i32, i32> e = 21;
            const auto ret = std::move(e).AndThen(succeed);
            REQUIRE(ret);
            REQUIRE(*ret == 42);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 21;
            static constexpr auto ret = std::move(e).AndThen(succeed);
            STATIC_REQUIRE(ret);
            STATIC_REQUIRE(*ret == 42);
        }

        {
            TRAP::Expected<i32, i32> e = 21;
            const auto ret = e.AndThen(fail);
            REQUIRE(!ret);
            REQUIRE(ret.Error() == 17);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 21;
            static constexpr auto ret = e.AndThen(fail);
            STATIC_REQUIRE(!ret);
            STATIC_REQUIRE(ret.Error() == 17);
        }

        {
            TRAP::Expected<i32, i32> e = 21;
            const auto ret = std::move(e).AndThen(fail);
            REQUIRE(!ret);
            REQUIRE(ret.Error() == 17);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 21;
            static constexpr auto ret = std::move(e).AndThen(fail);
            STATIC_REQUIRE(!ret);
            STATIC_REQUIRE(ret.Error() == 17);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            const auto ret = e.AndThen(succeed);
            REQUIRE(!ret);
            REQUIRE(ret.Error() == 21);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            static constexpr auto ret = e.AndThen(succeed);
            STATIC_REQUIRE(!ret);
            STATIC_REQUIRE(ret.Error() == 21);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            const auto ret = std::move(e).AndThen(succeed);
            REQUIRE(!ret);
            REQUIRE(ret.Error() == 21);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            static constexpr auto ret = std::move(e).AndThen(succeed);
            STATIC_REQUIRE(!ret);
            STATIC_REQUIRE(ret.Error() == 21);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            const auto ret = e.AndThen(fail);
            REQUIRE(!ret);
            REQUIRE(ret.Error() == 21);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            static constexpr auto ret = e.AndThen(fail);
            STATIC_REQUIRE(!ret);
            STATIC_REQUIRE(ret.Error() == 21);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            const auto ret = std::move(e).AndThen(fail);
            REQUIRE(!ret);
            REQUIRE(ret.Error() == 21);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            static constexpr auto ret = std::move(e).AndThen(fail);
            STATIC_REQUIRE(!ret);
            STATIC_REQUIRE(ret.Error() == 21);
        }
    }

    SECTION("OrElse")
    {
        using eptr = std::unique_ptr<i32>;
        static constexpr auto succeed = []([[maybe_unused]] const i32 a){return TRAP::Expected<i32, i32>(21 * 2);};
        static constexpr auto succeedptr = []([[maybe_unused]] const eptr e){return TRAP::Expected<i32, eptr>(21 * 2);};
        static constexpr auto fail = []([[maybe_unused]] const i32 a){return TRAP::Expected<i32, i32>(TRAP::Unexpect, 17);};
        static constexpr auto failptr = [](eptr e){*e = 18; return TRAP::Expected<i32, eptr>(TRAP::Unexpect, std::move(e));};
        static constexpr auto failvoid = []([[maybe_unused]] const i32 _){};
        static constexpr auto failvoidptr = []([[maybe_unused]] const eptr& _){};
        static constexpr auto consumeptr = []([[maybe_unused]] const eptr _){};
        static constexpr auto makeUInt = [](const i32 n){return std::make_unique<i32>(n);};

        {
            TRAP::Expected<i32, i32> e = 21;
            const auto ret = e.OrElse(succeed);
            REQUIRE(ret);
            REQUIRE(*ret == 21);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 21;
            static constexpr auto ret = e.OrElse(succeed);
            STATIC_REQUIRE(ret);
            STATIC_REQUIRE(*ret == 21);
        }

        {
            TRAP::Expected<i32, i32> e = 21;
            const auto ret = std::move(e).OrElse(succeed);
            REQUIRE(ret);
            REQUIRE(*ret == 21);
        }

        {
            TRAP::Expected<i32, eptr> e = 21;
            const auto ret = std::move(e).OrElse(succeedptr);
            REQUIRE(ret);
            REQUIRE(*ret == 21);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 21;
            static constexpr auto ret = std::move(e).OrElse(succeed);
            STATIC_REQUIRE(ret);
            STATIC_REQUIRE(*ret == 21);
        }

        {
            TRAP::Expected<i32, i32> e = 21;
            const auto ret = e.OrElse(fail);
            REQUIRE(ret);
            REQUIRE(*ret == 21);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 21;
            static constexpr auto ret = e.OrElse(fail);
            STATIC_REQUIRE(ret);
            STATIC_REQUIRE(*ret == 21);
        }

        {
            TRAP::Expected<i32, i32> e = 21;
            const auto ret = std::move(e).OrElse(fail);
            REQUIRE(ret);
            REQUIRE(ret == 21);
        }

        {
            TRAP::Expected<i32, eptr> e = 21;
            const auto ret = std::move(e).OrElse(failptr);
            REQUIRE(ret);
            REQUIRE(ret == 21);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 21;
            static constexpr auto ret = std::move(e).OrElse(fail);
            STATIC_REQUIRE(ret);
            STATIC_REQUIRE(*ret == 21);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            const auto ret = e.OrElse(succeed);
            REQUIRE(ret);
            REQUIRE(*ret == 42);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            static constexpr auto ret = e.OrElse(succeed);
            STATIC_REQUIRE(ret);
            STATIC_REQUIRE(*ret == 42);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            const auto ret = std::move(e).OrElse(succeed);
            REQUIRE(ret);
            REQUIRE(*ret == 42);
        }

        {
            TRAP::Expected<i32, eptr> e(TRAP::Unexpect, makeUInt(21));
            const auto ret = std::move(e).OrElse(succeedptr);
            REQUIRE(ret);
            REQUIRE(*ret == 42);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            static constexpr auto ret = std::move(e).OrElse(succeed);
            STATIC_REQUIRE(ret);
            STATIC_REQUIRE(*ret == 42);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            const auto ret = e.OrElse(fail);
            REQUIRE(!ret);
            REQUIRE(ret.Error() == 17);
        }

        // {
        //     TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
        //     const auto ret = e.OrElse(failvoid);
        //     REQUIRE(!ret);
        //     REQUIRE(ret.Error() == 21);
        // }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            static constexpr auto ret = e.OrElse(fail);
            STATIC_REQUIRE(!ret);
            STATIC_REQUIRE(ret.Error() == 17);
        }

        // {
        //     static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
        //     static constexpr auto ret = e.OrElse(failvoid);
        //     STATIC_REQUIRE(!ret);
        //     STATIC_REQUIRE(ret.Error() == 21);
        // }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            const auto ret = std::move(e).OrElse(fail);
            REQUIRE(!ret);
            REQUIRE(ret.Error() == 17);
        }

        // {
        //     TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
        //     const auto ret = std::move(e).OrElse(failvoid);
        //     REQUIRE(!ret);
        //     REQUIRE(ret.Error() == 21);
        // }

        // {
        //     TRAP::Expected<i32, eptr> e(TRAP::Unexpect, makeUInt(21));
        //     const auto ret = std::move(e).OrElse(failvoidptr);
        //     REQUIRE(!ret);
        //     REQUIRE(*ret.Error() == 21);
        // }

        // {
        //     TRAP::Expected<i32, eptr> e(TRAP::Unexpect, makeUInt(21));
        //     const auto ret = std::move(e).OrElse(consumeptr);
        //     REQUIRE(!ret);
        //     REQUIRE(ret.Error() == nullptr);
        // }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
            static constexpr auto ret = std::move(e).OrElse(fail);
            STATIC_REQUIRE(!ret);
            STATIC_REQUIRE(ret.Error() == 17);
        }

        // {
        //     static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 21);
        //     static constexpr auto ret = std::move(e).OrElse(failvoid);
        //     STATIC_REQUIRE(!ret);
        //     STATIC_REQUIRE(ret.Error() == 21);
        // }
    }

    SECTION("ValueOr")
    {
        {
            static constexpr TRAP::Expected<i32, i32> e = 42;
            static constexpr i32 ret = e.ValueOr(12);
            STATIC_REQUIRE(ret == 42);
        }

        {
            TRAP::Expected<i32, i32> e = 42;
            const i32 ret = e.ValueOr(12);
            REQUIRE(ret == 42);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 42;
            static constexpr i32 ret = std::move(e).ValueOr(12);
            STATIC_REQUIRE(ret == 42);
        }

        {
            TRAP::Expected<i32, i32> e = 42;
            const i32 ret = std::move(e).ValueOr(12);
            REQUIRE(ret == 42);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 5);
            static constexpr i32 ret = e.ValueOr(12);
            STATIC_REQUIRE(ret == 12);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 5);
            const i32 ret = e.ValueOr(12);
            REQUIRE(ret == 12);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 5);
            static constexpr i32 ret = std::move(e).ValueOr(12);
            STATIC_REQUIRE(ret == 12);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 5);
            const i32 ret = std::move(e).ValueOr(12);
            REQUIRE(ret == 12);
        }
    }

    SECTION("ValueOrElse")
    {
        static constexpr auto ValueElse = [](){return 12;};

        {
            static constexpr TRAP::Expected<i32, i32> e = 42;
            static constexpr i32 ret = e.ValueOrElse(ValueElse);
            STATIC_REQUIRE(ret == 42);
        }

        {
            TRAP::Expected<i32, i32> e = 42;
            const i32 ret = e.ValueOr(12);
            REQUIRE(ret == 42);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 42;
            static constexpr i32 ret = std::move(e).ValueOrElse(ValueElse);
            STATIC_REQUIRE(ret == 42);
        }

        {
            TRAP::Expected<i32, i32> e = 42;
            const i32 ret = std::move(e).ValueOrElse(ValueElse);
            REQUIRE(ret == 42);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 5);
            static constexpr i32 ret = e.ValueOrElse(ValueElse);
            STATIC_REQUIRE(ret == 12);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 5);
            const i32 ret = e.ValueOrElse(ValueElse);
            REQUIRE(ret == 12);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 5);
            static constexpr i32 ret = std::move(e).ValueOrElse(ValueElse);
            STATIC_REQUIRE(ret == 12);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 5);
            const i32 ret = std::move(e).ValueOrElse(ValueElse);
            REQUIRE(ret == 12);
        }
    }

    SECTION("ErrorOr")
    {
        {
            static constexpr TRAP::Expected<i32, i32> e = 42;
            static constexpr i32 ret = e.ErrorOr(12);
            STATIC_REQUIRE(ret == 12);
        }

        {
            TRAP::Expected<i32, i32> e = 42;
            const i32 ret = e.ErrorOr(12);
            REQUIRE(ret == 12);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e = 42;
            static constexpr i32 ret = std::move(e).ErrorOr(12);
            STATIC_REQUIRE(ret == 12);
        }

        {
            TRAP::Expected<i32, i32> e = 42;
            const i32 ret = std::move(e).ErrorOr(12);
            REQUIRE(ret == 12);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 5);
            static constexpr i32 ret = e.ErrorOr(12);
            STATIC_REQUIRE(ret == 5);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 5);
            const i32 ret = e.ErrorOr(12);
            REQUIRE(ret == 5);
        }

        {
            static constexpr TRAP::Expected<i32, i32> e(TRAP::Unexpect, 5);
            static constexpr i32 ret = std::move(e).ErrorOr(12);
            STATIC_REQUIRE(ret == 5);
        }

        {
            TRAP::Expected<i32, i32> e(TRAP::Unexpect, 5);
            const i32 ret = std::move(e).ErrorOr(12);
            REQUIRE(ret == 5);
        }
    }

    SECTION("BadExpectedAccess")
    {
        const TRAP::BadExpectedAccess<i32> err(12);
        [[maybe_unused]] const char* test = err.what();
        REQUIRE(err.Error() == 12);

        REQUIRE_THROWS_AS(TRAP::INTERNAL::ThrowOrAbort(err), TRAP::BadExpectedAccess<i32>);

        [[maybe_unused]] const TRAP::BadExpectedAccess<i32> err2(err);
    }

    SECTION("Guard")
    {
        i32 value = 10;
        [[maybe_unused]] TRAP::INTERNAL::Guard<i32> guard(value);
    }
}
