#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "Core/Types.h"
#include "Utils/UniqueResource.h"

namespace
{
    consteval void DefaultConstructorCompileTimeTest()
    {
        struct Value
        {
            i32 i;
        };

        struct Deleter
        {
            constexpr void operator()(Value) const
            {
                // static_assert(false);
            }
            i32 i;
        };

        static_assert(std::is_default_constructible_v<TRAP::UniqueResource<Value, Deleter>>);
        static_assert(!std::is_default_constructible_v<TRAP::UniqueResource<Value&, Deleter>>);

        static_assert(std::is_nothrow_default_constructible_v<TRAP::UniqueResource<Value, Deleter>>);
        struct ExValue : Value
        {
            constexpr ExValue() noexcept(false)
            {
            }
        };
        static_assert(!std::is_nothrow_default_constructible_v<TRAP::UniqueResource<ExValue, Deleter>>);

        constexpr TRAP::UniqueResource<Value, Deleter> res{};
        static_assert(res.Get().i == 0);
        static_assert(res.GetDeleter().i == 0);
    }

    void DefaultConstructorRunTimeTest()
    {
        struct Value
        {
            i32 i;
        };

        struct Deleter
        {
            void operator()(Value) const
            {
                REQUIRE(false);
            }
            i32 i;
        };

        TRAP::UniqueResource<Value, Deleter> res{};
        REQUIRE(res.Get().i == 0);
        REQUIRE(res.GetDeleter().i == 0);
    }

    void ConstructorRunTimeTest()
    {
        struct Value
        {
            i32 i;
        };

        struct Deleter
        {
            void operator()(Value v) const
            {
                REQUIRE(v.i == i);
            }
            i32 i;
        };

        const auto r1 = TRAP::UniqueResource<Value, Deleter>(Value{}, Deleter{});
        REQUIRE(r1.Get().i == 0);
        REQUIRE(r1.GetDeleter().i == 0);

        auto r2 = TRAP::UniqueResource<Value, Deleter>(1, 2);
        REQUIRE(r2.Get().i == 1);
        REQUIRE(r2.GetDeleter().i == 2);
        r2.Release();

        static_assert(!std::is_constructible_v<TRAP::UniqueResource<Value&, Deleter>, Value, Deleter>);
        static_assert(!std::is_constructible_v<TRAP::UniqueResource<Value&, Deleter>, i32, Deleter>);
        static_assert(!std::is_constructible_v<TRAP::UniqueResource<Value&, Deleter>, const Value&, Deleter>);

        Value v{3};
        const auto r3 = TRAP::UniqueResource<Value&, Deleter>(v, 3);

        Deleter d4{4};
        const auto r4 = TRAP::UniqueResource(std::ref(v), std::ref(d4));
        --d4.i;

        static_assert(std::same_as<decltype(r4), const TRAP::UniqueResource<std::reference_wrapper<Value>, std::reference_wrapper<Deleter>>>);
        static_assert(!std::is_constructible_v<decltype(r4), Value, Deleter>);

        i32 counter = 0, dcounter = 99;
        {
            const TRAP::UniqueResource r(std::ref(counter), [&dcounter](i32& i){++dcounter; ++i;});
        }
        REQUIRE(counter == 1);
        REQUIRE(dcounter == 100);

        {
            struct NoThrowMove
            {
                constexpr NoThrowMove() noexcept
                {
                }

                constexpr NoThrowMove(NoThrowMove&&) noexcept(true)
                {
                }

                NoThrowMove(const NoThrowMove&)
                {
                    throw 1;
                }
            };

            const TRAP::UniqueResource r(NoThrowMove{}, [&dcounter](NoThrowMove&){++dcounter;});
        }
        REQUIRE(dcounter == 101);

        {
            struct ThrowOnCopy
            {
                constexpr ThrowOnCopy() noexcept
                {
                }

                ThrowOnCopy(ThrowOnCopy&&) noexcept(false)
                {
                    REQUIRE(false);
                }

                ThrowOnCopy(const ThrowOnCopy&)
                {
                    throw 1;
                }

                explicit ThrowOnCopy(Value) noexcept(false)
                {
                    REQUIRE(false);
                }

                constexpr explicit ThrowOnCopy(Value&) noexcept(false)
                {
                }
            };
            const auto d = [&dcounter](auto&){++dcounter;};

            const TRAP::UniqueResource r(Value(1), d);

            try
            {
                const TRAP::UniqueResource r(ThrowOnCopy{}, d);
                REQUIRE(false);
            }
            catch(i32)
            {
                REQUIRE(dcounter == 102);
            }
        }
        REQUIRE(dcounter == 103);

        {
            struct CopyValue
            {
                constexpr explicit CopyValue(const Value& v)
                    : i(v.i)
                {
                }

                i32 i;
            };

            struct Deleter
            {
                void operator()(const Value&)
                {
                    REQUIRE(false);
                }
                constexpr void operator()(const CopyValue& c)
                {
                    ref = c.i;
                }
                i32& ref;
            };

            struct CopyDeleter
            {
                explicit CopyDeleter(Deleter&&) noexcept(false)
                {
                    REQUIRE(false);
                }
                explicit CopyDeleter(const Deleter&) noexcept(false)
                {
                    throw 1;
                }
                consteval void operator()(const Value&) noexcept = delete;
                void operator()(const CopyValue&)
                {
                    REQUIRE(false);
                }
            };

            try
            {
                const TRAP::UniqueResource<CopyValue, CopyDeleter> r(Value(11), Deleter{dcounter});
                REQUIRE(false);
            }
            catch (i32)
            {
                REQUIRE(dcounter == 11);
            }
        }
    }

