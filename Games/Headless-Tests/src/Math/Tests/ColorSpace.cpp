#include "ColorSpace.h"

#include <TRAP.h>

namespace ColorSpace
{
    int32_t TestSRGB()
    {
        int32_t error = 0;

        const TRAP::Math::Vec3 colorSrcRGB(1.0, 0.5, 0.0);

        {
            const TRAP::Math::Vec3 colorSRGB = TRAP::Math::ConvertLinearToSRGB(colorSrcRGB);
            const TRAP::Math::Vec3 colorRGB = TRAP::Math::ConvertSRGBToLinear(colorSRGB);
            error += TRAP::Math::All(TRAP::Math::Equal(colorSrcRGB, colorRGB, 0.00001f)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec3 colorSRGB = TRAP::Math::ConvertLinearToSRGB(colorSrcRGB, 2.8f);
            const TRAP::Math::Vec3 colorRGB = TRAP::Math::ConvertSRGBToLinear(colorSRGB, 2.8f);
            error += TRAP::Math::All(TRAP::Math::Equal(colorSrcRGB, colorRGB, 0.00001f)) ? 0 : 1;
        }

        const TRAP::Math::Vec4 colorSrcRGBA(1.0, 0.5, 0.0, 1.0);

        {
            const TRAP::Math::Vec4 colorSRGB = TRAP::Math::ConvertLinearToSRGB(colorSrcRGBA);
            const TRAP::Math::Vec4 colorRGB = TRAP::Math::ConvertSRGBToLinear(colorSRGB);
            error += TRAP::Math::All(TRAP::Math::Equal(colorSrcRGBA, colorRGB, 0.00001f)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4 colorSRGB = TRAP::Math::ConvertLinearToSRGB(colorSrcRGBA, 2.8f);
            const TRAP::Math::Vec4 colorRGB = TRAP::Math::ConvertSRGBToLinear(colorSRGB, 2.8f);
            error += TRAP::Math::All(TRAP::Math::Equal(colorSrcRGBA, colorRGB, 0.00001f)) ? 0 : 1;
        }

        const TRAP::Math::Vec4 colorSrcGNI = TRAP::Math::Vec4(107, 107, 104, 131) / TRAP::Math::Vec4(255);

        {
            [[maybe_unused]] const TRAP::Math::Vec4 colorGNA = TRAP::Math::ConvertSRGBToLinear(colorSrcGNI) * TRAP::Math::Vec4(255);
            [[maybe_unused]] const TRAP::Math::Vec4 colorGNE = TRAP::Math::ConvertLinearToSRGB(colorSrcGNI) * TRAP::Math::Vec4(255);
            const TRAP::Math::Vec4 colorSRGB = TRAP::Math::ConvertLinearToSRGB(colorSrcGNI);
            const TRAP::Math::Vec4 colorRGB = TRAP::Math::ConvertSRGBToLinear(colorSRGB);
            error += TRAP::Math::All(TRAP::Math::Equal(colorSrcGNI, colorRGB, 0.00001f)) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestSaturation()
    {
        int32_t error = 0;

        [[maybe_unused]] TRAP::Math::Vec4 color = TRAP::Math::Saturation(1.0f, TRAP::Math::Vec4(1.0, 0.5, 0.0, 1.0));

        return error;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunColorSpaceTests()
{
    int32_t error = 0;

    error += ColorSpace::TestSRGB();
    error += ColorSpace::TestSaturation();

    return error;
}