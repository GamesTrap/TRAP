#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::Any()", "[math][generic][any][vec]", TRAP::Math::Vec2b, TRAP::Math::Vec3b, TRAP::Math::Vec4b)
{
    STATIC_REQUIRE(TRAP::Math::Any(TestType(TRAP::Math::Vec4b(true, false, true, false))));
    STATIC_REQUIRE(TRAP::Math::Any(TRAP::Math::Equal(TestType(TRAP::Math::Vec4b(true)),
                                                     TestType(TRAP::Math::Vec4b(true, false, true, false)))));
}
