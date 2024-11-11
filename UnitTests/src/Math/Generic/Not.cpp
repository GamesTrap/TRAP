#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Not()", "[math][generic][not][scalar]", bool)
{
    STATIC_REQUIRE(!TRAP::Math::Not(true));
    STATIC_REQUIRE( TRAP::Math::Not(false));
}

TEMPLATE_TEST_CASE("TRAP::Math::Not()", "[math][generic][not][vec]", TRAP::Math::Vec2b, TRAP::Math::Vec3b, TRAP::Math::Vec4b)
{
    static constexpr TestType A(false);
    static constexpr TestType B(true);
    static constexpr TestType C(TRAP::Math::Vec4b(true, false, false, false));
    static constexpr TestType D(TRAP::Math::Vec4b(true, true, false, false));
    static constexpr TestType E(TRAP::Math::Vec4b(true, true, true, false));

    STATIC_REQUIRE( TRAP::Math::All(TRAP::Math::Not(A)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Not(B)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Not(C)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Not(D)));
    STATIC_REQUIRE(!TRAP::Math::All(TRAP::Math::Not(E)));
}