    void MoveConstructorRunTimeTest()
    {
        {
            struct Deleter
            {
                void operator()(i32) const
                {
                    REQUIRE(false);
                }
            };

            TRAP::UniqueResource<i32, Deleter> r0;
            const auto rr0 = std::move(r0);
            REQUIRE(r0.Get() == 0);
            REQUIRE(rr0.Get() == 0);

            struct DeleterThrowingCopy
            {
                constexpr DeleterThrowingCopy() = default;
                DeleterThrowingCopy(const DeleterThrowingCopy&)
                {
                    throw 1;
                }

                void operator()(i32) const
                {
                    REQUIRE(false);
                }
            };

            TRAP::UniqueResource<i32, DeleterThrowingCopy> r1;
            try
            {
                const auto rr1 = std::move(r1);
                REQUIRE(false);
            }
            catch(i32)
            {
            }
        }

        {
            struct Resource
            {
                constexpr Resource() = default;
                constexpr Resource(Resource&& r) noexcept
                    : moved(r.moved)
                {
                    r.moved = true;
                }
                constexpr Resource(Resource& r)
                    : moved(r.moved)
                {
                }
                bool moved = false;
            };

            TRAP::UniqueResource r(Resource{}, [](const auto&){});
            const auto rr = std::move(r);
            REQUIRE(r.Get().moved);
            REQUIRE_FALSE(rr.Get().moved);
        }

        {
            struct Resource2
            {
                constexpr Resource2() = default;
                constexpr Resource2(Resource2&& r) noexcept(false)
                    : moved(r.moved)
                {
                    r.moved = false;
                }
                constexpr Resource2(Resource2& r)
                    : moved(r.moved)
                {
                }
                bool moved = false;
            };

            TRAP::UniqueResource r2(Resource2{}, [](const auto&){});
            const auto rr2 = std::move(r2);
            REQUIRE_FALSE(r2.Get().moved);
            REQUIRE_FALSE(rr2.Get().moved);
        }

        {
            struct ThrowingCopy
            {
                constexpr ThrowingCopy(i32)
                {
                }
                ThrowingCopy(const ThrowingCopy&)
                {
                    throw 1;
                }
            };

            i32 dcounter = 0;
            {
                const auto d = [&dcounter](const auto&){++dcounter;};
                TRAP::UniqueResource<ThrowingCopy, decltype(d)> r(1, d);
                try
                {
                    const auto rr = std::move(r);
                    REQUIRE(false);
                }
                catch(i32)
                {
                    REQUIRE(dcounter == 0);
                }
            }
            REQUIRE(dcounter == 1);
        }
    }

    i32 called1 = 0;

    void AssignRunTimeTest()
    {
        struct ThrowingDeleter
        {
            constexpr ThrowingDeleter() = default;
            constexpr ThrowingDeleter(i32& called)
                : called(called)
            {
            }
            constexpr ThrowingDeleter(const ThrowingDeleter&) = default;
            ThrowingDeleter& operator=(const ThrowingDeleter&)
            {
                throw 1;
            }

            constexpr void operator()(i32 i) const noexcept
            {
                ++called;
            }
            i32& called = called1;
        };

        i32 called2 = 0;
        {
            TRAP::UniqueResource<i32, ThrowingDeleter> r1;
            REQUIRE(r1.Get() == 0);
            TRAP::UniqueResource<i32, ThrowingDeleter> r2(2, ThrowingDeleter{called2});
            REQUIRE(r2.Get() == 2);
            try
            {
                r1 = std::move(r2);
                REQUIRE(false);
            }
            catch(i32)
            {
            }
            REQUIRE(called1 == 0);
            REQUIRE(called2 == 0);
            REQUIRE(r1.Get() == 0);
            REQUIRE(r2.Get() == 2);
        }
        REQUIRE(called1 == 0);
        REQUIRE(called2 == 1);
    }

