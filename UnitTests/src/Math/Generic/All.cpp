#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

TEMPLATE_TEST_CASE("TRAP::Math::All()", "[math][generic][all][vec]", TRAP::Math::Vec2b, TRAP::Math::Vec3b, TRAP::Math::Vec4b)
{
    using Scalar = TestType::value_type;
    using Vec4Scalar = TRAP::Math::tVec4<Scalar>;

    STATIC_REQUIRE(TRAP::Math::All(TestType(Vec4Scalar(true, true, true, true))));
    STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::Not(TestType(Vec4Scalar(false, false, false, false)))));
}
