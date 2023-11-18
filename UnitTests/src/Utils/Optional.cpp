#include <catch2/catch_test_macros.hpp>

#include <unordered_set>
#include <array>

#include "Core/Types.h"
#include "Utils/Optional.h"

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

TEST_CASE("TRAP::Optional", "[utils][optional]")
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

    SECTION("Assignment reference")
    {
        i32 i = 42;
        i32 j = 12;

        TRAP::Optional<i32&> o1 = i;
        TRAP::Optional<i32&> o2 = j;
        static constexpr TRAP::Optional<i32&> o3;

        o1 = o1;
        REQUIRE(*o1 == 42);
        REQUIRE(&*o1 == &i);

        o1 = o2;
        REQUIRE(*o1 == 12);

        o1 = o3;
        REQUIRE(!o1);

        i32 k = 42;
        o1 = k;
        REQUIRE(*o1 == 42);
        REQUIRE(*o1 == i);
        REQUIRE(*o1 == k);
        REQUIRE(&*o1 != &i);
        REQUIRE(&*o1 == &k);

        k = 12;
        REQUIRE(*o1 == 12);

        o1 = TRAP::NullOpt;
        REQUIRE(!o1);

        o1 = std::move(o2);
        REQUIRE(*o1 == 12);
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

        {
            TRAP::Optional<i32&> o;
            REQUIRE(!o);

            TRAP::Optional<i32&> oo = o;
            REQUIRE(!oo);
        }

        {
            i32 i = 42;
            TRAP::Optional<i32&> o = i;
            REQUIRE(o);
            REQUIRE(*o == 42);

            TRAP::Optional<i32&> oo = o;
            REQUIRE(oo);
            REQUIRE(*oo == 42);
        }

        std::vector<Foo> v;
        v.emplace_back();
        TRAP::Optional<std::vector<Foo>> ov = std::move(v);
        REQUIRE(ov->size() == 1);
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
        STATIC_REQUIRE(std::hash<TRAP::Optional<i32>>{}(op2) == 0);

        using OptStr = TRAP::Optional<std::string>;

        const std::array<OptStr, 4> s{"ABC", "abc", TRAP::NullOpt, "def"};

#ifdef TRAP_PLATFORM_LINUX
        REQUIRE(std::hash<OptStr>{}(std::get<0>(s)) == 11746482041453314842u);
        REQUIRE(std::hash<OptStr>{}(std::get<1>(s)) == 3663726644998027833);
        REQUIRE(std::hash<OptStr>{}(std::get<2>(s)) == 0u);
        REQUIRE(std::hash<OptStr>{}(std::get<3>(s)) == 11697390762615875584u);
#elif defined(TRAP_PLATFORM_WINDOWS)
        REQUIRE(std::hash<OptStr>{}(std::get<0>(s)) == 18027876433081418475u);
        REQUIRE(std::hash<OptStr>{}(std::get<1>(s)) == 3663726644998027833);
        REQUIRE(std::hash<OptStr>{}(std::get<2>(s)) == 0u);
        REQUIRE(std::hash<OptStr>{}(std::get<3>(s)) == 11697390762615875584u);
#endif
    }

    SECTION("In place")
    {
        static constexpr TRAP::Optional<i32> o1{std::in_place};
        static constexpr TRAP::Optional<i32> o2(std::in_place);
        STATIC_REQUIRE(o1);
        STATIC_REQUIRE(o1 == 0);
        STATIC_REQUIRE(o2);
        STATIC_REQUIRE(o2 == 0);

        static constexpr TRAP::Optional<i32> o3(std::in_place, 42);
        STATIC_REQUIRE(o3 == 42);

        static constexpr TRAP::Optional<std::tuple<i32, i32>> o4(std::in_place, 0, 1);
        STATIC_REQUIRE(o4);
        STATIC_REQUIRE(std::get<0>(*o4) == 0);
        STATIC_REQUIRE(std::get<1>(*o4) == 1);

        TRAP::Optional<std::vector<i32>> o5(std::in_place, {0, 1});
        REQUIRE(o5);
        REQUIRE((*o5)[0] == 0);
        REQUIRE((*o5)[1] == 1);

        TRAP::Optional<TakesInitAndVariadic> o6(std::in_place, {0, 1}, 2, 3);
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

        i32 i = 42;
        auto o6 = TRAP::MakeOptional<i32&>(i);
        STATIC_REQUIRE((std::same_as<decltype(o6), TRAP::Optional<i32&>>));
        REQUIRE(o6);
        REQUIRE(*o6 == 42);
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

        STATIC_REQUIRE(!std::is_default_constructible_v<TRAP::nullopt_t>);
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

        TRAP::Optional<MoveDetector> o4{std::in_place};
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

    SECTION("Monadic operations")
    {
        //Transform
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

            //Test void return
            static constexpr TRAP::Optional<i32> o7 = 40;
            static constexpr auto f7 = [](const i32&){return;};
            static constexpr auto o7r = o7.Transform(f7);
            STATIC_REQUIRE((std::same_as<decltype(o7r), const TRAP::Optional<std::monostate>>));
            STATIC_REQUIRE(o7r.HasValue());

            //Test each overload in turn
            static constexpr TRAP::Optional<i32> o8 = 42;
            static constexpr auto o8r = o8.Transform([](i32) { return 42; });
            STATIC_REQUIRE(*o8r == 42);

            static constexpr TRAP::Optional<i32> o9 = 42;
            static constexpr auto o9r = o9.Transform([](i32) { return; });
            STATIC_REQUIRE(o9r);

            static constexpr TRAP::Optional<i32> o12 = 42;
            static constexpr auto o12r = std::move(o12).Transform([](i32) { return 42; });
            STATIC_REQUIRE(*o12r == 42);

            static constexpr TRAP::Optional<i32> o13 = 42;
            static constexpr auto o13r = std::move(o13).Transform([](i32) { return; });
            STATIC_REQUIRE(o13r);

            static constexpr TRAP::Optional<i32> o24 = TRAP::NullOpt;
            static constexpr auto o24r = o24.Transform([](i32) { return 42; });
            STATIC_REQUIRE(!o24r);

            static constexpr TRAP::Optional<i32> o25 = TRAP::NullOpt;
            static constexpr auto o25r = o25.Transform([](i32) { return; });
            STATIC_REQUIRE(!o25r);

            static constexpr TRAP::Optional<i32> o28 = TRAP::NullOpt;
            static constexpr auto o28r = std::move(o28).Transform([](i32) { return 42; });
            STATIC_REQUIRE(!o28r);

            static constexpr TRAP::Optional<i32> o29 = TRAP::NullOpt;
            static constexpr auto o29r = std::move(o29).Transform([](i32) { return; });
            STATIC_REQUIRE(!o29r);

            //Callable which returns a reference
            TRAP::Optional<i32> o38 = 42;
            auto o38r = o38.Transform([](i32& i) -> const i32& {return i;});
            REQUIRE(o38r);
            REQUIRE(*o38r == 42);

            i32 i = 42;
            TRAP::Optional<i32&> o39 = i;
            o39.Transform([](i32& x){x = 12;});
            REQUIRE(i == 12);
        }

        //AndThen
        {
            //lhs is empty
            static constexpr TRAP::Optional<i32> o1;
            static constexpr auto o1r = o1.AndThen([](i32) { return TRAP::Optional<f32>{42}; });
            STATIC_REQUIRE((std::same_as<decltype(o1r), const TRAP::Optional<f32>>));
            STATIC_REQUIRE(!o1r);

            //lhs has value
            static constexpr TRAP::Optional<i32> o2 = 12;
            static constexpr auto o2r = o2.AndThen([](i32) { return TRAP::Optional<f32>{42}; });
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

            i32 i = 3;
            TRAP::Optional<i32&> o20{i};
            const auto _ = std::move(o20).AndThen([](i32& r){return TRAP::Optional<i32&>{++r};});
            REQUIRE(o20);
            REQUIRE(i == 4);
        }

        //OrElse
        {
            static constexpr TRAP::Optional<i32> o1 = 42;
            STATIC_REQUIRE(*(o1.OrElse([]{return TRAP::MakeOptional(13);})) == 42);

            static constexpr TRAP::Optional<i32> o2;
            STATIC_REQUIRE(*(o2.OrElse([]{return TRAP::MakeOptional(13);})) == 13);
        }

        //ValueOrElse
        {
            static constexpr TRAP::Optional<i32> o1 = 42;
            STATIC_REQUIRE(o1.ValueOrElse([]{return 5;}) == 42);

            i32 i = 42;
            TRAP::Optional<i32&> o2 = i;
            REQUIRE(o2.ValueOrElse([]{return 5;}) == 42);

            static constexpr TRAP::Optional<i32> o3;
            STATIC_REQUIRE(o3.ValueOrElse([]{return 5;}) == 5);
        }

        //Disjunction
        {
            static constexpr TRAP::Optional<i32> o1 = 42;
            static constexpr TRAP::Optional<i32> o2 = 12;
            static constexpr TRAP::Optional<i32> o3;

            STATIC_REQUIRE(*o1.Disjunction(o2) == 42);
            STATIC_REQUIRE(*o1.Disjunction(o3) == 42);
            STATIC_REQUIRE(*o2.Disjunction(o1) == 12);
            STATIC_REQUIRE(*o2.Disjunction(o3) == 12);
            STATIC_REQUIRE(*o3.Disjunction(o1) == 42);
            STATIC_REQUIRE(*o3.Disjunction(o2) == 12);
        }

        //Conjunction
        {
            static constexpr TRAP::Optional<i32> o1 = 42;
            STATIC_REQUIRE(*o1.Conjunction(42.0) == 42.0);
            REQUIRE(*o1.Conjunction(std::string{"hello"}) == std::string{"hello"});

            static constexpr TRAP::Optional<i32> o2;
            STATIC_REQUIRE(!o2.Conjunction(42.0));
            STATIC_REQUIRE(!o2.Conjunction(std::string{"hello"}));
        }

        //TransformOr
        {
            static constexpr TRAP::Optional<i32> o1 = 21;
            STATIC_REQUIRE((o1.TransformOr([](i32 x){return x * 2;}, 13)) == 42);

            static constexpr TRAP::Optional<i32> o2;
            STATIC_REQUIRE((o2.TransformOr([](i32 x){return x * 2;}, 13)) == 13);
        }

        //TransformOrElse
        {
            static constexpr TRAP::Optional<i32> o1 = 21;
            STATIC_REQUIRE((o1.TransformOrElse([](i32 x){return x * 2;}, []{return 13;})) == 42);

            static constexpr TRAP::Optional<i32> o2;
            STATIC_REQUIRE((o2.TransformOrElse([](i32 x){return x * 2;}, []{return 13;})) == 13);
        }

        //Take
        {
            TRAP::Optional<i32> o1 = 42;
            REQUIRE(*o1.Take() == 42);
            REQUIRE(!o1);

            TRAP::Optional<i32> o2;
            REQUIRE(!o2.Take());
            REQUIRE(!o2);
        }
    }
}
