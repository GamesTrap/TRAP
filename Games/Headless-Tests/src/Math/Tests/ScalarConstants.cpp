#include "ScalarConstants.h"

#include <TRAP.h>

namespace ScalarConstants
{
    template<typename valType>
    requires std::floating_point<valType>
    int32_t TestEpsilon()
    {
        int32_t error = 0;

        const valType test = TRAP::Math::Epsilon<valType>();
        error += test > static_cast<valType>(0) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    template<typename valType>
    requires std::floating_point<valType>
    int32_t TestPI()
    {
        int32_t error = 0;

        const valType test = TRAP::Math::PI<valType>();
        error += test > static_cast<valType>(3.14) ? 0 : 1;
        error += test < static_cast<valType>(3.15) ? 0 : 1;

        return error;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunScalarConstantsTests()
{
    int32_t error = 0;

    error += ScalarConstants::TestEpsilon<float>();
    error += ScalarConstants::TestEpsilon<double>();
    error += ScalarConstants::TestPI<float>();
    error += ScalarConstants::TestPI<double>();

    return error;
}