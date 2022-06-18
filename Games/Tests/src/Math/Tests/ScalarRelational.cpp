#include "ScalarRelational.h"

#include <TRAP.h>

namespace ScalarRelational
{
    int32_t TestEqualEpsilon()
    {
        int32_t error = 0;

        static_assert(TRAP::Math::Equal(1.01f, 1.02f, 0.1f), "MathTests: Failed constexpr");
        static_assert(!TRAP::Math::Equal(1.01f, 1.02f, 0.001f), "MathTests: Failed constexpr");

        error += TRAP::Math::Equal(1.01f, 1.02f, 0.1f) ? 0 : 1;
        error += !TRAP::Math::Equal(1.01f, 1.02f, 0.001f) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestNotEqualEpsilon()
    {
        int32_t error = 0;

        static_assert(TRAP::Math::NotEqual(1.01f, 1.02f, 0.001f), "MathTests: Failed constexpr");
        static_assert(!TRAP::Math::NotEqual(1.01f, 1.02f, 0.1f), "MathTests: Failed constexpr");

        error += TRAP::Math::NotEqual(1.01f, 1.02f, 0.001f) ? 0 : 1;
        error += !TRAP::Math::NotEqual(1.01f, 1.02f, 0.1f) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestEqualSign()
    {
        int32_t error = 0;

        error += TRAP::Math::Equal(-0.0f, 0.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
        error += TRAP::Math::Equal(-0.0, 0.0, TRAP::Math::Epsilon<double>()) ? 0 : 1;

        error += !TRAP::Math::Equal(-1.0f, 2.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
        error += !TRAP::Math::Equal(-1.0, 2.0, TRAP::Math::Epsilon<double>()) ? 0 : 1;

        error += !TRAP::Math::Equal(-0.00001f, 1.00000f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
        error += !TRAP::Math::Equal(-0.00001, 1.00000, TRAP::Math::Epsilon<double>()) ? 0 : 1;

        error += !TRAP::Math::Equal(-1.0f, 1.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
        error += !TRAP::Math::Equal(-1.0, 1.0, TRAP::Math::Epsilon<double>()) ? 0 : 1;

        return error;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunScalarRelationalTests()
{
    int32_t error = 0;

    error += ScalarRelational::TestEqualEpsilon();
    error += ScalarRelational::TestNotEqualEpsilon();

    error += ScalarRelational::TestEqualSign();

    return error;
}