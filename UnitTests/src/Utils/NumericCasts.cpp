#include <catch2/catch_test_macros.hpp>

#include <Utils/NumericCasts.h>

#if 0

template<typename T>
consteval void CheckValidNarrowCastsCompileTime()
{
    if constexpr(std::is_signed_v<T>)
    {
        constexpr int64_t test = 120;
        static_assert(NumericCast<T>(test) == T(120));

        if constexpr(std::floating_point<T>)
        {
            constexpr T test1(T(50));
            static_assert(NumericCast<int32_t>(test1) == int32_t(50));
        }
    }
    else
    {
        constexpr uint64_t test = 120;
        static_assert(NumericCast<T>(test) == T(120));
    }
}

template<typename T>
void CheckValidNarrowCastsRunTime()
{
    if constexpr(std::is_signed_v<T>)
    {
        const int64_t test = 120;
        REQUIRE(NumericCast<T>(test) == T(120));
    }
    else
    {
        const uint64_t test = 120;
        REQUIRE(NumericCast<T>(test) == T(120));
    }
}

template<typename T>
[[maybe_unused]] consteval void CheckInvalidNarrowCastsCompileTime()
{
    if constexpr(std::is_signed_v<T>)
    {
        if constexpr(std::floating_point<T>)
        {
            const double test = std::numeric_limits<double>::max();
            static_assert(NumericCast<T>(test));
        }
        else
        {
            const int64_t test = std::numeric_limits<int64_t>::max();
            static_assert(NumericCast<T>(test));
        }
    }
    else
    {
        constexpr uint64_t test = std::numeric_limits<uint64_t>::max();
        static_assert(NumericCast<T>(test));
    }
}

