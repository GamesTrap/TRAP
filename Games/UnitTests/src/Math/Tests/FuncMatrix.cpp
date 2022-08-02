#include "FuncMatrix.h"

#include <TRAP.h>

namespace FuncMatrix
{
    int32_t TestMatrixCompMult()
    {
        int32_t error = 0;

        {
            TRAP::Math::Mat3 m(0, 1, 2, 3, 4, 5, 6, 7, 8);
            TRAP::Math::Mat3 n = TRAP::Math::MatrixCompMult(m, m);
            TRAP::Math::Mat3 expected = TRAP::Math::Mat3(0, 1, 4, 9, 16, 25, 36, 49, 64);
            error += TRAP::Math::All(TRAP::Math::Equal(n, expected, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            TRAP::Math::Mat4 m(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
            TRAP::Math::Mat4 n = TRAP::Math::MatrixCompMult(m, m);
            TRAP::Math::Mat4 expected = TRAP::Math::Mat4(0, 1, 4, 9, 16, 25, 36, 49, 64, 81, 100, 121, 144, 169, 196, 225);
            error += TRAP::Math::All(TRAP::Math::Equal(n, expected, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    void TestOuterProduct()
    {
        { TRAP::Math::Mat3 m = TRAP::Math::OuterProduct(TRAP::Math::Vec3(1.0f), TRAP::Math::Vec3(1.0f)); }
        { TRAP::Math::Mat4 m = TRAP::Math::OuterProduct(TRAP::Math::Vec4(1.0f), TRAP::Math::Vec4(1.0f)); }
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestTranspose()
    {
        int32_t error = 0;

        {
            TRAP::Math::Mat3 m(0, 1, 2, 3, 4, 5, 6, 7, 8);
            TRAP::Math::Mat3 t = TRAP::Math::Transpose(m);
            const TRAP::Math::Mat3 expected = TRAP::Math::Mat3(0, 3, 6, 1, 4, 7, 2, 5, 8);
            error += TRAP::Math::All(TRAP::Math::Equal(t, expected, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            TRAP::Math::Mat4 m(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
            TRAP::Math::Mat4 t = TRAP::Math::Transpose(m);
            const TRAP::Math::Mat4 expected = TRAP::Math::Mat4(0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15);
            error += TRAP::Math::All(TRAP::Math::Equal(t, expected, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestInverse()
    {
        int32_t error = 0;

        {
            TRAP::Math::Mat4 a(TRAP::Math::Vec4(1, 0, 1, 0), TRAP::Math::Vec4(0, 1, 0, 0),
                            TRAP::Math::Vec4(0, 0, 1, 0), TRAP::Math::Vec4(0, 0, 0, 1));
            TRAP::Math::Mat4 b = TRAP::Math::Inverse(a);
            TRAP::Math::Mat4 i = a * b;
            TRAP::Math::Mat4 identity(1);
            error += TRAP::Math::All(TRAP::Math::Equal(i, identity, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            TRAP::Math::Mat3 a(TRAP::Math::Vec3(1, 0, 1), TRAP::Math::Vec3(0, 1, 0), TRAP::Math::Vec3(0, 0, 1));
            TRAP::Math::Mat3 b = TRAP::Math::Inverse(a);
            TRAP::Math::Mat3 i = a * b;
            TRAP::Math::Mat3 identity(1);
            error += TRAP::Math::All(TRAP::Math::Equal(i, identity, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    template<typename matType, typename vecType>
    int32_t TestEqual()
    {
        using valType = typename matType::valueType;

        const valType epsilon = static_cast<valType>(0.001f);
        const valType one = static_cast<valType>(1);
        const valType two = static_cast<valType>(2);

        int32_t error = 0;

        error += TRAP::Math::All(TRAP::Math::Equal(matType(one), matType(one), epsilon)) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(matType(one), matType(two), vecType(epsilon))) ? 1 : 0;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    template<typename matType, typename vecType>
    int32_t TestNotEqual()
    {
        using valType = typename matType::valueType;

        const valType epsilon = static_cast<valType>(0.001f);
        const valType one = static_cast<valType>(1);
        const valType two = static_cast<valType>(2);

        int32_t error = 0;

        error += !TRAP::Math::Any(TRAP::Math::NotEqual(matType(one), matType(one), epsilon)) ? 0 : 1;
        error += !TRAP::Math::Any(TRAP::Math::NotEqual(matType(one), matType(two), vecType(epsilon))) ? 1 : 0;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestTranslate()
    {
        int32_t error = 0;

        const TRAP::Math::Mat4 m(1.0f);
        const TRAP::Math::Vec3 v(1.0f);

        const TRAP::Math::Mat4 t = TRAP::Math::Translate(m, v);
        error += TRAP::Math::All(TRAP::Math::Equal(t[3], TRAP::Math::Vec4(1.0f), TRAP::Math::Epsilon<float>())) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestScale()
    {
        int32_t error = 0;

        const TRAP::Math::Mat4 m(1.0f);
        const TRAP::Math::Vec3 v(2.0f);

        const TRAP::Math::Mat4 s = TRAP::Math::Scale(m, v);
        const TRAP::Math::Mat4 r = TRAP::Math::Mat4(TRAP::Math::Vec4(2, 0, 0, 0), TRAP::Math::Vec4(0, 2, 0, 0),
                                                    TRAP::Math::Vec4(0, 0, 2, 0), TRAP::Math::Vec4(0, 0, 0, 1));
        error += TRAP::Math::All(TRAP::Math::Equal(s, r, TRAP::Math::Epsilon<float>())) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestRotate()
    {
        int32_t error = 0;

        const TRAP::Math::Vec4 a(1.0f, 0.0f, 0.0f, 1.0f);

        const TRAP::Math::Mat4 r = TRAP::Math::Rotate(TRAP::Math::Mat4(1.0f), TRAP::Math::Radians(90.0f), TRAP::Math::Vec3(0, 0, 1));
        const TRAP::Math::Vec4 b = r * a;
        error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.0001f)) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMat3RowSet()
    {
        int32_t error = 0;

        TRAP::Math::Mat3 m(1);

        m = TRAP::Math::Row(m, 0, TRAP::Math::Vec3(0, 1, 2));
        m = TRAP::Math::Row(m, 1, TRAP::Math::Vec3(4, 5, 6));
        m = TRAP::Math::Row(m, 2, TRAP::Math::Vec3(8, 9, 10));

        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(m, 0), TRAP::Math::Vec3(0, 1, 2), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(m, 1), TRAP::Math::Vec3(4, 5, 6), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(m, 2), TRAP::Math::Vec3(8, 9, 10), TRAP::Math::Epsilon<float>())) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMat3ColSet()
    {
        int32_t error = 0;

        TRAP::Math::Mat3 m(1);

        m = TRAP::Math::Column(m, 0, TRAP::Math::Vec3(0, 1, 2));
        m = TRAP::Math::Column(m, 1, TRAP::Math::Vec3(4, 5, 6));
        m = TRAP::Math::Column(m, 2, TRAP::Math::Vec3(8, 9, 10));

        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 0), TRAP::Math::Vec3(0, 1, 2), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 1), TRAP::Math::Vec3(4, 5, 6), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 2), TRAP::Math::Vec3(8, 9, 10), TRAP::Math::Epsilon<float>())) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMat4RowSet()
    {
        int32_t error = 0;

        TRAP::Math::Mat4 m(1);

        m = TRAP::Math::Row(m, 0, TRAP::Math::Vec4(0, 1, 2, 3));
        m = TRAP::Math::Row(m, 1, TRAP::Math::Vec4(4, 5, 6, 7));
        m = TRAP::Math::Row(m, 2, TRAP::Math::Vec4(8, 9, 10, 11));
        m = TRAP::Math::Row(m, 3, TRAP::Math::Vec4(12, 13, 14, 15));

        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(m, 0), TRAP::Math::Vec4(0, 1, 2, 3), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(m, 1), TRAP::Math::Vec4(4, 5, 6, 7), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(m, 2), TRAP::Math::Vec4(8, 9, 10, 11), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Row(m, 3), TRAP::Math::Vec4(12, 13, 14, 15), TRAP::Math::Epsilon<float>())) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMat4ColSet()
    {
        int32_t error = 0;

        TRAP::Math::Mat4 m(1);

        m = TRAP::Math::Column(m, 0, TRAP::Math::Vec4(0, 1, 2, 3));
        m = TRAP::Math::Column(m, 1, TRAP::Math::Vec4(4, 5, 6, 7));
        m = TRAP::Math::Column(m, 2, TRAP::Math::Vec4(8, 9, 10, 11));
        m = TRAP::Math::Column(m, 3, TRAP::Math::Vec4(12, 13, 14, 15));

        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 0), TRAP::Math::Vec4(0, 1, 2, 3), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 1), TRAP::Math::Vec4(4, 5, 6, 7), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 2), TRAP::Math::Vec4(8, 9, 10, 11), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Column(m, 3), TRAP::Math::Vec4(12, 13, 14, 15), TRAP::Math::Epsilon<float>())) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMat3RowGet()
    {
        int32_t error = 0;

        TRAP::Math::Mat3 m(1);

        TRAP::Math::Vec3 a = TRAP::Math::Row(m, 0);
        error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec3(1, 0, 0), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        TRAP::Math::Vec3 b = TRAP::Math::Row(m, 1);
        error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec3(0, 1, 0), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        TRAP::Math::Vec3 c = TRAP::Math::Row(m, 2);
        error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec3(0, 0, 1), TRAP::Math::Epsilon<float>())) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMat3ColGet()
    {
        int32_t error = 0;

        TRAP::Math::Mat3 m(1);

        TRAP::Math::Vec3 a = TRAP::Math::Column(m, 0);
        error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec3(1, 0, 0), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        TRAP::Math::Vec3 b = TRAP::Math::Column(m, 1);
        error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec3(0, 1, 0), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        TRAP::Math::Vec3 c = TRAP::Math::Column(m, 2);
        error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec3(0, 0, 1), TRAP::Math::Epsilon<float>())) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMat4RowGet()
    {
        int32_t error = 0;

        TRAP::Math::Mat4 m(1);

        TRAP::Math::Vec4 a = TRAP::Math::Row(m, 0);
        error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec4(1, 0, 0, 0), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        TRAP::Math::Vec4 b = TRAP::Math::Row(m, 1);
        error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec4(0, 1, 0, 0), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        TRAP::Math::Vec4 c = TRAP::Math::Row(m, 2);
        error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec4(0, 0, 1, 0), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        TRAP::Math::Vec4 d = TRAP::Math::Row(m, 3);
        error += TRAP::Math::All(TRAP::Math::Equal(d, TRAP::Math::Vec4(0, 0, 0, 1), TRAP::Math::Epsilon<float>())) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMat4ColGet()
    {
        int32_t error = 0;

        TRAP::Math::Mat4 m(1);

        TRAP::Math::Vec4 a = TRAP::Math::Column(m, 0);
        error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec4(1, 0, 0, 0), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        TRAP::Math::Vec4 b = TRAP::Math::Column(m, 1);
        error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec4(0, 1, 0, 0), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        TRAP::Math::Vec4 c = TRAP::Math::Column(m, 2);
        error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec4(0, 0, 1, 0), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        TRAP::Math::Vec4 d = TRAP::Math::Column(m, 3);
        error += TRAP::Math::All(TRAP::Math::Equal(d, TRAP::Math::Vec4(0, 0, 0, 1), TRAP::Math::Epsilon<float>())) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestToString()
    {
        int32_t error = 0;

        TRAP::Math::Mat3 a1(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
        std::string a2 = a1.ToString();
        error += a2 != std::string("Mat3f((1.000000, 2.000000, 3.000000), (4.000000, 5.000000, 6.000000), (7.000000, 8.000000, 9.000000))") ? 1 : 0;

        TRAP::Math::Mat4 b1(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0);
        std::string b2 = b1.ToString();
        error += b2 != std::string("Mat4f((1.000000, 2.000000, 3.000000, 4.000000), (5.000000, 6.000000, 7.000000, 8.000000), (9.000000, 10.000000, 11.000000, 12.000000), (13.000000, 14.000000, 15.000000, 16.000000))") ? 1 : 0;

        TRAP::Math::Mat3d c1(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
        std::string c2 = c1.ToString();
        error += c2 != std::string("Mat3d((1.000000, 2.000000, 3.000000), (4.000000, 5.000000, 6.000000), (7.000000, 8.000000, 9.000000))") ? 1 : 0;

        TRAP::Math::Mat4d d1(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0);
        std::string d2 = d1.ToString();
        error += d2 != std::string("Mat4d((1.000000, 2.000000, 3.000000, 4.000000), (5.000000, 6.000000, 7.000000, 8.000000), (9.000000, 10.000000, 11.000000, 12.000000), (13.000000, 14.000000, 15.000000, 16.000000))") ? 1 : 0;

        return error;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunFuncMatrixTests()
{
    int32_t error = 0;

    error += FuncMatrix::TestMatrixCompMult();
    FuncMatrix::TestOuterProduct();
    error += FuncMatrix::TestTranspose();
    error += FuncMatrix::TestInverse();

    error += FuncMatrix::TestEqual<TRAP::Math::Mat3, TRAP::Math::Vec3>();
    error += FuncMatrix::TestEqual<TRAP::Math::Mat4, TRAP::Math::Vec4>();
    error += FuncMatrix::TestEqual<TRAP::Math::Mat3d, TRAP::Math::Vec3d>();
    error += FuncMatrix::TestEqual<TRAP::Math::Mat4d, TRAP::Math::Vec4d>();

    error += FuncMatrix::TestNotEqual<TRAP::Math::Mat3, TRAP::Math::Vec3>();
    error += FuncMatrix::TestNotEqual<TRAP::Math::Mat4, TRAP::Math::Vec4>();
    error += FuncMatrix::TestNotEqual<TRAP::Math::Mat3d, TRAP::Math::Vec3d>();
    error += FuncMatrix::TestNotEqual<TRAP::Math::Mat4d, TRAP::Math::Vec4d>();

    error += FuncMatrix::TestTranslate();
    error += FuncMatrix::TestScale();
    error += FuncMatrix::TestRotate();

    error += FuncMatrix::TestMat3RowSet();
    error += FuncMatrix::TestMat3ColSet();
    error += FuncMatrix::TestMat4RowSet();
    error += FuncMatrix::TestMat4ColSet();

    error += FuncMatrix::TestMat3RowGet();
    error += FuncMatrix::TestMat3ColGet();
    error += FuncMatrix::TestMat4RowGet();
    error += FuncMatrix::TestMat4ColGet();

    error += FuncMatrix::TestToString();

    return error;
}