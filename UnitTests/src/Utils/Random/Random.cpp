#include <catch2/catch_test_macros.hpp>
#include <list>

#include "Utils/Random/Random.h"

TEST_CASE("TRAP::Utils::SeederDefault", "[utils][random][seederdefault]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Utils::SeederDefault>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Utils::SeederDefault>);
        STATIC_REQUIRE_FALSE(std::movable<TRAP::Utils::SeederDefault>);
    }

    SECTION("Constructors")
    {
        TRAP::Utils::SeederDefault seeder{};

        //Copy constructor
        // const TRAP::Utils::SeederDefault seederCopy(seeder); //Deleted

        //Move constructor
        // const TRAP::Utils::SeederDefault seederMove(std::move(seeder)); //Deleted
    }

    SECTION("Assignments")
    {
        TRAP::Utils::SeederDefault seeder{};

        //Copy assignment operator
        // const TRAP::Utils::SeederDefault seederCopy = seeder; //Deleted

        //Move assignment operator
        // const TRAP::Utils::SeederDefault seederMove = std::move(seeder); //Deleted
    }

    SECTION("operator()")
    {
        TRAP::Utils::SeederDefault seeder{};

        [[maybe_unused]] const std::seed_seq& seedSeq = seeder();
    }
}

TEST_CASE("TRAP::Utils::RandomLocal", "[utils][random][local]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Utils::RandomLocal>);
        STATIC_REQUIRE(std::copyable<TRAP::Utils::RandomLocal>);
        STATIC_REQUIRE(std::movable<TRAP::Utils::RandomLocal>);

        STATIC_REQUIRE(std::same_as<TRAP::Utils::RandomLocal::EngineType, std::mt19937_64>);
        STATIC_REQUIRE(std::same_as<TRAP::Utils::RandomLocal::SeederType, TRAP::Utils::SeederDefault>);
        STATIC_REQUIRE(std::same_as<TRAP::Utils::RandomLocal::IntegerDistT<i32>, std::uniform_int_distribution<i32>>);
        STATIC_REQUIRE(std::same_as<TRAP::Utils::RandomLocal::RealDistT<f32>, std::uniform_real_distribution<f32>>);
        STATIC_REQUIRE(std::same_as<TRAP::Utils::RandomLocal::BoolDistT, std::bernoulli_distribution>);
    }

    SECTION("Constructors")
    {
        TRAP::Utils::RandomLocal random{};

        //Copy constructor
        const TRAP::Utils::RandomLocal randomCopy(random);

        //Move constructor
        const TRAP::Utils::RandomLocal randomMove(std::move(random));
    }

    SECTION("Assignments")
    {
        TRAP::Utils::RandomLocal random{};

        //Copy assignment operator
        const TRAP::Utils::RandomLocal randomCopy = random;

        //Move assignment operator
        const TRAP::Utils::RandomLocal randomMove = std::move(random);
    }

    SECTION("Min()")
    {
        STATIC_REQUIRE(TRAP::Utils::RandomLocal::Min() == std::mt19937_64::min());
    }

    SECTION("Max()")
    {
        STATIC_REQUIRE(TRAP::Utils::RandomLocal::Max() == std::mt19937_64::max());
    }

    SECTION("Discard()")
    {
        TRAP::Utils::RandomLocal random{};

        random.Discard(10u);
    }

    SECTION("Reseed()")
    {
        TRAP::Utils::RandomLocal random{};

        random.Reseed();
    }

    SECTION("Seed(engine::result_type)")
    {
        TRAP::Utils::RandomLocal random{};

        random.Seed(234234234u);
    }

    SECTION("Seed<SSeq>(SSeq&)")
    {
        TRAP::Utils::RandomLocal random{};

        std::seed_seq seed({std::random_device{}()});
        random.Seed(seed);
    }

    SECTION("Get()")
    {
        TRAP::Utils::RandomLocal random{};

        const std::mt19937_64::result_type num = random.Get();

        REQUIRE(num <= std::mt19937_64::max());
        REQUIRE(num >= std::mt19937_64::min());
    }

    SECTION("Get(integral, integral)")
    {
        TRAP::Utils::RandomLocal random{};

        i16 num1 = random.Get<i16>(i16(-1000), i16(1000));
        i32 num2 = random.Get<i32>(i32(-1000), i32(1000));
        i64 num3 = random.Get<i64>(i64(-1000), i64(1000));
        REQUIRE((num1 >= -1000 && num1 <= 1000));
        REQUIRE((num2 >= -1000 && num2 <= 1000));
        REQUIRE((num3 >= -1000 && num3 <= 1000));

        num1 = random.Get<i16>(i16(1000), i16(-1000));
        num2 = random.Get<i32>(i32(1000), i32(-1000));
        num3 = random.Get<i64>(i64(1000), i64(-1000));
        REQUIRE((num1 >= -1000 && num1 <= 1000));
        REQUIRE((num2 >= -1000 && num2 <= 1000));
        REQUIRE((num3 >= -1000 && num3 <= 1000));

        u16 num4 = random.Get<u16>(u16(0), u16(1000));
        u32 num5 = random.Get<u32>(u32(0), u32(1000));
        u64 num6 = random.Get<u64>(u64(0), u64(1000));
        REQUIRE(num4 <= 1000);
        REQUIRE(num5 <= 1000);
        REQUIRE(num6 <= 1000);

        num4 = random.Get<u16>(u16(1000), u16(0));
        num5 = random.Get<u32>(u32(1000), u32(0));
        num6 = random.Get<u64>(u64(1000), u64(0));
        REQUIRE(num4 <= 1000);
        REQUIRE(num5 <= 1000);
        REQUIRE(num6 <= 1000);
    }

    SECTION("Get(floating-point, floating-point)")
    {
        TRAP::Utils::RandomLocal random{};

        f32 num1 = random.Get<f32>(-1000.0f, 1000.0f);
        f64 num2 = random.Get<f64>(-1000.0, 1000.0);
        REQUIRE((num1 >= -1000.0f && num1 <= 1000.0f));
        REQUIRE((num2 >= -1000.0 && num2 <= 1000.0));

        num1 = random.Get<f32>(1000.0f, -1000.0f);
        num2 = random.Get<f64>(1000.0, -1000.0);
        REQUIRE((num1 >= -1000.0f && num1 <= 1000.0f));
        REQUIRE((num2 >= -1000.0 && num2 <= 1000.0));
    }

    SECTION("Get(byte, byte)")
    {
        TRAP::Utils::RandomLocal random{};

        i8 num1 = random.Get<i8>(i8(-128), i8(128));
        u8 num2 = random.Get<u8>(u8(0), u8(128));
        REQUIRE((num1 >= -128 && num1 <= 128));
        REQUIRE(num2 <= 128);

        num1 = random.Get<i8>(i8(128), i8(-128));
        num2 = random.Get<u8>(u8(128), u8(0));
        REQUIRE((num1 >= -128 && num1 <= 128));
        REQUIRE(num2 <= 128);
    }

    SECTION("Get(character, character)")
    {
        TRAP::Utils::RandomLocal random{};

        char num1 = random.Get<char>(char(-128), char(128));
        wchar_t num2 = random.Get<wchar_t>(wchar_t(0), wchar_t(1000));
        char16_t num3 = random.Get<char16_t>(char16_t(0), char16_t(1000));
        char32_t num4 = random.Get<char32_t>(char32_t(0), char32_t(1000));
        REQUIRE((num1 >= -128 && num1 <= 128));
        REQUIRE(num2 <= 1000);
        REQUIRE(num3 <= 1000);
        REQUIRE(num4 <= 1000);

        num1 = random.Get<char>(char(128), char(-128));
        num2 = random.Get<wchar_t>(wchar_t(1000), wchar_t(0));
        num3 = random.Get<char16_t>(char16_t(1000), char16_t(0));
        num4 = random.Get<char32_t>(char32_t(1000), char32_t(0));
        REQUIRE((num1 >= -128 && num1 <= 128));
        REQUIRE(num2 <= 1000);
        REQUIRE(num3 <= 1000);
        REQUIRE(num4 <= 1000);
    }

    SECTION("bool Get(probability)")
    {
        TRAP::Utils::RandomLocal random{};

        [[maybe_unused]] const bool num1 = random.Get(0.5);
        REQUIRE_FALSE(random.Get(0.0));
        REQUIRE_FALSE(random.Get(-1.0));
        REQUIRE(random.Get(10.0));
    }

    SECTION("Get(initializer_list)")
    {
        TRAP::Utils::RandomLocal random{};

        [[maybe_unused]] const i32 num = random.Get({-10, 5, 6, 7, 100});
        REQUIRE(random.Get(std::initializer_list<i32>{}) == i32{});
    }

    SECTION("Get(iterator, iterator)")
    {
        TRAP::Utils::RandomLocal random{};

        const std::list<i32> arr{-10, 3, 4, 100, 10};
        [[maybe_unused]] decltype(arr)::const_iterator num = random.Get(arr.begin(), arr.end());

        const std::list<i32> arrEmpty{};
        REQUIRE(arrEmpty.end() == random.Get(arrEmpty.begin(), arrEmpty.end()));
    }

    SECTION("Get(const container&)")
    {
        TRAP::Utils::RandomLocal random{};

        const std::list<i32> arr{-10, 3, 4, 100, 10};
        [[maybe_unused]] decltype(arr)::const_iterator num = random.Get(arr);

        const std::list<i32> arrEmpty{};
        REQUIRE(arrEmpty.end() == random.Get(arrEmpty));
    }

    SECTION("Get(container&)")
    {
        TRAP::Utils::RandomLocal random{};

        std::list<i32> arr{-10, 3, 4, 100, 10};
        [[maybe_unused]] decltype(arr)::iterator num = random.Get(arr);

        std::list<i32> arrEmpty{};
        REQUIRE(arrEmpty.end() == random.Get(arrEmpty));
    }

    SECTION("Get(c-array)")
    {
        TRAP::Utils::RandomLocal random{};

        i32 arr[5] = {-10, 3, 4, 100, 10};

        const i32* const num = random.Get(arr);
        REQUIRE(num);
    }

    SECTION("Shuffle(iterator, iterator)")
    {
        TRAP::Utils::RandomLocal random{};

        static constexpr std::array<i32, 5u> arr{-10, 3, 4, 100, 10};
        std::array<i32, 5u> arrCopy = arr;

        random.Shuffle(arrCopy.begin(), arrCopy.end());
        REQUIRE_FALSE(arrCopy == arr);
    }

    SECTION("Shuffle(container)")
    {
        TRAP::Utils::RandomLocal random{};

        static constexpr std::array<i32, 5u> arr{-10, 3, 4, 100, 10};
        std::array<i32, 5u> arrCopy = arr;

        random.Shuffle(arrCopy);
        REQUIRE_FALSE(arrCopy == arr);
    }

    SECTION("operator==()")
    {
        const TRAP::Utils::RandomLocal random{};
        const TRAP::Utils::RandomLocal randomCopy = random;

        REQUIRE(random == randomCopy);

        const TRAP::Utils::RandomLocal randomOther{};

        REQUIRE_FALSE(random == randomOther);
        REQUIRE_FALSE(randomCopy == randomOther);
    }

    SECTION("Serialize()")
    {
        TRAP::Utils::RandomLocal random{};

        const std::string randomState = random.Serialize();
        REQUIRE_FALSE(randomState.empty());
    }

    SECTION("Deserialize()")
    {
        TRAP::Utils::RandomLocal random{};

        const std::string randomState = random.Serialize();
        REQUIRE_FALSE(randomState.empty());

        TRAP::Utils::RandomLocal random2{};
        REQUIRE(random2.Deserialize(randomState));

        //random2s state now must match that of random.
        REQUIRE(random == random2);
    }
}

