#include "VectorCommon.h"

#include <TRAP.h>

namespace VectorCommon
{
    template<typename vecType>
    requires TRAP::Math::IsVec<vecType>
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
    requires TRAP::Math::IsVec<vecType>
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
    requires TRAP::Math::IsVec<vecType>
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
    requires TRAP::Math::IsVec<vecType>
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

    int32_t TestLog2()
    {
        int32_t error = 0;

        // int32_t a0 = static_cast<int32_t>(TRAP::Math::Log2(16.0f));
        TRAP::Math::Vec2i b0(TRAP::Math::Log2(TRAP::Math::Vec2(16.0f)));
        TRAP::Math::Vec3i c0(TRAP::Math::Log2(TRAP::Math::Vec3(16.0f)));
        TRAP::Math::Vec4i d0(TRAP::Math::Log2(TRAP::Math::Vec4(16.0f)));

        // int32_t a1 = TRAP::Math::Log2(int32_t(16));
        // TRAP::Math::Vec2i b1 = TRAP::Math::Log2(TRAP::Math::Vec2i(16));
        // TRAP::Math::Vec3i c1 = TRAP::Math::Log2(TRAP::Math::Vec3i(16));
        // TRAP::Math::Vec4i d1 = TRAP::Math::Log2(TRAP::Math::Vec4i(16));

        // error += a0 == a1 ? 0 : 1;
        // error += TRAP::Math::All(TRAP::Math::Equal(b0, b1)) ? 0 : 1;
        // error += TRAP::Math::All(TRAP::Math::Equal(c0, c1)) ? 0 : 1;
        // error += TRAP::Math::All(TRAP::Math::Equal(d0, d1)) ? 0 : 1;

        // uint64_t a2 = TRAP::Math::Log2(uint64_t(16));
        // TRAP::Math::Vec2ui64 b2 = TRAP::Math::Log2(TRAP::Math::Vec2ui64(16));
        // TRAP::Math::Vec3ui64 c2 = TRAP::Math::Log2(TRAP::Math::Vec3ui64(16));
        // TRAP::Math::Vec4ui64 d2 = TRAP::Math::Log2(TRAP::Math::Vec4ui64(16));

        // error += a2  == uint64_t(4) ? 0 : 1;
        // error += TRAP::Math::All(TRAP::Math::Equal(b2, TRAP::Math::Vec2ui64(4))) ? 0 : 1;
        // error += TRAP::Math::All(TRAP::Math::Equal(c2, TRAP::Math::Vec3ui64(4))) ? 0 : 1;
        // error += TRAP::Math::All(TRAP::Math::Equal(d2, TRAP::Math::Vec4ui64(4))) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestToString()
    {
        int32_t error = 0;

        {
            TRAP::Math::Vec2 a1(1, 2);
            std::string a2 = a1.ToString();
            error += a2 != std::string("Vec2f(1.000000, 2.000000)") ? 1 : 0;

            TRAP::Math::Vec3 b1(1, 2, 3);
            std::string b2 = b1.ToString();
            error += b2 != std::string("Vec3f(1.000000, 2.000000, 3.000000)") ? 1 : 0;

            TRAP::Math::Vec4 c1(1, 2, 3, 4);
            std::string c2 = c1.ToString();
            error += c2 != std::string("Vec4f(1.000000, 2.000000, 3.000000, 4.000000)") ? 1 : 0;

            TRAP::Math::Vec2d j1(1, 2);
            std::string j2 = j1.ToString();
            error += j2 != std::string("Vec2d(1.000000, 2.000000)") ? 1 : 0;

            TRAP::Math::Vec3d k1(1, 2, 3);
            std::string k2 = k1.ToString();
            error += k2 != std::string("Vec3d(1.000000, 2.000000, 3.000000)") ? 1 : 0;

            TRAP::Math::Vec4d l1(1, 2, 3, 4);
            std::string l2 = l1.ToString();
            error += l2 != std::string("Vec4d(1.000000, 2.000000, 3.000000, 4.000000)") ? 1 : 0;
        }

        {
            TRAP::Math::Vec2b m1(false, true);
            std::string m2 = m1.ToString();
            error += m2 != std::string("Vec2b(false, true)") ? 1 : 0;

            TRAP::Math::Vec3b o1(false, true, false);
            std::string o2 = o1.ToString();
            error += o2 != std::string("Vec3b(false, true, false)") ? 1 : 0;

            TRAP::Math::Vec4b p1(false, true, false, true);
            std::string p2 = p1.ToString();
            error += p2 != std::string("Vec4b(false, true, false, true)") ? 1 : 0;
        }

        {
            TRAP::Math::Vec2i d1(1, 2);
            std::string d2 = d1.ToString();
            error += d2 != std::string("Vec2i32(1, 2)") ? 1 : 0;

            TRAP::Math::Vec3i e1(1, 2, 3);
            std::string e2 = e1.ToString();
            error += e2 != std::string("Vec3i32(1, 2, 3)") ? 1 : 0;

            TRAP::Math::Vec4i f1(1, 2, 3, 4);
            std::string f2 = f1.ToString();
            error += f2 != std::string("Vec4i32(1, 2, 3, 4)") ? 1 : 0;
        }

        {
            TRAP::Math::Vec2i8 d1(1, 2);
            std::string d2 = d1.ToString();
            error += d2 != std::string("Vec2i8(1, 2)") ? 1 : 0;

            TRAP::Math::Vec3i8 e1(1, 2, 3);
            std::string e2 = e1.ToString();
            error += e2 != std::string("Vec3i8(1, 2, 3)") ? 1 : 0;

            TRAP::Math::Vec4i8 f1(1, 2, 3, 4);
            std::string f2 = f1.ToString();
            error += f2 != std::string("Vec4i8(1, 2, 3, 4)") ? 1 : 0;
        }

        {
            TRAP::Math::Vec2i16 d1(1, 2);
            std::string d2 = d1.ToString();
            error += d2 != std::string("Vec2i16(1, 2)") ? 1 : 0;

            TRAP::Math::Vec3i16 e1(1, 2, 3);
            std::string e2 = e1.ToString();
            error += e2 != std::string("Vec3i16(1, 2, 3)") ? 1 : 0;

            TRAP::Math::Vec4i16 f1(1, 2, 3, 4);
            std::string f2 = f1.ToString();
            error += f2 != std::string("Vec4i16(1, 2, 3, 4)") ? 1 : 0;
        }

        {
            TRAP::Math::Vec2i64 d1(1, 2);
            std::string d2 = d1.ToString();
            error += d2 != std::string("Vec2i64(1, 2)") ? 1 : 0;

            TRAP::Math::Vec3i64 e1(1, 2, 3);
            std::string e2 = e1.ToString();
            error += e2 != std::string("Vec3i64(1, 2, 3)") ? 1 : 0;

            TRAP::Math::Vec4i64 f1(1, 2, 3, 4);
            std::string f2 = f1.ToString();
            error += f2 != std::string("Vec4i64(1, 2, 3, 4)") ? 1 : 0;
        }

        {
            TRAP::Math::Vec2ui d1(1, 2);
            std::string d2 = d1.ToString();
            error += d2 != std::string("Vec2ui32(1, 2)") ? 1 : 0;

            TRAP::Math::Vec3ui e1(1, 2, 3);
            std::string e2 = e1.ToString();
            error += e2 != std::string("Vec3ui32(1, 2, 3)") ? 1 : 0;

            TRAP::Math::Vec4ui f1(1, 2, 3, 4);
            std::string f2 = f1.ToString();
            error += f2 != std::string("Vec4ui32(1, 2, 3, 4)") ? 1 : 0;
        }

        {
            TRAP::Math::Vec2ui8 d1(1, 2);
            std::string d2 = d1.ToString();
            error += d2 != std::string("Vec2ui8(1, 2)") ? 1 : 0;

            TRAP::Math::Vec3ui8 e1(1, 2, 3);
            std::string e2 = e1.ToString();
            error += e2 != std::string("Vec3ui8(1, 2, 3)") ? 1 : 0;

            TRAP::Math::Vec4ui8 f1(1, 2, 3, 4);
            std::string f2 = f1.ToString();
            error += f2 != std::string("Vec4ui8(1, 2, 3, 4)") ? 1 : 0;
        }

        {
            TRAP::Math::Vec2ui16 d1(1, 2);
            std::string d2 = d1.ToString();
            error += d2 != std::string("Vec2ui16(1, 2)") ? 1 : 0;

            TRAP::Math::Vec3ui16 e1(1, 2, 3);
            std::string e2 = e1.ToString();
            error += e2 != std::string("Vec3ui16(1, 2, 3)") ? 1 : 0;

            TRAP::Math::Vec4ui16 f1(1, 2, 3, 4);
            std::string f2 = f1.ToString();
            error += f2 != std::string("Vec4ui16(1, 2, 3, 4)") ? 1 : 0;
        }

        {
            TRAP::Math::Vec2ui64 d1(1, 2);
            std::string d2 = d1.ToString();
            error += d2 != std::string("Vec2ui64(1, 2)") ? 1 : 0;

            TRAP::Math::Vec3ui64 e1(1, 2, 3);
            std::string e2 = e1.ToString();
            error += e2 != std::string("Vec3ui64(1, 2, 3)") ? 1 : 0;

            TRAP::Math::Vec4ui64 f1(1, 2, 3, 4);
            std::string f2 = f1.ToString();
            error += f2 != std::string("Vec4ui64(1, 2, 3, 4)") ? 1 : 0;
        }

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

    error += VectorCommon::TestLog2();

    error += VectorCommon::TestToString();

    return error;
}