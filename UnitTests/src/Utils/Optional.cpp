#include <catch2/catch_test_macros.hpp>

#include <unordered_set>
#include <array>

#include "Core/Types.h"
#include "Utils/Optional.h"

namespace
{
    struct TakesInitAndVariadic
    {
        std::vector<i32> v;
        std::tuple<i32, i32> t;

        template<typename... Args>
        constexpr TakesInitAndVariadic(std::initializer_list<i32> l, Args&&... args)
            : v(l), t(std::forward<Args>(args)...)
        {}
    };

    [[nodiscard]] constexpr i32 GetInt([[maybe_unused]] const i32 _) noexcept
    {
        return 42;
    }

    constexpr TRAP::Optional<i32> GetOptInt([[maybe_unused]] const i32 _) noexcept
    {
        return 42;
    }

    template<typename T, typename = std::void_t<>>
    struct IsHashable : std::false_type
    {};

    template<typename T>
    struct IsHashable<T, std::void_t<decltype(std::declval<std::hash<T>>()(std::declval<T>()))>> : std::true_type
    {};

    template<typename T>
    constexpr bool IsHashableV = IsHashable<T>::value;

    struct NotHashable
    {};
}

TEST_CASE("TRAP::Optional<T>", "[utils][optional]")
{
    SECTION("Assignment value")
    {
        TRAP::Optional<i32> o1 = 42;
        static constexpr TRAP::Optional<i32> o2 = 12;
        static constexpr TRAP::Optional<i32> o3;

        o1 = o1;
        REQUIRE(*o1 == 42);

        o1 = o2;
        REQUIRE(*o1 == 12);

        o1 = o3;
        REQUIRE(!o1);

        o1 = 42;
        REQUIRE(*o1 == 42);

        o1 = TRAP::NullOpt;
        REQUIRE(!o1);

        o1 = std::move(o2);
        REQUIRE(*o1 == 12);

        static constexpr TRAP::Optional<i16> o4 = 42;

        o1 = o4;
        REQUIRE(*o1 == 42);

        o1 = std::move(o4);
        REQUIRE(*o1 == 42);
    }

    SECTION("Triviality")
    {
        STATIC_REQUIRE(std::is_trivially_copy_constructible_v<TRAP::Optional<i32>>);
        STATIC_REQUIRE(std::is_trivially_copy_assignable_v<TRAP::Optional<i32>>);
        STATIC_REQUIRE(std::is_trivially_move_constructible_v<TRAP::Optional<i32>>);
        STATIC_REQUIRE(std::is_trivially_move_assignable_v<TRAP::Optional<i32>>);
        STATIC_REQUIRE(std::is_trivially_destructible_v<TRAP::Optional<i32>>);

        {
            struct T
            {
                constexpr T(const T&) = default;
                constexpr T(T&&) noexcept = default;
                T& operator=(const T&) = default;
                T& operator=(T&&) noexcept = default;
                constexpr ~T() = default;
            };
            STATIC_REQUIRE(std::is_trivially_copy_constructible_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(std::is_trivially_copy_assignable_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(std::is_trivially_move_constructible_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(std::is_trivially_move_assignable_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(std::is_trivially_destructible_v<TRAP::Optional<T>>);
        }
        {
            struct T
            {
                constexpr T(const T&)
                {}
                constexpr T(T&&) noexcept
                {}
                constexpr T& operator=(const T&)
                {
                    return *this;
                }
                constexpr T& operator=(T&&) noexcept
                {
                    return *this;
                }
                constexpr ~T()
                {}
            };
            STATIC_REQUIRE(!std::is_trivially_copy_constructible_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(!std::is_trivially_copy_assignable_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(!std::is_trivially_move_constructible_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(!std::is_trivially_move_assignable_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(!std::is_trivially_destructible_v<TRAP::Optional<T>>);
        }
    }

    SECTION("Deletion")
    {
        STATIC_REQUIRE(std::is_copy_constructible_v<TRAP::Optional<i32>>);
        STATIC_REQUIRE(std::is_copy_assignable_v<TRAP::Optional<i32>>);
        STATIC_REQUIRE(std::is_move_constructible_v<TRAP::Optional<i32>>);
        STATIC_REQUIRE(std::is_move_assignable_v<TRAP::Optional<i32>>);
        STATIC_REQUIRE(std::is_destructible_v<TRAP::Optional<i32>>);

        {
            struct T
            {
                constexpr T(const T&) = default;
                constexpr T(T&&) noexcept = default;
                T& operator=(const T&) = default;
                T& operator=(T&&) noexcept = default;
                constexpr ~T() = default;
            };
            STATIC_REQUIRE(std::is_copy_constructible_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(std::is_copy_assignable_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(std::is_move_constructible_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(std::is_move_assignable_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(std::is_destructible_v<TRAP::Optional<T>>);
        }
        {
            struct T
            {
                consteval T(const T&) = delete;
                consteval T(T&&) noexcept = delete;
                consteval T& operator=(const T&) = delete;
                consteval T& operator=(T&&) noexcept = delete;
            };
            STATIC_REQUIRE(!std::is_copy_constructible_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(!std::is_copy_assignable_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(!std::is_move_constructible_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(!std::is_move_assignable_v<TRAP::Optional<T>>);
        }
        {
            struct T
            {
                consteval T(const T&) = delete;
                constexpr T(T&&) noexcept = default;
                consteval T& operator=(const T&) = delete;
                T& operator=(T&&) noexcept = default;
            };
            STATIC_REQUIRE(!std::is_copy_constructible_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(!std::is_copy_assignable_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(std::is_move_constructible_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(std::is_move_assignable_v<TRAP::Optional<T>>);
        }
        {
            struct T
            {
                constexpr T(const T&) = default;
                consteval T(T&&) noexcept = delete;
                T& operator=(const T&) = default;
                consteval T& operator=(T&&) noexcept = delete;
            };
            STATIC_REQUIRE(std::is_copy_constructible_v<TRAP::Optional<T>>);
            STATIC_REQUIRE(std::is_copy_assignable_v<TRAP::Optional<T>>);
        }
    }

    SECTION("Constexpr - Empty constructor")
    {
        static constexpr TRAP::Optional<i32> o2{};
        static constexpr TRAP::Optional<i32> o3 = {};
        static constexpr TRAP::Optional<i32> o4 = TRAP::NullOpt;
        static constexpr TRAP::Optional<i32> o5 = {TRAP::NullOpt};
        static constexpr TRAP::Optional<i32> o6{TRAP::NullOpt};

        STATIC_REQUIRE(!o2);
        STATIC_REQUIRE(!o3);
        STATIC_REQUIRE(!o4);
        STATIC_REQUIRE(!o5);
        STATIC_REQUIRE(!o6);
    }

    SECTION("Constexpr - Value constructor")
    {
        static constexpr TRAP::Optional<i32> o1 = 42;
        static constexpr TRAP::Optional<i32> o2{42};
        static constexpr TRAP::Optional<i32> o3(42);
        static constexpr TRAP::Optional<i32> o4 = {42};
        static constexpr i32 i = 42;
        static constexpr TRAP::Optional<i32> o5 = std::move(i);
        static constexpr TRAP::Optional<i32> o6{std::move(i)};
        static constexpr TRAP::Optional<i32> o7(std::move(i));
        static constexpr TRAP::Optional<i32> o8 = {std::move(i)};

        STATIC_REQUIRE(*o1 == 42);
        STATIC_REQUIRE(*o2 == 42);
        STATIC_REQUIRE(*o3 == 42);
        STATIC_REQUIRE(*o4 == 42);
        STATIC_REQUIRE(*o5 == 42);
        STATIC_REQUIRE(*o6 == 42);
        STATIC_REQUIRE(*o7 == 42);
        STATIC_REQUIRE(*o8 == 42);
    }

    SECTION("Constructors")
    {
        struct Foo
        {
            constexpr Foo() = default;
            consteval Foo(Foo&) = delete;
            constexpr Foo(Foo&&)
            {}
        };

        TRAP::Optional<i32> o1;
        REQUIRE(!o1);

        TRAP::Optional<i32> o2 = TRAP::NullOpt;
        REQUIRE(!o2);

        TRAP::Optional<i32> o3 = 42;
        REQUIRE(*o3 == 42);

        TRAP::Optional<i32> o4 = o3;
        REQUIRE(*o4 == 42);

        TRAP::Optional<i32> o5 = o1;
        REQUIRE(!o5);

        TRAP::Optional<i32> o6 = std::move(o3);
        REQUIRE(*o6 == 42);

        TRAP::Optional<i16> o7 = 42;
        REQUIRE(*o7 == 42);

        TRAP::Optional<i32> o8 = o7;
        REQUIRE(*o8 == 42);

        TRAP::Optional<i32> o9 = std::move(o7);
        REQUIRE(*o9 == 42);

        std::vector<Foo> v;
        v.emplace_back();
        TRAP::Optional<std::vector<Foo>> ov = std::move(v);
        REQUIRE(ov->size() == 1);

        TRAP::Optional<i32> nullOpt(TRAP::NullOpt);
        REQUIRE(!nullOpt.HasValue());
        REQUIRE_THROWS_AS(nullOpt.Value(), TRAP::BadOptionalAccess);
        REQUIRE_THROWS_AS(std::move(nullOpt).Value(), TRAP::BadOptionalAccess);

        {
            TRAP::Optional<i64> conversionOpt = 10;
            TRAP::Optional<i32> conversionOpt1(TRAP::NullOpt);
            conversionOpt = conversionOpt1;
        }

        {
            TRAP::Optional<i64> conversionOpt(TRAP::NullOpt);
            const TRAP::Optional<i32> conversionOpt1 = 10;
            conversionOpt = conversionOpt1;
        }

        {
            TRAP::Optional<i64> conversionOpt(TRAP::NullOpt);
            const TRAP::Optional<i32> conversionOpt1{TRAP::NullOpt};
            conversionOpt = conversionOpt1;
        }

        {
            REQUIRE(!TRAP::Optional<i64>(TRAP::Optional<i32>{TRAP::NullOpt}).HasValue());
        }

        {
            const TRAP::Optional<i32> conversionOpt{TRAP::NullOpt};
            REQUIRE(!TRAP::Optional<i64>(conversionOpt).HasValue());
        }
    }

    SECTION("Emplace")
    {
        TRAP::Optional<std::pair<std::pair<i32, i32>, std::pair<f64, f64>>> i;
        i.Emplace(std::piecewise_construct, std::make_tuple(0, 2), std::make_tuple(3, 4));
        REQUIRE(i->first.first == 0);
        REQUIRE(i->first.second == 2);
        REQUIRE(i->second.first == 3);
        REQUIRE(i->second.second == 4);
    }

    SECTION("Emplace with exception thrown")
    {
        struct A
        {
            A()
            {
                throw std::exception();
            }
        };

        TRAP::Optional<A> a;
        REQUIRE_THROWS(a.Emplace());
    }

    SECTION("Hashing")
    {
        static constexpr TRAP::Optional<i32> op1(1);

        STATIC_REQUIRE(IsHashableV<TRAP::Optional<i32>>);
        STATIC_REQUIRE(!IsHashableV<TRAP::Optional<NotHashable>>);

        REQUIRE(std::hash<i32>{}(1) == std::hash<TRAP::Optional<i32>>{}(op1));

        static constexpr TRAP::Optional<i32> op2(TRAP::NullOpt);
        STATIC_REQUIRE(std::hash<TRAP::Optional<i32>>{}(op2) == std::numeric_limits<std::size_t>::max());
        TRAP::Optional<i32> op2_1(TRAP::NullOpt);
        REQUIRE(std::hash<TRAP::Optional<i32>>{}(op2_1) == std::numeric_limits<std::size_t>::max());

        using OptStr = TRAP::Optional<std::string>;

        const std::array<OptStr, 4> s{"ABC", "abc", TRAP::NullOpt, "def"};

#ifdef TRAP_PLATFORM_LINUX
        REQUIRE(std::hash<OptStr>{}(std::get<0>(s)) == 11746482041453314842u);
        REQUIRE(std::hash<OptStr>{}(std::get<1>(s)) == 3663726644998027833);
        REQUIRE(std::hash<OptStr>{}(std::get<2>(s)) == std::numeric_limits<std::size_t>::max());
        REQUIRE(std::hash<OptStr>{}(std::get<3>(s)) == 11697390762615875584u);
#elif defined(TRAP_PLATFORM_WINDOWS)
        REQUIRE(std::hash<OptStr>{}(std::get<0>(s)) == 18027876433081418475u);
        REQUIRE(std::hash<OptStr>{}(std::get<1>(s)) == 3663726644998027833);
        REQUIRE(std::hash<OptStr>{}(std::get<2>(s)) == std::numeric_limits<std::size_t>::max());
        REQUIRE(std::hash<OptStr>{}(std::get<3>(s)) == 11697390762615875584u);
#endif
    }

    SECTION("In place")
    {
        static constexpr TRAP::Optional<i32> o1{TRAP::InPlace};
        static constexpr TRAP::Optional<i32> o2(TRAP::InPlace);
        STATIC_REQUIRE(o1);
        STATIC_REQUIRE(o1 == 0);
        STATIC_REQUIRE(o2);
        STATIC_REQUIRE(o2 == 0);

        static constexpr TRAP::Optional<i32> o3(TRAP::InPlace, 42);
        STATIC_REQUIRE(o3 == 42);

        static constexpr TRAP::Optional<std::tuple<i32, i32>> o4(TRAP::InPlace, 0, 1);
        STATIC_REQUIRE(o4);
        STATIC_REQUIRE(std::get<0>(*o4) == 0);
        STATIC_REQUIRE(std::get<1>(*o4) == 1);

        TRAP::Optional<std::vector<i32>> o5(TRAP::InPlace, {0, 1});
        REQUIRE(o5);
        REQUIRE((*o5)[0] == 0);
        REQUIRE((*o5)[1] == 1);

        TRAP::Optional<TakesInitAndVariadic> o6(TRAP::InPlace, {0, 1}, 2, 3);
        REQUIRE(o6->v[0] == 0);
        REQUIRE(o6->v[1] == 1);
        REQUIRE(std::get<0>(o6->t) == 2);
        REQUIRE(std::get<1>(o6->t) == 3);
    }

    SECTION("Make optional")
    {
        static constexpr auto o1 = TRAP::MakeOptional(42);
        static constexpr auto o2 = TRAP::Optional<i32>(42);

        static constexpr bool isSame = std::same_as<decltype(o1), const TRAP::Optional<i32>>;
        STATIC_REQUIRE(isSame);
        STATIC_REQUIRE(o1 == o2);

        static constexpr auto o3 = TRAP::MakeOptional<std::tuple<i32, i32, i32, i32>>(0, 1, 2, 3);
        STATIC_REQUIRE(std::get<0>(*o3) == 0);
        STATIC_REQUIRE(std::get<1>(*o3) == 1);
        STATIC_REQUIRE(std::get<2>(*o3) == 2);
        STATIC_REQUIRE(std::get<3>(*o3) == 3);

        auto o4 = TRAP::MakeOptional<std::vector<i32>>({0, 1, 2, 3});
        REQUIRE(o4.Value()[0] == 0);
        REQUIRE(o4.Value()[1] == 1);
        REQUIRE(o4.Value()[2] == 2);
        REQUIRE(o4.Value()[3] == 3);

        auto o5 = TRAP::MakeOptional<TakesInitAndVariadic>({0, 1}, 2, 3);
        REQUIRE(o5->v[0] == 0);
        REQUIRE(o5->v[1] == 1);
        REQUIRE(std::get<0>(o5->t) == 2);
        REQUIRE(std::get<1>(o5->t) == 3);

        REQUIRE(TRAP::MakeOptional(42) == 42);
    }

    SECTION("Noexcept")
    {
        TRAP::Optional<i32> o1{4};
        TRAP::Optional<i32> o2{42};

        //Comparison with nullopt
        {
            STATIC_REQUIRE(noexcept(o1 == TRAP::NullOpt));
            STATIC_REQUIRE(noexcept(TRAP::NullOpt == o1));
            STATIC_REQUIRE(noexcept(o1 != TRAP::NullOpt));
            STATIC_REQUIRE(noexcept(TRAP::NullOpt != o1));
            STATIC_REQUIRE(noexcept(o1 < TRAP::NullOpt));
            STATIC_REQUIRE(noexcept(TRAP::NullOpt < o1));
            STATIC_REQUIRE(noexcept(o1 <= TRAP::NullOpt));
            STATIC_REQUIRE(noexcept(TRAP::NullOpt <= o1));
            STATIC_REQUIRE(noexcept(o1 > TRAP::NullOpt));
            STATIC_REQUIRE(noexcept(TRAP::NullOpt > o1));
            STATIC_REQUIRE(noexcept(o1 >= TRAP::NullOpt));
            STATIC_REQUIRE(noexcept(TRAP::NullOpt >= o1));
        }

        //Swap
        {
            STATIC_REQUIRE(noexcept(std::swap(o1, o2)) == noexcept(o1.Swap(o2)));

            struct NoThrowSwappable
            {
                constexpr NoThrowSwappable& swap(const NoThrowSwappable&) noexcept
                {
                    return *this;
                }
            };

            struct ThrowSwappable
            {
                constexpr ThrowSwappable() = default;
                constexpr ThrowSwappable(const ThrowSwappable&)
                {}
                constexpr ThrowSwappable(ThrowSwappable&&)
                {}
                constexpr ThrowSwappable& swap(const ThrowSwappable&)
                {
                    return *this;
                }
            };

            TRAP::Optional<NoThrowSwappable> ont;
            TRAP::Optional<ThrowSwappable> ot;

            STATIC_REQUIRE(noexcept(ont.Swap(ont)));
            STATIC_REQUIRE(!noexcept(ot.Swap(ot)));
        }

        //Constructors
        {
            STATIC_REQUIRE(noexcept(TRAP::Optional<i32>{}));
            STATIC_REQUIRE(noexcept(TRAP::Optional<i32>{TRAP::NullOpt}));

            struct NoThrowMove
            {
                constexpr NoThrowMove(NoThrowMove&&) noexcept = default;
            };

            struct ThrowMove
            {
                constexpr ThrowMove(ThrowMove&&)
                {}
            };

            using NoThrowOpt = TRAP::Optional<NoThrowMove>;
            using ThrowOpt = TRAP::Optional<ThrowMove>;

            STATIC_REQUIRE(std::is_nothrow_move_constructible_v<NoThrowOpt>);
            STATIC_REQUIRE(!std::is_nothrow_move_constructible_v<ThrowOpt>);
        }

        //Assignment
        {
            STATIC_REQUIRE(noexcept(o1 == TRAP::NullOpt));

            struct NoThrowMoveAssign
            {
                constexpr NoThrowMoveAssign() = default;
                constexpr NoThrowMoveAssign(NoThrowMoveAssign&&) noexcept = default;
                constexpr NoThrowMoveAssign& operator=(const NoThrowMoveAssign&) = default;
            };

            struct ThrowMoveAssign
            {
                constexpr ThrowMoveAssign() = default;
                constexpr ThrowMoveAssign(ThrowMoveAssign&&)
                {}
                constexpr ThrowMoveAssign& operator=(const ThrowMoveAssign&)
                {
                    return *this;
                }
            };

            using NoThrowOpt = TRAP::Optional<NoThrowMoveAssign>;
            using ThrowOpt = TRAP::Optional<ThrowMoveAssign>;

            STATIC_REQUIRE(noexcept(std::declval<NoThrowOpt>() = std::declval<NoThrowOpt>()));
            STATIC_REQUIRE(!noexcept(std::declval<ThrowOpt>() = std::declval<ThrowOpt>()));
        }

        //Observers
        {
            STATIC_REQUIRE(noexcept(static_cast<bool>(o1)));
            STATIC_REQUIRE(noexcept(o1.HasValue()));
        }

        //Modifiers
        {
            STATIC_REQUIRE(noexcept(o1.Reset()));
        }
    }

    SECTION("Nullopt")
    {
        static constexpr TRAP::Optional<i32> o1 = TRAP::NullOpt;
        static constexpr TRAP::Optional<i32> o2{TRAP::NullOpt};
        static constexpr TRAP::Optional<i32> o3(TRAP::NullOpt);
        static constexpr TRAP::Optional<i32> o4 = {TRAP::NullOpt};

        STATIC_REQUIRE(!o1);
        STATIC_REQUIRE(!o2);
        STATIC_REQUIRE(!o3);
        STATIC_REQUIRE(!o4);

        STATIC_REQUIRE(!std::is_default_constructible_v<TRAP::NullOptT>);
    }

    SECTION("Observers")
    {
        struct MoveDetector
        {
            constexpr MoveDetector() = default;
            constexpr MoveDetector(MoveDetector&& rhs)
            {
                rhs.BeenMoved = true;
            }

            bool BeenMoved = false;
        };

        TRAP::Optional<i32> o1 = 42;
        static constexpr TRAP::Optional<i32> o2;
        static constexpr const TRAP::Optional<i32> o3 = 42;

        REQUIRE(*o1 == 42);
        REQUIRE(*o1 == o1.Value());
        STATIC_REQUIRE(o2.ValueOr(42) == 42);
        STATIC_REQUIRE(o3.Value() == 42);
        static constexpr auto success = std::same_as<decltype(o1.Value()), i32&>;
        STATIC_REQUIRE(success);
        static constexpr auto success1 = std::same_as<decltype(o3.Value()), const i32&>;
        STATIC_REQUIRE(success1);
        static constexpr auto success2 = std::same_as<decltype(std::move(o1).Value()), i32&&>;
        STATIC_REQUIRE(success2);
        static constexpr auto success3 = std::same_as<decltype(std::move(o3).Value()), const i32&&>;
        STATIC_REQUIRE(success3);

        TRAP::Optional<MoveDetector> o4{TRAP::InPlace};
        MoveDetector o5 = std::move(o4).Value();
        REQUIRE(o4->BeenMoved);
        REQUIRE(!o5.BeenMoved);
    }

    SECTION("Swap value")
    {
        TRAP::Optional<i32> o1 = 42;
        TRAP::Optional<i32> o2 = 12;
        o1.Swap(o2);

        REQUIRE(o1.Value() == 12);
        REQUIRE(o2.Value() == 42);
    }

    SECTION("Swap value with null initialized")
    {
        TRAP::Optional<i32> o1 = 42;
        TRAP::Optional<i32> o2 = TRAP::NullOpt;
        o1.Swap(o2);
        REQUIRE(!o1.HasValue());
        REQUIRE(o2.Value() == 42);

        TRAP::Optional<i32> o3(TRAP::NullOpt);
        TRAP::Optional<i32> o4(TRAP::NullOpt);
        o3.Swap(o4);
    }

    SECTION("Swap null initialized with value")
    {
        TRAP::Optional<i32> o1 = TRAP::NullOpt;
        TRAP::Optional<i32> o2 = 42;
        o1.Swap(o2);
        REQUIRE(o1.Value() == 42);
        REQUIRE(!o2.HasValue());
    }

    SECTION("Relational operations")
    {
        static constexpr TRAP::Optional<i32> o1{4};
        static constexpr TRAP::Optional<i32> o2{42};
        static constexpr TRAP::Optional<i32> o3{};

        //Self simple
        {
            STATIC_REQUIRE(!(o1 == o2));
            STATIC_REQUIRE(o1 == o1);
            STATIC_REQUIRE(o1 != o2);
            STATIC_REQUIRE(!(o1 != o1));
            STATIC_REQUIRE(o1 < o2);
            STATIC_REQUIRE(!(o1 < o1));
            STATIC_REQUIRE(!(o1 > o2));
            STATIC_REQUIRE(!(o1 > o1));
            STATIC_REQUIRE(o1 <= o2);
            STATIC_REQUIRE(o1 <= o1);
            STATIC_REQUIRE(!(o1 >= o2));
            STATIC_REQUIRE(o1 >= o1);
        }

        //nullopt simple
        {
            STATIC_REQUIRE(!(o1 == TRAP::NullOpt));
            STATIC_REQUIRE(!(TRAP::NullOpt == o1));
            STATIC_REQUIRE(o1 != TRAP::NullOpt);
            STATIC_REQUIRE(TRAP::NullOpt != o1);
            STATIC_REQUIRE(!(o1 < TRAP::NullOpt));
            STATIC_REQUIRE(TRAP::NullOpt < o1);
            STATIC_REQUIRE(o1 > TRAP::NullOpt);
            STATIC_REQUIRE(!(TRAP::NullOpt > o1));
            STATIC_REQUIRE(!(o1 <= TRAP::NullOpt));
            STATIC_REQUIRE(TRAP::NullOpt <= o1);
            STATIC_REQUIRE(o1 >= TRAP::NullOpt);
            STATIC_REQUIRE(!(TRAP::NullOpt >= o1));

            STATIC_REQUIRE(o3 == TRAP::NullOpt);
            STATIC_REQUIRE(TRAP::NullOpt == o3);
            STATIC_REQUIRE(!(o3 != TRAP::NullOpt));
            STATIC_REQUIRE(!(TRAP::NullOpt != o3));
            STATIC_REQUIRE(!(o3 < TRAP::NullOpt));
            STATIC_REQUIRE(!(TRAP::NullOpt < o3));
            STATIC_REQUIRE(!(o3 > TRAP::NullOpt));
            STATIC_REQUIRE(!(TRAP::NullOpt > o3));
            STATIC_REQUIRE(o3 <= TRAP::NullOpt);
            STATIC_REQUIRE(TRAP::NullOpt <= o3);
            STATIC_REQUIRE(o3 >= TRAP::NullOpt);
            STATIC_REQUIRE(TRAP::NullOpt >= o3);
        }

        //With T simple
        {
            STATIC_REQUIRE(!(o1 == 1));
            STATIC_REQUIRE(!(1 == o1));
            STATIC_REQUIRE(o1 != 1);
            STATIC_REQUIRE(1 != o1);
            STATIC_REQUIRE(!(o1 < 1));
            STATIC_REQUIRE(1 < o1);
            STATIC_REQUIRE(o1 > 1);
            STATIC_REQUIRE(!(1 > o1));
            STATIC_REQUIRE(!(o1 <= 1));
            STATIC_REQUIRE(1 <= o1);
            STATIC_REQUIRE(o1 >= 1);
            STATIC_REQUIRE(!(1 >= o1));

            STATIC_REQUIRE(o1 == 4);
            STATIC_REQUIRE(4 == o1);
            STATIC_REQUIRE(!(o1 != 4));
            STATIC_REQUIRE(!(4 != o1));
            STATIC_REQUIRE(!(o1 < 4));
            STATIC_REQUIRE(!(4 < o1));
            STATIC_REQUIRE(!(o1 > 4));
            STATIC_REQUIRE(!(4 > o1));
            STATIC_REQUIRE(o1 <= 4);
            STATIC_REQUIRE(4 <= o1);
            STATIC_REQUIRE(o1 >= 4);
            STATIC_REQUIRE(4 >= o1);
        }

        const TRAP::Optional<std::string> o4{"hello"};
        const TRAP::Optional<std::string> o5{"xyz"};

        //Self complex
        {
            REQUIRE(!(o4 == o5));
            REQUIRE(o4 == o4);
            REQUIRE(o4 != o5);
            REQUIRE(!(o4 != o4));
            REQUIRE(o4 < o5);
            REQUIRE(!(o4 < o4));
            REQUIRE(!(o4 > o5));
            REQUIRE(!(o4 > o4));
            REQUIRE(o4 <= o5);
            REQUIRE(o4 <= o4);
            REQUIRE(!(o4 >= o5));
            REQUIRE(o4 >= o4);
        }

        //Nullopt complex
        {
            REQUIRE(!(o4 == TRAP::NullOpt));
            REQUIRE(!(TRAP::NullOpt == o4));
            REQUIRE(o4 != TRAP::NullOpt);
            REQUIRE(TRAP::NullOpt != o4);
            REQUIRE(!(o4 < TRAP::NullOpt));
            REQUIRE(TRAP::NullOpt < o4);
            REQUIRE(o4 > TRAP::NullOpt);
            REQUIRE(!(TRAP::NullOpt > o4));
            REQUIRE(!(o4 <= TRAP::NullOpt));
            REQUIRE(TRAP::NullOpt <= o4);
            REQUIRE(o4 >= TRAP::NullOpt);
            REQUIRE(!(TRAP::NullOpt >= o4));

            STATIC_REQUIRE(o3 == TRAP::NullOpt);
            STATIC_REQUIRE(TRAP::NullOpt == o3);
            STATIC_REQUIRE(!(o3 != TRAP::NullOpt));
            STATIC_REQUIRE(!(TRAP::NullOpt != o3));
            STATIC_REQUIRE(!(o3 < TRAP::NullOpt));
            STATIC_REQUIRE(!(TRAP::NullOpt < o3));
            STATIC_REQUIRE(!(o3 > TRAP::NullOpt));
            STATIC_REQUIRE(!(TRAP::NullOpt > o3));
            STATIC_REQUIRE(o3 <= TRAP::NullOpt);
            STATIC_REQUIRE(TRAP::NullOpt <= o3);
            STATIC_REQUIRE(o3 >= TRAP::NullOpt);
            STATIC_REQUIRE(TRAP::NullOpt >= o3);
        }

        //With T complex
        {
            REQUIRE(!(o4 == "a"));
            REQUIRE(!("a" == o4));
            REQUIRE(o4 != "a");
            REQUIRE("a" != o4);
            REQUIRE(!(o4 < "a"));
            REQUIRE("a" < o4);
            REQUIRE(o4 > "a");
            REQUIRE(!("a" > o4));
            REQUIRE(!(o4 <= "a"));
            REQUIRE("a" <= o4);
            REQUIRE(o4 >= "a");
            REQUIRE(!("a" >= o4));

            REQUIRE(o4 == "hello");
            REQUIRE("hello" == o4);
            REQUIRE(!(o4 != "hello"));
            REQUIRE(!("hello" != o4));
            REQUIRE(!(o4 < "hello"));
            REQUIRE(!("hello" < o4));
            REQUIRE(!(o4 > "hello"));
            REQUIRE(!("hello" > o4));
            REQUIRE(o4 <= "hello");
            REQUIRE("hello" <= o4);
            REQUIRE(o4 >= "hello");
            REQUIRE("hello" >= o4);
        }
    }

    SECTION("Transform")
    {
        //lhs is empty
        static constexpr TRAP::Optional<i32> o1;
        static constexpr auto o1r = o1.Transform([](const i32 i){return i + 2;});
        STATIC_REQUIRE((std::same_as<decltype(o1r), const TRAP::Optional<i32>>));
        STATIC_REQUIRE(!o1r);

        //lhs has value
        static constexpr TRAP::Optional<i32> o2 = 40;
        static constexpr auto o2r = o2.Transform([](const i32 i){return i + 2;});
        STATIC_REQUIRE((std::same_as<decltype(o2r), const TRAP::Optional<i32>>));
        STATIC_REQUIRE(o2r.Value() == 42);

        struct RValCallTransform
        {
            constexpr f64 operator()([[maybe_unused]] const i32 _) && noexcept
            {
                return 42.0;
            }
        };

        //Ensure that function object is forwarded
        static constexpr TRAP::Optional<i32> o3 = 42;
        static constexpr auto o3r = o3.Transform(RValCallTransform{});
        STATIC_REQUIRE((std::same_as<decltype(o3r), const TRAP::Optional<f64>>));
        STATIC_REQUIRE(o3r.Value() == 42);

        //Ensure that lhs is forwarded
        static constexpr TRAP::Optional<i32> o4 = 40;
        static constexpr auto o4r = std::move(o4).Transform([](const i32&& i){return i + 2;});
        STATIC_REQUIRE((std::same_as<decltype(o4r), const TRAP::Optional<i32>>));
        STATIC_REQUIRE(o4r.Value() == 42);

        //Ensure that lhs is const-propagated
        static constexpr TRAP::Optional<i32> o5 = 40;
        static constexpr auto o5r = o5.Transform([](const i32& i){return i + 2;});
        STATIC_REQUIRE((std::same_as<decltype(o5r), const TRAP::Optional<i32>>));
        STATIC_REQUIRE(o5r.Value() == 42);

        //Test each overload in turn
        static constexpr TRAP::Optional<i32> o8 = 42;
        static constexpr auto o8r = o8.Transform([](i32) { return 42; });
        STATIC_REQUIRE(*o8r == 42);

        static constexpr TRAP::Optional<i32> o12 = 42;
        static constexpr auto o12r = std::move(o12).Transform([](i32) { return 42; });
        STATIC_REQUIRE(*o12r == 42);

        static constexpr TRAP::Optional<i32> o24 = TRAP::NullOpt;
        static constexpr auto o24r = o24.Transform([](i32) { return 42; });
        STATIC_REQUIRE(!o24r);

        static constexpr TRAP::Optional<i32> o28 = TRAP::NullOpt;
        static constexpr auto o28r = std::move(o28).Transform([](i32) { return 42; });
        STATIC_REQUIRE(!o28r);

        //Callable which returns a reference
        TRAP::Optional<i32> o38 = 42;
        auto o38r = o38.Transform([](i32& i) -> const i32& {return i;});
        REQUIRE(o38r);
        REQUIRE(*o38r == 42);
    }

    SECTION("AndThen")
    {
        //lhs is empty
        static constexpr TRAP::Optional<i32> o1;
        static constexpr auto o1r = o1.AndThen([](i32) { return TRAP::Optional<f32>{42.0f}; });
        STATIC_REQUIRE((std::same_as<decltype(o1r), const TRAP::Optional<f32>>));
        STATIC_REQUIRE(!o1r);

        //lhs has value
        static constexpr TRAP::Optional<i32> o2 = 12;
        static constexpr auto o2r = o2.AndThen([](i32) { return TRAP::Optional<f32>{42.0f}; });
        STATIC_REQUIRE((std::same_as<decltype(o2r), const TRAP::Optional<f32>>));
        STATIC_REQUIRE(o2r.Value() == 42.f);

        //lhs is empty, rhs returns empty
        static constexpr TRAP::Optional<i32> o3;
        static constexpr auto o3r = o3.AndThen([](i32) { return TRAP::Optional<f32>{}; });
        STATIC_REQUIRE((std::same_as<decltype(o3r), const TRAP::Optional<f32>>));
        STATIC_REQUIRE(!o3r);

        //rhs returns empty
        static constexpr TRAP::Optional<i32> o4 = 12;
        static constexpr auto o4r = o4.AndThen([](i32) { return TRAP::Optional<f32>{}; });
        STATIC_REQUIRE((std::same_as<decltype(o4r), const TRAP::Optional<f32>>));
        STATIC_REQUIRE(!o4r);

        struct RValCallAndThen
        {
            constexpr TRAP::Optional<f64> operator()(i32) &&
            {
                return TRAP::Optional<f64>(42.0);
            };
        };

        //Ensure that function object is forwarded
        static constexpr TRAP::Optional<i32> o5 = 42;
        static constexpr auto o5r = o5.AndThen(RValCallAndThen{});
        STATIC_REQUIRE((std::same_as<decltype(o5r), const TRAP::Optional<f64>>));
        STATIC_REQUIRE(o5r.Value() == 42);

        //Ensure that lhs is forwarded
        TRAP::Optional<i32> o6 = 42;
        auto o6r = std::move(o6).AndThen([](i32&& i) { return TRAP::Optional<f64>(i); });
        STATIC_REQUIRE((std::same_as<decltype(o6r), TRAP::Optional<f64>>));
        REQUIRE(o6r.Value() == 42);

        //Ensure that function object is const-propagated
        static constexpr TRAP::Optional<i32> o7 = 42;
        static constexpr auto o7r = o7.AndThen([](const i32& i) { return TRAP::Optional<f64>(i); });
        STATIC_REQUIRE((std::same_as<decltype(o7r), const TRAP::Optional<f64>>));
        STATIC_REQUIRE(o7r.Value() == 42);

        //Test each overload in turn
        static constexpr TRAP::Optional<i32> o8 = 42;
        static constexpr auto o8r = o8.AndThen([](i32) { return TRAP::MakeOptional(42); });
        STATIC_REQUIRE(*o8r == 42);

        static constexpr TRAP::Optional<i32> o9 = 42;
        static constexpr auto o9r = std::move(o9).AndThen([](i32) { return TRAP::MakeOptional(42); });
        STATIC_REQUIRE(*o9r == 42);

        static constexpr TRAP::Optional<i32> o16 = TRAP::NullOpt;
        static constexpr auto o16r = o16.AndThen([](i32) { return TRAP::MakeOptional(42); });
        STATIC_REQUIRE(!o16r);

        static constexpr TRAP::Optional<i32> o17 = TRAP::NullOpt;
        static constexpr auto o17r = std::move(o17).AndThen([](i32) { return TRAP::MakeOptional(42); });
        STATIC_REQUIRE(!o17r);

        TRAP::Optional<i32> o21(TRAP::NullOpt);
        REQUIRE(!std::move(o21).AndThen([](i32){return TRAP::MakeOptional(42);}).HasValue());
    }

    SECTION("OrElse")
    {
        static constexpr TRAP::Optional<i32> o1 = 42;
        STATIC_REQUIRE(*(o1.OrElse([]{return TRAP::MakeOptional(13);})) == 42);

        static constexpr TRAP::Optional<i32> o2;
        STATIC_REQUIRE(*(o2.OrElse([]{return TRAP::MakeOptional(13);})) == 13);
    }

    SECTION("ValueOrElse")
    {
        static constexpr TRAP::Optional<i32> o1 = 42;
        STATIC_REQUIRE(o1.ValueOrElse([]{return 5;}) == 42);

        i32 i = 42;
        TRAP::Optional<i32&> o2 = i;
        REQUIRE(o2.ValueOrElse([]{return 5;}) == 42);

        static constexpr TRAP::Optional<i32> o3;
        STATIC_REQUIRE(o3.ValueOrElse([]{return 5;}) == 5);

        const TRAP::Optional<i32&> o4{TRAP::NullOpt};
        REQUIRE(o4.ValueOrElse([]{return 5;}) == 5);
    }

    SECTION("TransformOr")
    {
        static constexpr TRAP::Optional<i32> o1 = 21;
        STATIC_REQUIRE((o1.TransformOr([](i32 x){return x * 2;}, 13)) == 42);

        static constexpr TRAP::Optional<i32> o2;
        STATIC_REQUIRE((o2.TransformOr([](i32 x){return x * 2;}, 13)) == 13);
    }

    SECTION("TransformOrElse")
    {
        static constexpr TRAP::Optional<i32> o1 = 21;
        STATIC_REQUIRE((o1.TransformOrElse([](i32 x){return x * 2;}, []{return 13;})) == 42);

        static constexpr TRAP::Optional<i32> o2;
        STATIC_REQUIRE((o2.TransformOrElse([](i32 x){return x * 2;}, []{return 13;})) == 13);
    }

    SECTION("Take")
    {
        TRAP::Optional<i32> o1 = 42;
        REQUIRE(*o1.Take() == 42);
        REQUIRE(!o1);

        TRAP::Optional<i32> o2;
        REQUIRE(!o2.Take());
        REQUIRE(!o2);
    }

    SECTION("BadOptionalAccess")
    {
        using namespace std::string_view_literals;

        const TRAP::BadOptionalAccess boa{};
        REQUIRE("BadOptionalAccess in TRAP::Optional"sv == boa.what());
    }
}

namespace
{
    struct Empty
    {
    };

    struct NoDefault
    {
        constexpr NoDefault() = delete;
        constexpr NoDefault(const NoDefault&) = default;
        constexpr NoDefault(NoDefault&&) = default;
        constexpr NoDefault& operator=(const NoDefault&) = default;
        constexpr NoDefault& operator=(NoDefault&&) = default;
        constexpr NoDefault(Empty)
        {
        }
    };

    struct Base
    {
        i32 I;

        constexpr Base()
            : I(0)
        {
        }

        constexpr Base(i32 i)
            : I(i)
        {
        }
    };

    struct Derived : public Base
    {
        i32 J;

        constexpr Derived()
            : Base(0), J(0)
        {
        }

        constexpr Derived(i32 i, i32 j)
            : Base(i), J(j)
        {
        }
    };

    static_assert(std::is_default_constructible_v<Derived>);

    struct MoveDetector
    {
        constexpr MoveDetector() = default;
        constexpr MoveDetector(MoveDetector&& rhs)
        {
            rhs.BeenMoved = true;
        }
        bool BeenMoved = false;
    };
}

TEST_CASE("TRAP::Optional<T&>", "[utils][optional]")
{
    SECTION("Constructors")
    {
        STATIC_REQUIRE(std::is_same_v<TRAP::Optional<i32&>::value_type, i32&>);

        static constexpr TRAP::Optional<i32&> i1;
        static constexpr TRAP::Optional<i32&> i2{TRAP::NullOpt};

        i32 i = 0;
        const TRAP::Optional<i32&> i3{i};

        static constexpr TRAP::Optional<Empty&> e1;
        static constexpr TRAP::Optional<Empty&> e2{TRAP::NullOpt};

        Empty e{};
        const TRAP::Optional<Empty&> e3{e};

        static constexpr TRAP::Optional<NoDefault&> nd1;
        static constexpr TRAP::Optional<NoDefault&> nd2{TRAP::NullOpt};

        NoDefault nd{e};

        const TRAP::Optional<NoDefault&> nd3{nd};

        static constexpr TRAP::Optional<i32&> ie;
        static constexpr TRAP::Optional<i32&> i4 = ie;
        REQUIRE_FALSE(i4);

        Base b{1};
        Derived d(1, 2);
        const TRAP::Optional<Base&> b1{b};
        const TRAP::Optional<Base&> b2{d};

        const TRAP::Optional<Derived&> d2{d};
        const TRAP::Optional<Base&> b3{d2};
        const TRAP::Optional<Base&> b4 = d2;

        static constexpr TRAP::Optional<Derived&> empty;
        static constexpr TRAP::Optional<Base&> fromEmpty{empty};
        static constexpr TRAP::Optional<Base&> fromEmpty2 = empty;
    }

    SECTION("Assignment")
    {
        TRAP::Optional<i32&> i1;
        REQUIRE_FALSE(i1);
        i32 i = 5;
        i1 = i;
        i = 7;
        REQUIRE(i1);
        REQUIRE((*i1 = 7));

        f64 d;
        // i1 = d; //ill-formed by mandate
        const TRAP::Optional<f64&> d1{d};
        // i1 = d1; //ill-formed by mandate
        TRAP::Optional<i32&> i2 = i1;
        REQUIRE(i2);
        REQUIRE((*i2 = 7));

        TRAP::Optional<i32&> empty;
        REQUIRE_FALSE(empty);
        i2 = empty;
        REQUIRE_FALSE(i2);
        i32 eight = 8;
        empty.Emplace(eight);
        REQUIRE(empty);
        // REQUIRE(empty == 8);
    }

    SECTION("RelationalOps")
    {
        i32 i1 = 4;
        i32 i2 = 42;
        const TRAP::Optional<i32&> o1{i1};
        const TRAP::Optional<i32&> o2{i2};
        static constexpr TRAP::Optional<i32&> o3{};

        REQUIRE(!(o1 == o2));
        REQUIRE(o1 == o1);
        REQUIRE(o1 != o2);
        REQUIRE(!(o1 != o1));
        REQUIRE(o1 < o2);
        REQUIRE(!(o1 < o1));
        REQUIRE(!(o1 > o2));
        REQUIRE(!(o1 > o1));
        REQUIRE(o1 <= o2);
        REQUIRE(o1 <= o1);
        REQUIRE(!(o1 >= o2));
        REQUIRE(o1 >= o1);

        REQUIRE(!(o1 == TRAP::NullOpt));
        REQUIRE(!(TRAP::NullOpt == o1));
        REQUIRE(o1 != TRAP::NullOpt);
        REQUIRE(TRAP::NullOpt != o1);
        REQUIRE(!(o1 < TRAP::NullOpt));
        REQUIRE(TRAP::NullOpt < o1);
        REQUIRE(o1 > TRAP::NullOpt);
        REQUIRE(!(TRAP::NullOpt > o1));
        REQUIRE(!(o1 <= TRAP::NullOpt));
        REQUIRE(TRAP::NullOpt <= o1);
        REQUIRE(o1 >= TRAP::NullOpt);
        REQUIRE(!(TRAP::NullOpt >= o1));

        REQUIRE(o3 == TRAP::NullOpt);
        REQUIRE(TRAP::NullOpt == o3);
        REQUIRE(!(o3 != TRAP::NullOpt));
        REQUIRE(!(TRAP::NullOpt != o3));
        REQUIRE(!(o3 < TRAP::NullOpt));
        REQUIRE(!(TRAP::NullOpt < o3));
        REQUIRE(!(o3 > TRAP::NullOpt));
        REQUIRE(!(TRAP::NullOpt > o3));
        REQUIRE(o3 <= TRAP::NullOpt);
        REQUIRE(TRAP::NullOpt <= o3);
        REQUIRE(o3 >= TRAP::NullOpt);
        REQUIRE(TRAP::NullOpt >= o3);

        REQUIRE(!(o1 == 1));
        REQUIRE(!(1 == o1));
        REQUIRE(o1 != 1);
        REQUIRE(1 != o1);
        REQUIRE(!(o1 < 1));
        REQUIRE(1 < o1);
        REQUIRE(o1 > 1);
        REQUIRE(!(1 > o1));
        REQUIRE(!(o1 <= 1));
        REQUIRE(1 <= o1);
        REQUIRE(o1 >= 1);
        REQUIRE(!(1 >= o1));

        REQUIRE(o1 == 4);
        REQUIRE(4 == o1);
        REQUIRE(!(o1 != 4));
        REQUIRE(!(4 != o1));
        REQUIRE(!(o1 < 4));
        REQUIRE(!(4 < o1));
        REQUIRE(!(o1 > 4));
        REQUIRE(!(4 > o1));
        REQUIRE(o1 <= 4);
        REQUIRE(4 <= o1);
        REQUIRE(o1 >= 4);
        REQUIRE(4 >= o1);

        std::string s4 = "hello";
        std::string s5 = "xyz";
        const TRAP::Optional<std::string&> o4{s4};
        const TRAP::Optional<std::string&> o5{s5};

        REQUIRE(!(o4 == o5));
        REQUIRE(o4 == o4);
        REQUIRE(o4 != o5);
        REQUIRE(!(o4 != o4));
        REQUIRE(o4 < o5);
        REQUIRE(!(o4 < o4));
        REQUIRE(!(o4 > o5));
        REQUIRE(!(o4 > o4));
        REQUIRE(o4 <= o5);
        REQUIRE(o4 <= o4);
        REQUIRE(!(o4 >= o5));
        REQUIRE(o4 >= o4);

        REQUIRE(!(o4 == TRAP::NullOpt));
        REQUIRE(!(TRAP::NullOpt == o4));
        REQUIRE(o4 != TRAP::NullOpt);
        REQUIRE(TRAP::NullOpt != o4);
        REQUIRE(!(o4 < TRAP::NullOpt));
        REQUIRE(TRAP::NullOpt < o4);
        REQUIRE(o4 > TRAP::NullOpt);
        REQUIRE(!(TRAP::NullOpt > o4));
        REQUIRE(!(o4 <= TRAP::NullOpt));
        REQUIRE(TRAP::NullOpt <= o4);
        REQUIRE(o4 >= TRAP::NullOpt);
        REQUIRE(!(TRAP::NullOpt >= o4));

        REQUIRE(o3 == TRAP::NullOpt);
        REQUIRE(TRAP::NullOpt == o3);
        REQUIRE(!(o3 != TRAP::NullOpt));
        REQUIRE(!(TRAP::NullOpt != o3));
        REQUIRE(!(o3 < TRAP::NullOpt));
        REQUIRE(!(TRAP::NullOpt < o3));
        REQUIRE(!(o3 > TRAP::NullOpt));
        REQUIRE(!(TRAP::NullOpt > o3));
        REQUIRE(o3 <= TRAP::NullOpt);
        REQUIRE(TRAP::NullOpt <= o3);
        REQUIRE(o3 >= TRAP::NullOpt);
        REQUIRE(TRAP::NullOpt >= o3);

        REQUIRE(!(o4 == "a"));
        REQUIRE(!("a" == o4));
        REQUIRE(o4 != "a");
        REQUIRE("a" != o4);
        REQUIRE(!(o4 < "a"));
        REQUIRE("a" < o4);
        REQUIRE(o4 > "a");
        REQUIRE(!("a" > o4));
        REQUIRE(!(o4 <= "a"));
        REQUIRE("a" <= o4);
        REQUIRE(o4 >= "a");
        REQUIRE(!("a" >= o4));

        REQUIRE(o4 == "hello");
        REQUIRE("hello" == o4);
        REQUIRE(!(o4 != "hello"));
        REQUIRE(!("hello" != o4));
        REQUIRE(!(o4 < "hello"));
        REQUIRE(!("hello" < o4));
        REQUIRE(!(o4 > "hello"));
        REQUIRE(!("hello" > o4));
        REQUIRE(o4 <= "hello");
        REQUIRE("hello" <= o4);
        REQUIRE(o4 >= "hello");
        REQUIRE("hello" >= o4);
    }

    SECTION("Triviality")
    {
        STATIC_REQUIRE(std::is_trivially_copy_constructible_v<TRAP::Optional<i32&>>);
        STATIC_REQUIRE(std::is_trivially_copy_assignable_v<TRAP::Optional<i32&>>);
        STATIC_REQUIRE(std::is_trivially_move_constructible_v<TRAP::Optional<i32&>>);
        STATIC_REQUIRE(std::is_trivially_move_assignable_v<TRAP::Optional<i32&>>);
        STATIC_REQUIRE(std::is_trivially_destructible_v<TRAP::Optional<i32&>>);

        {
            struct T
            {
                constexpr T(const T&) = default;
                constexpr T(T&&) = default;
                T& operator=(const T&) = default;
                T& operator=(T&&) = default;
                constexpr ~T() = default;
            };
            STATIC_REQUIRE(std::is_trivially_copy_constructible_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_trivially_copy_assignable_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_trivially_move_constructible_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_trivially_move_assignable_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_trivially_destructible_v<TRAP::Optional<T&>>);
        }

        {
            struct T
            {
                constexpr T(const T&)
                {
                };
                constexpr T(T&&)
                {
                };
                constexpr T& operator=(const T&)
                {
                    return *this;
                };
                constexpr T& operator=(T&&)
                {
                    return *this;
                };
                constexpr ~T()
                {
                };
            };
            STATIC_REQUIRE(std::is_trivially_copy_constructible_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_trivially_copy_assignable_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_trivially_move_constructible_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_trivially_move_assignable_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_trivially_destructible_v<TRAP::Optional<T&>>);
        }
    }

    SECTION("Deletion")
    {
        STATIC_REQUIRE(std::is_copy_constructible_v<TRAP::Optional<i32&>>);
        STATIC_REQUIRE(std::is_copy_assignable_v<TRAP::Optional<i32&>>);
        STATIC_REQUIRE(std::is_move_constructible_v<TRAP::Optional<i32&>>);
        STATIC_REQUIRE(std::is_move_assignable_v<TRAP::Optional<i32&>>);
        STATIC_REQUIRE(std::is_destructible_v<TRAP::Optional<i32&>>);

        {
            struct T
            {
                constexpr T(const T&) = default;
                constexpr T(T&&) = default;
                T& operator=(const T&) = default;
                T& operator=(T&&) = default;
                constexpr ~T() = default;
            };
            STATIC_REQUIRE(std::is_copy_constructible_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_copy_assignable_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_move_constructible_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_move_assignable_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_destructible_v<TRAP::Optional<T&>>);
        }

        {
            struct T
            {
                constexpr T(const T&) = delete;
                constexpr T(T&&) = delete;
                constexpr T& operator=(const T&) = delete;
                constexpr T& operator=(T&&) = delete;
            };
            STATIC_REQUIRE(std::is_copy_constructible_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_copy_assignable_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_move_constructible_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_move_assignable_v<TRAP::Optional<T&>>);
        }

        {
            struct T
            {
                constexpr T(const T&) = delete;
                constexpr T(T&&) = default;
                constexpr T& operator=(const T&) = delete;
                T& operator=(T&&) = default;
            };
            STATIC_REQUIRE(std::is_copy_constructible_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_copy_assignable_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_move_constructible_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_move_assignable_v<TRAP::Optional<T&>>);
        }

        {
            struct T
            {
                constexpr T(const T&) = default;
                constexpr T(T&&) = delete;
                T& operator=(const T&) = default;
                constexpr T& operator=(T&&) = delete;
            };
            STATIC_REQUIRE(std::is_copy_constructible_v<TRAP::Optional<T&>>);
            STATIC_REQUIRE(std::is_copy_assignable_v<TRAP::Optional<T&>>);
        }
    }

    SECTION("MakeOptional")
    {
        i32 var{42};
        const auto o1 = TRAP::MakeOptional([&]()->i32&{return var;}());
        const auto o2 = TRAP::Optional<i32&>(var);

        STATIC_REQUIRE(std::same_as<decltype(o1), const TRAP::Optional<i32>>);
        REQUIRE(*o1 == *o2);

        std::tuple<i32, i32, i32, i32> tvar{0, 1, 2, 3};
        const auto o3 = TRAP::MakeOptional<std::tuple<i32, i32, i32, i32>&>(tvar);
        REQUIRE(std::get<0>(*o3) == 0);
        REQUIRE(std::get<1>(*o3) == 1);
        REQUIRE(std::get<2>(*o3) == 2);
        REQUIRE(std::get<3>(*o3) == 3);

        std::vector<i32> ivec{0, 1, 2, 3};
        const auto o4 = TRAP::MakeOptional<std::vector<i32>&>(ivec);
        REQUIRE(o4.Value()[0] == 0);
        REQUIRE(o4.Value()[1] == 1);
        REQUIRE(o4.Value()[2] == 2);
        REQUIRE(o4.Value()[3] == 3);

        TakesInitAndVariadic tiv{{0, 1}, 2, 3};
        const auto o5 = TRAP::MakeOptional<TakesInitAndVariadic&>(tiv);
        REQUIRE(o5->v[0] == 0);
        REQUIRE(o5->v[1] == 1);
        REQUIRE(std::get<0>(o5->t) == 2);
        REQUIRE(std::get<1>(o5->t) == 3);

        auto i = 42;
        static const auto o6 = TRAP::MakeOptional<i32&>(i);

        STATIC_REQUIRE(std::same_as<decltype(o6), const TRAP::Optional<i32>>);
        REQUIRE(o6);
        REQUIRE((*o6 == 42));
    }

    SECTION("NullOpt")
    {
        static constexpr TRAP::Optional<i32&> o1 = TRAP::NullOpt;
        static constexpr TRAP::Optional<i32&> o2{TRAP::NullOpt};
        static constexpr TRAP::Optional<i32&> o3{TRAP::NullOpt};
        static constexpr TRAP::Optional<i32&> o4 = {TRAP::NullOpt};

        STATIC_REQUIRE(!o1);
        STATIC_REQUIRE(!o2);
        STATIC_REQUIRE(!o3);
        STATIC_REQUIRE(!o4);

        STATIC_REQUIRE(!std::is_default_constructible_v<TRAP::NullOptT>);
    }

    SECTION("Observers")
    {
        i32 var = 42;
        TRAP::Optional<i32&> o1{var};
        static constexpr TRAP::Optional<i32&> o2;
        const TRAP::Optional<i32&> o3{var};
        i32 fortyTwo = 42;
        REQUIRE(*o1 == 42);
        REQUIRE(*o1 == o1.Value());
        REQUIRE(o2.ValueOr(fortyTwo) == 42);
        REQUIRE(o3.Value() == 42);
        STATIC_REQUIRE(std::same_as<decltype(o1.Value()), i32&>);
        STATIC_REQUIRE(std::same_as<decltype(o2.Value()), const i32&>);
        STATIC_REQUIRE(std::same_as<decltype(o3.Value()), const i32&>);
        STATIC_REQUIRE(std::same_as<decltype(std::move(o1).Value()), i32&>);

        STATIC_REQUIRE(std::same_as<decltype(*o1), i32&>);
        STATIC_REQUIRE(std::same_as<decltype(*o2), i32&>);
        STATIC_REQUIRE(std::same_as<decltype(*o3), i32&>);
        STATIC_REQUIRE(std::same_as<decltype(*std::move(o1)), i32&>);

        STATIC_REQUIRE(std::same_as<decltype(o1.operator->()), i32*>);
        STATIC_REQUIRE(std::same_as<decltype(o2.operator->()), const i32*>);
        STATIC_REQUIRE(std::same_as<decltype(o3.operator->()), const i32*>);
        STATIC_REQUIRE(std::same_as<decltype(std::move(o1).operator->()), i32*>);

        struct IntBox
        {
            i32 I;
        };
        IntBox i1{3};
        TRAP::Optional<IntBox&> ob1{i1};
        static constexpr TRAP::Optional<IntBox&> ob2;
        const TRAP::Optional<IntBox&> ob3{i1};
        STATIC_REQUIRE(std::same_as<decltype(ob1->I), i32>);
        STATIC_REQUIRE(std::same_as<decltype(ob2->I), i32>);
        STATIC_REQUIRE(std::same_as<decltype(ob3->I), i32>);
        STATIC_REQUIRE(std::same_as<decltype(std::move(ob1)->I), i32>);
    }

    SECTION("Move check")
    {
        i32 x = 0;
        const i32& y = std::move(TRAP::Optional<i32&>(x)).Value();
        REQUIRE((&y == &x));

        const i32& z = *std::move(TRAP::Optional<i32&>(x));
        REQUIRE((&z == &x));
    }

    SECTION("Swap value")
    {
        i32 var = 42;
        i32 twelve = 12;
        TRAP::Optional<i32&> o1{var};
        TRAP::Optional<i32&> o2{twelve};
        o1.Swap(o2);
        REQUIRE(o1.Value() == 12);
        REQUIRE(o2.Value() == 42);
    }

    SECTION("Swap with NullOpt")
    {
        i32 var = 42;

        TRAP::Optional<i32&> o1{var};
        TRAP::Optional<i32&> o2{TRAP::NullOpt};
        o1.Swap(o2);
        REQUIRE(!o1.HasValue());
        REQUIRE(o2.Value() == 42);
    }

    SECTION("Swap NullOpt with value")
    {
        i32 var = 42;
        TRAP::Optional<i32&> o1{TRAP::NullOpt};
        TRAP::Optional<i32&> o2{var};
        o1.Swap(o2);
        REQUIRE(o1.Value() == 42);
        REQUIRE(!o2.HasValue());
    }

    SECTION("Monadic Transform")
    {
        TRAP::Optional<i32&> o1;
        auto o1r = o1.Transform([](i32 i){return i + 2; });
        STATIC_REQUIRE(std::same_as<decltype(o1r), TRAP::Optional<i32>>);
        REQUIRE(!o1r);

        i32 forty = 40;
        TRAP::Optional<i32&> o2 = forty;
        auto o2r = o2.Transform([](i32 i){return i + 2; });
        STATIC_REQUIRE(std::same_as<decltype(o2r), TRAP::Optional<i32>>);
        REQUIRE(o2r.Value() == 42);

        struct RValCallTransform
        {
            [[nodiscard]] constexpr f64 operator()(i32) &&
            {
                return 42.0;
            }
        };

        i32 fortyTwo = 42;
        TRAP::Optional<i32&> o3 = fortyTwo;
        auto o3r = o3.Transform(RValCallTransform{});
        STATIC_REQUIRE(std::same_as<decltype(o3r), TRAP::Optional<f64>>);
        REQUIRE(o3r.Value() == 42);

        forty = 40;
        const TRAP::Optional<i32&> o5 = forty;
        auto o5r = o5.Transform([](const i32& i){return i + 2;});
        STATIC_REQUIRE(std::same_as<decltype(o5r), TRAP::Optional<i32>>);
        REQUIRE(o5r.Value() == 42);

        fortyTwo = 42;
        TRAP::Optional<i32&> o8 = fortyTwo;
        auto o8r = o8.Transform([](i32){return 42;});
        REQUIRE(*o8r == 42);

        TRAP::Optional<i32&> o12 = fortyTwo;
        auto o12r = std::move(o12).Transform([](i32){return 42;});
        REQUIRE(*o12r == 42);

        const TRAP::Optional<i32&> o16 = fortyTwo;
        auto o16r = o16.Transform([](i32){return 42;});
        REQUIRE(*o16r == 42);

        const TRAP::Optional<i32&> o20 = fortyTwo;
        auto o20r = std::move(o20).Transform([](i32){return 42;});
        REQUIRE(*o20r == 42);

        TRAP::Optional<i32&> o24 = TRAP::NullOpt;
        auto o24r = o24.Transform([](i32){return 42;});
        REQUIRE(!o24r);

        TRAP::Optional<i32&> o28 = TRAP::NullOpt;
        auto o28r = std::move(o28).Transform([](i32){return 42;});
        REQUIRE(!o28r);

        const TRAP::Optional<i32&> o32 = TRAP::NullOpt;
        auto o32r = o32.Transform([](i32){return 42;});
        REQUIRE(!o32r);

        const TRAP::Optional<i32&> o36 = TRAP::NullOpt;
        auto o36r = std::move(o36).Transform([](i32){return 42;});
        REQUIRE(!o36r);

        TRAP::Optional<i32&> o38 = fortyTwo;
        auto o38r = o38.Transform([](i32& i) -> const i32&{return i;});
        REQUIRE(o38r);
        REQUIRE(*o38r == 42);
    }

    SECTION("Monadic Transform constexpr")
    {
        static constexpr i32 constexprFortyTwo = 42;
        static constexpr auto GetInt = [](i32){return 42;};

        static constexpr TRAP::Optional<const i32&> o16 = constexprFortyTwo;
        static constexpr auto o16r = o16.Transform(GetInt);

        static constexpr TRAP::Optional<const i32&> o20 = constexprFortyTwo;
        static constexpr auto o20r = std::move(o20).Transform(GetInt);

        static constexpr TRAP::Optional<i32&> o32 = TRAP::NullOpt;
        static constexpr auto o32r = o32.Transform(GetInt);
        STATIC_REQUIRE(!o32r);
        static constexpr TRAP::Optional<i32&> o36 = TRAP::NullOpt;
        static constexpr auto o36r = std::move(o36).Transform(GetInt);
        STATIC_REQUIRE(!o36r);
    }

    SECTION("Monadic AndThen")
    {
        TRAP::Optional<i32&> o1;
        auto o1r = o1.AndThen([](i32){return TRAP::Optional<f32>{42.0f};});
        STATIC_REQUIRE(std::same_as<decltype(o1r), TRAP::Optional<f32>>);
        REQUIRE(!o1r);

        i32 twelve = 12;
        TRAP::Optional<i32&> o2 = twelve;
        auto o2r = o2.AndThen([](i32){return TRAP::Optional<f32>{42.0f};});
        STATIC_REQUIRE(std::same_as<decltype(o2r), TRAP::Optional<f32>>);
        REQUIRE(o2r.Value() == 42.0f);

        TRAP::Optional<i32&> o3;
        auto o3r = o3.AndThen([](i32){return TRAP::Optional<f32>{};});
        STATIC_REQUIRE(std::same_as<decltype(o3r), TRAP::Optional<f32>>);
        REQUIRE(!o3r);

        TRAP::Optional<i32&> o4 = twelve;
        auto o4r = o4.AndThen([](i32){return TRAP::Optional<f32>{};});
        STATIC_REQUIRE(std::same_as<decltype(o4r), TRAP::Optional<f32>>);
        REQUIRE(!o4r);

        struct RValCallAndThen
        {
            [[nodiscard]] constexpr TRAP::Optional<f64> operator()(i32) &&
            {
                return TRAP::Optional<f64>(42.0);
            }
        };

        i32 fortyTwo = 42;
        TRAP::Optional<i32&> o5 = fortyTwo;
        auto o5r = o5.AndThen(RValCallAndThen{});
        STATIC_REQUIRE(std::same_as<decltype(o5r), TRAP::Optional<f64>>);
        REQUIRE(o5r.Value() == 42);

        const TRAP::Optional<i32&> o7 = fortyTwo;
        auto o7r = o7.AndThen([](const i32& i){return TRAP::Optional<f64>(i);});
        STATIC_REQUIRE(std::same_as<decltype(o7r), TRAP::Optional<f64>>);
        REQUIRE(o7r.Value() == 42);

        TRAP::Optional<i32&> o8 = fortyTwo;
        auto o8r = o8.AndThen([](i32){return TRAP::MakeOptional(42);});
        REQUIRE(*o8r == 42);

        TRAP::Optional<i32&> o9 = fortyTwo;
        auto o9r = std::move(o9).AndThen([](i32){return TRAP::MakeOptional(42);});
        REQUIRE(*o9r == 42);

        const TRAP::Optional<i32&> o10 = fortyTwo;
        auto o10r = o10.AndThen([](i32){return TRAP::MakeOptional(42);});
        REQUIRE(*o10r == 42);

        const TRAP::Optional<i32&> o11 = fortyTwo;
        auto o11r = std::move(o11).AndThen([](i32){return TRAP::MakeOptional(42);});
        REQUIRE(*o11r == 42);

        TRAP::Optional<i32&> o16 = TRAP::NullOpt;
        auto o16r = o16.AndThen([](i32){return TRAP::MakeOptional(42);});
        REQUIRE(!o16r);

        TRAP::Optional<i32&> o17 = TRAP::NullOpt;
        auto o17r = std::move(o17).AndThen([](i32){return TRAP::MakeOptional(42);});
        REQUIRE(!o17r);

        const TRAP::Optional<i32&> o18 = TRAP::NullOpt;
        auto o18r = o18.AndThen([](i32){return TRAP::MakeOptional(42);});
        REQUIRE(!o18r);

        const TRAP::Optional<i32&> o19 = TRAP::NullOpt;
        auto o19r = std::move(o19).AndThen([](i32){return TRAP::MakeOptional(42);});
        REQUIRE(!o19r);

        i32 i = 3;
        TRAP::Optional<i32&> o20{i};
        std::move(o20).AndThen([](i32& r){return TRAP::Optional<i32&>{++r};});
        REQUIRE(o20);
        REQUIRE(i == 4);
    }

    SECTION("Monadic AndThen constexpr")
    {
        static constexpr i32 constexprFortyTwo = 42;
        static constexpr auto GetOptInt = [](i32) -> TRAP::Optional<const i32&>
        {
            return constexprFortyTwo;
        };

        static constexpr TRAP::Optional<const i32&> o10 = constexprFortyTwo;
        static constexpr auto o10r = o10.AndThen(GetOptInt);
        REQUIRE(*o10r == 42);

        static constexpr TRAP::Optional<const i32&> o11 = constexprFortyTwo;
        static constexpr auto o11r = std::move(o11).AndThen(GetOptInt);
        REQUIRE(*o11r == 42);

        static constexpr TRAP::Optional<i32&> o18 = TRAP::NullOpt;
        static constexpr auto o18r = o18.AndThen(GetOptInt);
        REQUIRE(!o18r);

        static constexpr TRAP::Optional<i32&> o19 = TRAP::NullOpt;
        static constexpr auto o19r = std::move(o19).AndThen(GetOptInt);
        REQUIRE(!o19r);
    }

    SECTION("Monadic OrElse")
    {
        i32 fortyTwo = 42;
        i32 thirteen = 13;
        TRAP::Optional<i32&> o1 = fortyTwo;
        REQUIRE(*(o1.OrElse([&]{return TRAP::Optional<i32&>(thirteen);})) == 42);

        TRAP::Optional<i32&> o2;
        REQUIRE(*(o2.OrElse([&]{return TRAP::Optional<i32&>(thirteen);})) == 13);
    }
}
