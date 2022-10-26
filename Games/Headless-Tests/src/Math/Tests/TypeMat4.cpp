#include "TypeMat4.h"

#include <TRAP.h>

namespace TypeMat4
{
    int32_t TestConstructor()
    {
        int32_t error = 0;

        const TRAP::Math::Mat4 m0(TRAP::Math::Vec4(0, 1, 2, 3), TRAP::Math::Vec4(4, 5, 6, 7),
                                TRAP::Math::Vec4(8, 9, 10, 11), TRAP::Math::Vec4(12, 13, 14, 15));

        TRAP_ASSERT(sizeof(m0) == 4 * 4 * 4);

        const TRAP::Math::Mat4 m1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

        const TRAP::Math::Mat4 m2
        {
            {0, 1, 2, 3},
            {4, 5, 6, 7},
            {8, 9, 10, 11},
            {12, 13, 14, 15}
        };

        error += TRAP::Math::All(TRAP::Math::Equal(m0, m2, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(m1, m2, TRAP::Math::Epsilon<float>())) ? 0 : 1;

        const std::vector<TRAP::Math::Mat4> m3
        {
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
        };

        const TRAP::Math::Mat4 m4
        {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}
        };

        error += TRAP::Math::Equal(m4[0][0], 1.0f, 0.0001f) ? 0 : 1;
        error += TRAP::Math::Equal(m4[3][3], 1.0f, 0.0001f) ? 0 : 1;

        const std::vector<TRAP::Math::Mat4> v1
        {
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
        };

        const std::vector<TRAP::Math::Mat4> v2
        {
            {
                {0, 1, 2, 3},
                {4, 5, 6, 7},
                {8, 9, 10, 11},
                {12, 13, 14, 15}
            },
            {
                {0, 1, 2, 3},
                {4, 5, 6, 7},
                {8, 9, 10, 11},
                {12, 13, 14, 15}
            }
        };

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    template<typename matType, typename vecType>
    int32_t TestOperators()
    {
        using valueType = typename matType::valueType;

        const valueType epsilon = static_cast<valueType>(0.001);

        int32_t error = 0;

        const matType m(static_cast<valueType>(2.0f));
        const matType n(static_cast<valueType>(1.0f));
        const vecType u(static_cast<valueType>(2.0f));

        {
            const matType p = n * static_cast<valueType>(2.0f);
            error += TRAP::Math::All(TRAP::Math::Equal(p, m, epsilon)) ? 0 : 1;

            const matType q = m / static_cast<valueType>(2.0f);
            error += TRAP::Math::All(TRAP::Math::Equal(q, n, epsilon)) ? 0 : 1;
        }

        {
            const vecType v = m * u;
            error += TRAP::Math::All(TRAP::Math::Equal(v, vecType(static_cast<valueType>(4.0f)), epsilon)) ? 0 : 1;

            const vecType w = u / m;
            error += TRAP::Math::All(TRAP::Math::Equal(w, vecType(static_cast<valueType>(1.0f)), epsilon)) ? 0 : 1;
        }

        {
            const matType o = m * n;
            error += TRAP::Math::All(TRAP::Math::Equal(o, matType(static_cast<valueType>(2.0f)), epsilon)) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    template<typename matType>
    int32_t TestInverse()
    {
        using valueType = typename matType::valueType;

        const valueType epsilon = static_cast<valueType>(0.001);

        int32_t error = 0;

        const matType identity(static_cast<valueType>(1.0f));
        const matType matrix(TRAP::Math::Vec4(0.6f, 0.2f, 0.3f, 0.4f), TRAP::Math::Vec4(0.2f, 0.7f, 0.5f, 0.3f),
                             TRAP::Math::Vec4(0.3f, 0.5f, 0.7f, 0.2f), TRAP::Math::Vec4(0.4f, 0.3f, 0.2f, 0.6f));
        const matType inverse = identity / matrix;
        const matType result = matrix * inverse;

        error += TRAP::Math::All(TRAP::Math::Equal(identity, result, epsilon)) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestSize()
    {
        int32_t error = 0;

        error += 64 == sizeof(TRAP::Math::Mat4) ? 0 : 1;
        error += 128 == sizeof(TRAP::Math::Mat4d) ? 0 : 1;
        error += TRAP::Math::Mat4().Length() == 4 ? 0 : 1;
        error += TRAP::Math::Mat4d().Length() == 4 ? 0 : 1;
        error += TRAP::Math::Mat4::Length() == 4 ? 0 : 1;
        error += TRAP::Math::Mat4d::Length() == 4 ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    void TestConstexpr()
    {
        static_assert(TRAP::Math::Mat4::Length() == 4, "MathTests: Failed constexpr");
        static constexpr TRAP::Math::Mat4 a(1.0f);
        static constexpr TRAP::Math::Mat4 b(1.0f);
        static constexpr TRAP::Math::Vec4b c = TRAP::Math::Equal(a, b, 0.01f);
        static_assert(TRAP::Math::All(c), "MathTests: Failed constexpr");
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunTypeMat4Tests()
{
    int32_t error = 0;

    error += TypeMat4::TestConstructor();
    error += TypeMat4::TestOperators<TRAP::Math::Mat4, TRAP::Math::Vec4>();
    error += TypeMat4::TestOperators<TRAP::Math::Mat4d, TRAP::Math::Vec4d>();
    error += TypeMat4::TestInverse<TRAP::Math::Mat4>();
    error += TypeMat4::TestInverse<TRAP::Math::Mat4d>();
    error += TypeMat4::TestSize();
    TypeMat4::TestConstexpr();

    return error;
}