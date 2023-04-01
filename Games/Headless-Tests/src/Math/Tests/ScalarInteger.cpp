#include "ScalarInteger.h"

#include <TRAP.h>

namespace ScalarInteger
{
    template<typename genType>
    struct TypeIsPowerOfTwo
    {
        genType Value;
        bool Return;
    };

    int32_t TestIsPowerOfTwo()
    {
        int32_t error = 0;

        //int16_t
        {
            constexpr std::array<TypeIsPowerOfTwo<int16_t>, 6> data =
            {
                {
                    { 0x0001, true },
                    { 0x0002, true },
                    { 0x0004, true },
                    { 0x0080, true },
                    { 0x0000, true },
                    { 0x0003, false }
                }
            };

            for(const auto& entry : data)
            {
                const bool result = TRAP::Math::IsPowerOfTwo(entry.Value);
                error += entry.Return == result ? 0 : 1;
            }
        }

        //uint16_t
        {
            constexpr std::array<TypeIsPowerOfTwo<uint16_t>, 6> data =
            {
                {
                    { 0x0001, true },
                    { 0x0002, true },
                    { 0x0004, true },
                    { 0x0000, true },
                    { 0x0000, true },
                    { 0x0003, false }
                }
            };

            for(const auto& entry : data)
            {
                const bool result = TRAP::Math::IsPowerOfTwo(entry.Value);
                error += entry.Return == result ? 0 : 1;
            }
        }

        //int32_t
        {
            constexpr std::array<TypeIsPowerOfTwo<int32_t>, 6> data =
            {
                {
                    { 0x00000001, true },
                    { 0x00000002, true },
                    { 0x00000004, true },
                    { 0x0000000f, false },
                    { 0x00000000, true },
                    { 0x00000003, false }
                }
            };

            for(const auto& entry : data)
            {
                const bool result = TRAP::Math::IsPowerOfTwo(entry.Value);
                error += entry.Return == result ? 0 : 1;
            }
        }

        //uint32_t
        {
            constexpr std::array<TypeIsPowerOfTwo<uint32_t>, 6> data =
            {
                {
                    { 0x00000001, true },
                    { 0x00000002, true },
                    { 0x00000004, true },
                    { 0x80000000, true },
                    { 0x00000000, true },
                    { 0x00000003, false }
                }
            };

            for(const auto& entry : data)
            {
                const bool result = TRAP::Math::IsPowerOfTwo(entry.Value);
                error += entry.Return == result ? 0 : 1;
            }
        }

        return error;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunScalarIntegerTests()
{
    int32_t error = 0;

    error += ScalarInteger::TestIsPowerOfTwo();

    return error;
}