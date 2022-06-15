#include "VectorCommon.h"

#include <TRAP.h>

namespace VectorCommon
{
    template<typename vecType>
    int32_t TestMin()
    {
        using T = typename vecType::valueType;

        int32_t error = 0;

        const vecType n(static_cast<T>(0));
        const vecType b(static_cast<T>(1));

        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(n, b), n, TRAP::Math::Epsilon<T>())) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Min(b, n), n, TRAP::Math::Epsilon<T>())) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    template<typename vecType>
    int32_t TestMinNaN()
    {
        using T = typename vecType::valueType;

        int32_t error = 0;

        const vecType b(static_cast<T>(1));
        const vecType n(NAN);

        error += TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Min(n, b))) ? 0 : 1;
        error += !TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Min(b, n))) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    template<typename vecType>
    int32_t TestMax()
    {
        using T = typename vecType::valueType;

        int32_t error = 0;

        const vecType n(static_cast<T>(0));
        const vecType b(static_cast<T>(1));
        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(n, b), b, TRAP::Math::Epsilon<T>())) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(b, n), b, TRAP::Math::Epsilon<T>())) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    template<typename vecType>
    int32_t TestMaxNaN()
    {
        using T = typename vecType::valueType;

        int32_t error = 0;

        const vecType b(static_cast<T>(1));
        const vecType n(NAN);

        error += TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Max(n, b))) ? 0 : 1;
        error += !TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Max(b, n))) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestIRound()
    {
        int32_t error = 0;

        for(float f = 0.0f; f < 3.1f; f += 0.05f)
        {
            int32_t roundFast = static_cast<int32_t>(TRAP::Math::IRound(f));
            int32_t roundStd = static_cast<int32_t>(TRAP::Math::Round(f));
            error += roundFast == roundStd ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestURound()
    {
        int32_t error = 0;

        for(float f = 0.0f; f < 3.1f; f += 0.05f)
        {
            int32_t roundFast = static_cast<int32_t>(TRAP::Math::URound(f));
            int32_t roundStd = static_cast<int32_t>(TRAP::Math::Round(f));
            error += roundFast == roundStd ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestLog2()
    {
        int32_t error = 0;

        int32_t a0 = static_cast<int32_t>(TRAP::Math::Log2(16.0f));
        TRAP::Math::Vec2i b0(TRAP::Math::Log2(TRAP::Math::Vec2(16.0f)));
        TRAP::Math::Vec3i c0(TRAP::Math::Log2(TRAP::Math::Vec3(16.0f)));
        TRAP::Math::Vec4i d0(TRAP::Math::Log2(TRAP::Math::Vec4(16.0f)));

        int32_t a1 = TRAP::Math::Log2(int32_t(16));
        TRAP::Math::Vec2i b1 = TRAP::Math::Log2(TRAP::Math::Vec2i(16));
        TRAP::Math::Vec3i c1 = TRAP::Math::Log2(TRAP::Math::Vec3i(16));
        TRAP::Math::Vec4i d1 = TRAP::Math::Log2(TRAP::Math::Vec4i(16));

        error += a0 == a1 ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(b0, b1)) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(c0, c1)) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(d0, d1)) ? 0 : 1;

        uint64_t a2 = TRAP::Math::Log2(uint64_t(16));
        TRAP::Math::Vec2ui64 b2 = TRAP::Math::Log2(TRAP::Math::Vec2ui64(16));
        TRAP::Math::Vec3ui64 c2 = TRAP::Math::Log2(TRAP::Math::Vec3ui64(16));
        TRAP::Math::Vec4ui64 d2 = TRAP::Math::Log2(TRAP::Math::Vec4ui64(16));

        error += a2  == uint64_t(4) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(b2, TRAP::Math::Vec2ui64(4))) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(c2, TRAP::Math::Vec3ui64(4))) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(d2, TRAP::Math::Vec4ui64(4))) ? 0 : 1;

        return error;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunVectorCommonTests()
{
    int32_t error = 0;

    error += VectorCommon::TestMin<TRAP::Math::Vec3>();
    error += VectorCommon::TestMin<TRAP::Math::Vec2>();
    error += VectorCommon::TestMinNaN<TRAP::Math::Vec3>();
    error += VectorCommon::TestMinNaN<TRAP::Math::Vec2>();

    error += VectorCommon::TestMax<TRAP::Math::Vec3>();
    error += VectorCommon::TestMax<TRAP::Math::Vec2>();
    error += VectorCommon::TestMaxNaN<TRAP::Math::Vec3>();
    error += VectorCommon::TestMaxNaN<TRAP::Math::Vec2>();

    error += VectorCommon::TestIRound();
    error += VectorCommon::TestURound();

    error += VectorCommon::TestLog2();

    return error;
}