#include "ScalarCommon.h"

#include <TRAP.h>

namespace ScalarCommon
{
    template<typename T>
    requires std::floating_point<T>
    int32_t TestMin()
    {
        int32_t error = 0;

        const T n = static_cast<T>(0);
        const T b = static_cast<T>(1);
        error += TRAP::Math::Equal(TRAP::Math::Min(n, b), n, TRAP::Math::Epsilon<T>()) ? 0 : 1;
        error += TRAP::Math::Equal(TRAP::Math::Min(b, n), n, TRAP::Math::Epsilon<T>()) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    template<typename T>
    requires std::floating_point<T>
    int32_t TestMinNaN()
    {
        int32_t error = 0;

        const T b = static_cast<T>(1);
        const T n = static_cast<T>(NAN);
        error += TRAP::Math::IsNaN(TRAP::Math::Min(n, b)) ? 0 : 1;
        error += !TRAP::Math::IsNaN(TRAP::Math::Min(b, n)) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    template<typename T>
    requires std::floating_point<T>
    int32_t TestMax()
    {
        int32_t error = 0;

        const T n = static_cast<T>(0);
        const T b = static_cast<T>(1);
        error += TRAP::Math::Equal(TRAP::Math::Max(n, b), b, TRAP::Math::Epsilon<T>()) ? 0 : 1;
        error += TRAP::Math::Equal(TRAP::Math::Max(b, n), b, TRAP::Math::Epsilon<T>()) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    template<typename T>
    requires std::floating_point<T>
    int32_t TestMaxNaN()
    {
        int32_t error = 0;

        const T b = static_cast<T>(1);
        const T n = static_cast<T>(NAN);
        error += TRAP::Math::IsNaN(TRAP::Math::Max(n, b)) ? 0 : 1;
        error += !TRAP::Math::IsNaN(TRAP::Math::Max(b, n)) ? 0 : 1;

        return error;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunScalarCommonTests()
{
    int32_t error = 0;

    error += ScalarCommon::TestMin<float>();
    error += ScalarCommon::TestMin<double>();
    error += ScalarCommon::TestMinNaN<float>();
    error += ScalarCommon::TestMinNaN<double>();

    error += ScalarCommon::TestMax<float>();
    error += ScalarCommon::TestMax<double>();
    error += ScalarCommon::TestMaxNaN<float>();
    error += ScalarCommon::TestMaxNaN<double>();

    return error;
}