template<typename T>
void CheckInvalidNarrowCastsRunTime()
{
    if constexpr(std::is_signed_v<T>)
    {
        if constexpr(std::floating_point<T>)
        {
            const double test = std::numeric_limits<double>::max();
            using ErrorType = NumericCastError<T, double>;
            REQUIRE_THROWS_AS(NumericCast<T>(test), ErrorType);
        }
        else
        {
            const int64_t test = std::numeric_limits<int64_t>::max();
            using ErrorType = NumericCastError<T, int64_t>;
            REQUIRE_THROWS_AS(NumericCast<T>(test), ErrorType);
        }
    }
    else
    {
        const uint64_t test = std::numeric_limits<uint64_t>::max();
        using ErrorType = NumericCastError<T, uint64_t>;
        REQUIRE_THROWS_AS(NumericCast<T>(test), ErrorType);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
consteval void CheckValidWideCastsCompileTime()
{
    if constexpr(std::is_signed_v<T>)
    {
        constexpr int8_t test = 120;
        static_assert(NumericCast<T>(test) == T(120));

        if constexpr(std::same_as<T, float>)
        {
            constexpr int32_t test1 = 50;
            static_assert(NumericCast<T>(test1) == T(50));
        }
        else if constexpr(std::same_as<T, double>)
        {
            constexpr int64_t test1 = 50;
            static_assert(NumericCast<T>(test1) == T(50));
        }
    }
    else
    {
        constexpr uint8_t test = 120;
        static_assert(NumericCast<T>(test) == T(120));
    }
}

template<typename T>
void CheckValidWideCastsRunTime()
{
    if constexpr(std::is_signed_v<T>)
    {
        const int8_t test = 120;
        REQUIRE(NumericCast<T>(test) == T(120));

        if constexpr(std::same_as<T, float>)
        {
            const int32_t test1 = 50;
            REQUIRE(NumericCast<T>(test1) == T(50));
        }
        else if constexpr(std::same_as<T, double>)
        {
            const int64_t test1 = 50;
            REQUIRE(NumericCast<T>(test1) == T(50));
        }
    }
    else
    {
        const uint8_t test = 120;
        REQUIRE(NumericCast<T>(test) == T(120));
    }
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename ExpectedType, typename U>
consteval void CheckValidSignCastsCompileTime()
{
    static_assert(NumericCast<ExpectedType>(U(120)) == ExpectedType(120));
    static_assert(std::same_as<decltype(NumericCast<ExpectedType>(U(120))), ExpectedType>);
}

template<typename ExpectedType, typename U>
void CheckValidSignCastsRunTime()
{
    const U test(120);
    REQUIRE(NumericCast<ExpectedType>(test) == ExpectedType(120));
    REQUIRE(std::same_as<decltype(NumericCast<ExpectedType>(test)), ExpectedType>);
}

template<typename ExpectedType, typename U>
[[maybe_unused]] consteval void CheckInvalidSignCastsCompileTime()
{
    if constexpr(std::is_signed_v<U>)
    {
        constexpr U test = -120;
        static_assert(NumericCast<ExpectedType>(test));
    }
    else
    {
        constexpr U test = std::numeric_limits<U>::max();
        static_assert(NumericCast<ExpectedType>(test));
    }
}

template<typename ExpectedType, typename U>
void CheckInvalidSignCastsRunTime()
{
    if constexpr(std::is_signed_v<U>)
    {
        const U test = -120;
        using ErrorType = NumericCastError<ExpectedType, U>;
        REQUIRE_THROWS_AS(NumericCast<ExpectedType>(test), ErrorType);
    }
    else
    {
        const U test = std::numeric_limits<U>::max();
        using ErrorType = NumericCastError<ExpectedType, U>;
        REQUIRE_THROWS_AS(NumericCast<ExpectedType>(test), ErrorType);
    }
}

TEST_CASE("NumericCast()", "[utils][numericcast]")
{
    SECTION("Valid narowing casts")
    {
        // CheckValidNarrowCastsCompileTime<uint64_t>();
        CheckValidNarrowCastsCompileTime<uint32_t>();
        CheckValidNarrowCastsCompileTime<uint16_t>();
        CheckValidNarrowCastsCompileTime<uint8_t>();

        // CheckValidNarrowCastsCompileTime<int64_t>();
        CheckValidNarrowCastsCompileTime<int32_t>();
        CheckValidNarrowCastsCompileTime<int16_t>();
        CheckValidNarrowCastsCompileTime<int8_t>();
        CheckValidNarrowCastsCompileTime<double>();
        CheckValidNarrowCastsCompileTime<float>();

        // CheckValidNarrowCastsRunTime<uint64_t>();
        CheckValidNarrowCastsRunTime<uint32_t>();
        CheckValidNarrowCastsRunTime<uint16_t>();
        CheckValidNarrowCastsRunTime<uint8_t>();

        // CheckValidNarrowCastsRunTime<int64_t>();
        CheckValidNarrowCastsRunTime<int32_t>();
        CheckValidNarrowCastsRunTime<int16_t>();
        CheckValidNarrowCastsRunTime<int8_t>();
        CheckValidNarrowCastsRunTime<double>();
        CheckValidNarrowCastsRunTime<float>();
    }

    SECTION("Invalid narrowing casts")
    {
        // CheckInvalidNarrowCastsCompileTime<uint64_t>();
        // CheckInvalidNarrowCastsCompileTime<uint32_t>();
        // CheckInvalidNarrowCastsCompileTime<uint16_t>();
        // CheckInvalidNarrowCastsCompileTime<uint8_t>();

        // CheckInvalidNarrowCastsCompileTime<int64_t>();
        // CheckInvalidNarrowCastsCompileTime<int32_t>();
        // CheckInvalidNarrowCastsCompileTime<int16_t>();
        // CheckInvalidNarrowCastsCompileTime<int8_t>();
        // CheckInvalidNarrowCastsCompileTime<double>();
        // CheckInvalidNarrowCastsCompileTime<float>();

        // CheckInvalidNarrowCastsRunTime<uint64_t>();
        CheckInvalidNarrowCastsRunTime<uint32_t>();
        CheckInvalidNarrowCastsRunTime<uint16_t>();
        CheckInvalidNarrowCastsRunTime<uint8_t>();

        // CheckInvalidNarrowCastsRunTime<int64_t>();
        CheckInvalidNarrowCastsRunTime<int32_t>();
        CheckInvalidNarrowCastsRunTime<int16_t>();
        CheckInvalidNarrowCastsRunTime<int8_t>();
        // CheckInvalidNarrowCastsRunTime<double>();
        CheckInvalidNarrowCastsRunTime<float>();
    }

    SECTION("Valid widening casts")
    {
        CheckValidWideCastsCompileTime<uint64_t>();
        CheckValidWideCastsCompileTime<uint32_t>();
        CheckValidWideCastsCompileTime<uint16_t>();
        // CheckValidWideCastsCompileTime<uint8_t>();

        CheckValidWideCastsCompileTime<int64_t>();
        CheckValidWideCastsCompileTime<int32_t>();
        CheckValidWideCastsCompileTime<int16_t>();
        // CheckValidWideCastsCompileTime<int8_t>();
        CheckValidWideCastsCompileTime<double>();
        CheckValidWideCastsCompileTime<float>();

        CheckValidWideCastsRunTime<uint64_t>();
        CheckValidWideCastsRunTime<uint32_t>();
        CheckValidWideCastsRunTime<uint16_t>();
        // CheckValidWideCastsRunTime<uint8_t>();

        CheckValidWideCastsRunTime<int64_t>();
        CheckValidWideCastsRunTime<int32_t>();
        CheckValidWideCastsRunTime<int16_t>();
        // CheckValidWideCastsRunTime<int8_t>();
        CheckValidWideCastsRunTime<double>();
        CheckValidWideCastsRunTime<float>();
    }

    SECTION("Valid sign casts")
    {
        CheckValidSignCastsCompileTime<uint64_t, int64_t>();
        CheckValidSignCastsCompileTime<uint32_t, int32_t>();
        CheckValidSignCastsCompileTime<uint16_t, int16_t>();
        CheckValidSignCastsCompileTime<uint8_t, int8_t>();

        CheckValidSignCastsCompileTime<int64_t, uint64_t>();
        CheckValidSignCastsCompileTime<int32_t, uint32_t>();
        CheckValidSignCastsCompileTime<int16_t, uint16_t>();
        CheckValidSignCastsCompileTime<int8_t, uint8_t>();

        CheckValidSignCastsRunTime<uint64_t, int64_t>();
        CheckValidSignCastsRunTime<uint32_t, int32_t>();
        CheckValidSignCastsRunTime<uint16_t, int16_t>();
        CheckValidSignCastsRunTime<uint8_t, int8_t>();

        CheckValidSignCastsRunTime<int64_t, uint64_t>();
        CheckValidSignCastsRunTime<int32_t, uint32_t>();
        CheckValidSignCastsRunTime<int16_t, uint16_t>();
        CheckValidSignCastsRunTime<int8_t, uint8_t>();
    }

    SECTION("Invalid sign casts")
    {
        // CheckInvalidSignCastsCompileTime<uint64_t, int64_t>();
        // CheckInvalidSignCastsCompileTime<uint32_t, int32_t>();
        // CheckInvalidSignCastsCompileTime<uint16_t, int16_t>();
        // CheckInvalidSignCastsCompileTime<uint8_t, int8_t>();

        // CheckInvalidSignCastsCompileTime<int64_t, uint64_t>();
        // CheckInvalidSignCastsCompileTime<int32_t, uint32_t>();
        // CheckInvalidSignCastsCompileTime<int16_t, uint16_t>();
        // CheckInvalidSignCastsCompileTime<int8_t, uint8_t>();

        CheckInvalidSignCastsRunTime<uint64_t, int64_t>();
        CheckInvalidSignCastsRunTime<uint32_t, int32_t>();
        CheckInvalidSignCastsRunTime<uint16_t, int16_t>();
        CheckInvalidSignCastsRunTime<uint8_t, int8_t>();

        CheckInvalidSignCastsRunTime<int64_t, uint64_t>();
        CheckInvalidSignCastsRunTime<int32_t, uint32_t>();
        CheckInvalidSignCastsRunTime<int16_t, uint16_t>();
        CheckInvalidSignCastsRunTime<int8_t, uint8_t>();
    }
}
#endif