TEST_CASE("TRAP::Utils::Random", "[utils][random][static]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::Utils::Random>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Utils::Random>);
        STATIC_REQUIRE_FALSE(std::movable<TRAP::Utils::Random>);

        STATIC_REQUIRE(std::same_as<TRAP::Utils::Random::EngineType, std::mt19937_64>);
        STATIC_REQUIRE(std::same_as<TRAP::Utils::Random::SeederType, TRAP::Utils::SeederDefault>);
        STATIC_REQUIRE(std::same_as<TRAP::Utils::Random::IntegerDistT<i32>, std::uniform_int_distribution<i32>>);
        STATIC_REQUIRE(std::same_as<TRAP::Utils::Random::RealDistT<f32>, std::uniform_real_distribution<f32>>);
        STATIC_REQUIRE(std::same_as<TRAP::Utils::Random::BoolDistT, std::bernoulli_distribution>);
    }

    // SECTION("Constructors")
    // {
    //     // TRAP::Utils::Random random{}; //Deleted

    //     //Copy constructor
    //     // const TRAP::Utils::Random randomCopy(random); //Deleted

    //     //Move constructor
    //     // const TRAP::Utils::Random randomMove(std::move(random)); //Deleted
    // }

    // SECTION("Assignments")
    // {
    //     // TRAP::Utils::Random random{}; //Deleted

    //     //Copy assignment operator
    //     // const TRAP::Utils::Random randomCopy = random; //Deleted

    //     //Move assignment operator
    //     // const TRAP::Utils::Random randomMove = std::move(random); //Deleted
    // }

    SECTION("Min()")
    {
        STATIC_REQUIRE(TRAP::Utils::Random::Min() == std::mt19937_64::min());
    }

    SECTION("Max()")
    {
        STATIC_REQUIRE(TRAP::Utils::Random::Max() == std::mt19937_64::max());
    }

    SECTION("Discard()")
    {
        TRAP::Utils::Random::Discard(10u);
    }

    SECTION("Reseed()")
    {
        TRAP::Utils::Random::Reseed();
    }

    SECTION("Seed(engine::result_type)")
    {
        TRAP::Utils::Random::Seed(234234234u);
    }

    SECTION("Seed<SSeq>(SSeq&)")
    {
        std::seed_seq seed({std::random_device{}()});
        TRAP::Utils::Random::Seed(seed);
    }

    SECTION("Get()")
    {
        const std::mt19937_64::result_type num = TRAP::Utils::Random::Get();

        REQUIRE(num <= std::mt19937_64::max());
        REQUIRE(num >= std::mt19937_64::min());
    }

    SECTION("Get(integral, integral)")
    {
        i16 num1 = TRAP::Utils::Random::Get<i16>(i16(-1000), i16(1000));
        i32 num2 = TRAP::Utils::Random::Get<i32>(i32(-1000), i32(1000));
        i64 num3 = TRAP::Utils::Random::Get<i64>(i64(-1000), i64(1000));
        REQUIRE((num1 >= -1000 && num1 <= 1000));
        REQUIRE((num2 >= -1000 && num2 <= 1000));
        REQUIRE((num3 >= -1000 && num3 <= 1000));

        num1 = TRAP::Utils::Random::Get<i16>(i16(1000), i16(-1000));
        num2 = TRAP::Utils::Random::Get<i32>(i32(1000), i32(-1000));
        num3 = TRAP::Utils::Random::Get<i64>(i64(1000), i64(-1000));
        REQUIRE((num1 >= -1000 && num1 <= 1000));
        REQUIRE((num2 >= -1000 && num2 <= 1000));
        REQUIRE((num3 >= -1000 && num3 <= 1000));

        u16 num4 = TRAP::Utils::Random::Get<u16>(u16(0), u16(1000));
        u32 num5 = TRAP::Utils::Random::Get<u32>(u32(0), u32(1000));
        u64 num6 = TRAP::Utils::Random::Get<u64>(u64(0), u64(1000));
        REQUIRE(num4 <= 1000);
        REQUIRE(num5 <= 1000);
        REQUIRE(num6 <= 1000);

        num4 = TRAP::Utils::Random::Get<u16>(u16(1000), u16(0));
        num5 = TRAP::Utils::Random::Get<u32>(u32(1000), u32(0));
        num6 = TRAP::Utils::Random::Get<u64>(u64(1000), u64(0));
        REQUIRE(num4 <= 1000);
        REQUIRE(num5 <= 1000);
        REQUIRE(num6 <= 1000);
    }

    SECTION("Get(floating-point, floating-point)")
    {
        f32 num1 = TRAP::Utils::Random::Get<f32>(-1000.0f, 1000.0f);
        f64 num2 = TRAP::Utils::Random::Get<f64>(-1000.0, 1000.0);
        REQUIRE((num1 >= -1000.0f && num1 <= 1000.0f));
        REQUIRE((num2 >= -1000.0 && num2 <= 1000.0));

        num1 = TRAP::Utils::Random::Get<f32>(1000.0f, -1000.0f);
        num2 = TRAP::Utils::Random::Get<f64>(1000.0, -1000.0);
        REQUIRE((num1 >= -1000.0f && num1 <= 1000.0f));
        REQUIRE((num2 >= -1000.0 && num2 <= 1000.0));
    }

    SECTION("Get(byte, byte)")
    {
        i8 num1 = TRAP::Utils::Random::Get<i8>(i8(-128), i8(128));
        u8 num2 = TRAP::Utils::Random::Get<u8>(u8(0), u8(128));
        REQUIRE((num1 >= -128 && num1 <= 128));
        REQUIRE(num2 <= 128);

        num1 = TRAP::Utils::Random::Get<i8>(i8(128), i8(-128));
        num2 = TRAP::Utils::Random::Get<u8>(u8(128), u8(0));
        REQUIRE((num1 >= -128 && num1 <= 128));
        REQUIRE(num2 <= 128);
    }

    SECTION("Get(character, character)")
    {
        char num1 = TRAP::Utils::Random::Get<char>(char(-128), char(128));
        wchar_t num2 = TRAP::Utils::Random::Get<wchar_t>(wchar_t(0), wchar_t(1000));
        char16_t num3 = TRAP::Utils::Random::Get<char16_t>(char16_t(0), char16_t(1000));
        char32_t num4 = TRAP::Utils::Random::Get<char32_t>(char32_t(0), char32_t(1000));
        REQUIRE((num1 >= -128 && num1 <= 128));
        REQUIRE(num2 <= 1000);
        REQUIRE(num3 <= 1000);
        REQUIRE(num4 <= 1000);

        num1 = TRAP::Utils::Random::Get<char>(char(128), char(-128));
        num2 = TRAP::Utils::Random::Get<wchar_t>(wchar_t(1000), wchar_t(0));
        num3 = TRAP::Utils::Random::Get<char16_t>(char16_t(1000), char16_t(0));
        num4 = TRAP::Utils::Random::Get<char32_t>(char32_t(1000), char32_t(0));
        REQUIRE((num1 >= -128 && num1 <= 128));
        REQUIRE(num2 <= 1000);
        REQUIRE(num3 <= 1000);
        REQUIRE(num4 <= 1000);
    }

    SECTION("bool Get(probability)")
    {
        [[maybe_unused]] const bool num1 = TRAP::Utils::Random::Get(0.5);
        REQUIRE_FALSE(TRAP::Utils::Random::Get(0.0));
        REQUIRE_FALSE(TRAP::Utils::Random::Get(-1.0));
        REQUIRE(TRAP::Utils::Random::Get(10.0));
    }

    SECTION("Get(initializer_list)")
    {
        [[maybe_unused]] const i32 num = TRAP::Utils::Random::Get({-10, 5, 6, 7, 100});
        REQUIRE(TRAP::Utils::Random::Get(std::initializer_list<i32>{}) == i32{});
    }

    SECTION("Get(iterator, iterator)")
    {
        const std::list<i32> arr{-10, 3, 4, 100, 10};
        [[maybe_unused]] decltype(arr)::const_iterator num = TRAP::Utils::Random::Get(arr.begin(), arr.end());

        const std::list<i32> arrEmpty{};
        REQUIRE(arrEmpty.end() == TRAP::Utils::Random::Get(arrEmpty.begin(), arrEmpty.end()));
    }

    SECTION("Get(const container&)")
    {
        const std::list<i32> arr{-10, 3, 4, 100, 10};
        [[maybe_unused]] decltype(arr)::const_iterator num = TRAP::Utils::Random::Get(arr);

        const std::list<i32> arrEmpty{};
        REQUIRE(arrEmpty.end() == TRAP::Utils::Random::Get(arrEmpty));
    }

    SECTION("Get(container&)")
    {
        std::list<i32> arr{-10, 3, 4, 100, 10};
        [[maybe_unused]] decltype(arr)::iterator num = TRAP::Utils::Random::Get(arr);

        std::list<i32> arrEmpty{};
        REQUIRE(arrEmpty.end() == TRAP::Utils::Random::Get(arrEmpty));
    }

    SECTION("Get(c-array)")
    {
        i32 arr[5] = {-10, 3, 4, 100, 10};

        const i32* const num = TRAP::Utils::Random::Get(arr);
        REQUIRE(num);
    }

    SECTION("Shuffle(iterator, iterator)")
    {
        static constexpr std::array<i32, 5u> arr{-10, 3, 4, 100, 10};
        std::array<i32, 5u> arrCopy = arr;

        TRAP::Utils::Random::Shuffle(arrCopy.begin(), arrCopy.end());
        REQUIRE_FALSE(arrCopy == arr);
    }

    SECTION("Shuffle(container)")
    {
        static constexpr std::array<i32, 5u> arr{-10, 3, 4, 100, 10};
        std::array<i32, 5u> arrCopy = arr;

        TRAP::Utils::Random::Shuffle(arrCopy);
        REQUIRE_FALSE(arrCopy == arr);
    }

    SECTION("Serialize()")
    {
        const std::string randomState = TRAP::Utils::Random::Serialize();
        REQUIRE_FALSE(randomState.empty());
    }

    SECTION("Deserialize()")
    {
        const std::string randomState = TRAP::Utils::Random::Serialize();
        REQUIRE_FALSE(randomState.empty());

        REQUIRE(TRAP::Utils::Random::Deserialize(randomState));
    }
}