    void ModifiersRunTimeTest()
    {
        i32 dcounter = 0;
        const auto d = [&dcounter](i32 i){dcounter += i;};
        TRAP::UniqueResource<i32, decltype(d)> r(1, d);
        r.Reset();
        REQUIRE(dcounter == 1);
        r.Reset(2);
        REQUIRE(dcounter == 1);
        r.Release();
        REQUIRE(dcounter == 1);
        r.Release();
        REQUIRE(dcounter == 1);
        r.Reset(3);
        REQUIRE(dcounter == 1);
        r.Reset(4);
        REQUIRE(dcounter == 4);
    }

    template<typename T>
    concept HasStar = requires(T& t){*t;};

    template<typename T>
    concept HasArrow = requires(T& t){t.operator->();};

    consteval void ObserversCompileTimeTest()
    {
        using R1 = TRAP::UniqueResource<i32, void(*)(i32)>;
        static_assert(!HasStar<R1>);
        static_assert(!HasArrow<R1>);
        using R2 = TRAP::UniqueResource<const void*, void(*)(const void*)>;
        static_assert(!HasStar<R2>);
        static_assert(HasArrow<R2>);
    }

    void ObserversRunTimeTest()
    {
        struct D
        {
            void operator()(i32* p) const noexcept
            {
                delete p;
            }
        };
        int* p = new int(3);
        const TRAP::UniqueResource<i32*, D> r(p, D{});
        REQUIRE(r.Get() == p);
        REQUIRE(*r == 3);
        REQUIRE(r.operator->() == p);
        (void)r.GetDeleter();
    }

    void MakeUniqueResourceCheckedRunTimeTest()
    {
        struct Boolish
        {
            constexpr explicit operator bool() const noexcept
            {
                return val;
            }
            bool val;
        };

        using TRAP::MakeUniqueResourceChecked;

        {
            struct ThrowingCopy
            {
                constexpr ThrowingCopy(i32 i)
                    : val(i)
                {
                }
                ThrowingCopy(const ThrowingCopy&)
                {
                    throw 1;
                }
                constexpr Boolish operator==(i32 i) const noexcept
                {
                    return {i == val};
                }
                i32 val;
            };

            i32 dcounter = 0;
            auto d = [&dcounter](const auto&){++dcounter;};

            try
            {
                (void)MakeUniqueResourceChecked(ThrowingCopy(1), 0, d);
                REQUIRE(false);
            }
            catch(i32)
            {
                REQUIRE(dcounter == 1);
            }

            dcounter = 0;
            try
            {
                (void)MakeUniqueResourceChecked(ThrowingCopy(1), 1, d);
                REQUIRE(false);
            }
            catch(i32)
            {
                REQUIRE(dcounter == 0);
            }
        }
    }
}

TEST_CASE("TRAP::UniqueResource", "[utils][uniqueresource]")
{
    SECTION("Class requirements")
    {
        struct Value
        {
            i32 i;
        };

        struct Deleter
        {
            void operator()(Value v) const
            {
                REQUIRE(v.i == i);
            }
            i32 i;
        };

        STATIC_REQUIRE(std::is_final_v<TRAP::UniqueResource<Value, Deleter>>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::UniqueResource<Value, Deleter>>);
        STATIC_REQUIRE(std::movable<TRAP::UniqueResource<Value, Deleter>>);
    }

    SECTION("Default constructor")
    {
        DefaultConstructorCompileTimeTest();
        DefaultConstructorRunTimeTest();
    }

    SECTION("Constructors")
    {
        ConstructorRunTimeTest();
    }

    SECTION("Move constructor")
    {
        MoveConstructorRunTimeTest();
    }

    SECTION("Assign")
    {
        AssignRunTimeTest();
    }

    SECTION("Modifiers")
    {
        ModifiersRunTimeTest();
    }

    SECTION("Observers")
    {
        ObserversCompileTimeTest();
        ObserversRunTimeTest();
    }
}

TEST_CASE("TRAP::MakeUniqueResourceChecked", "[utils][makeuniqueresourcechecked]")
{
    MakeUniqueResourceCheckedRunTimeTest();
}