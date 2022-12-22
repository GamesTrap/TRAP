// MIT License

// Copyright (c) 2019 DeanoC

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Modified by Jan "GamesTrap" Schuerkamp

#ifndef TRAP_IMAGEFORMAT_H
#define TRAP_IMAGEFORMAT_H

#include <cstdint>

namespace TRAP::Graphics::API
{
    enum class ImageFormat
    {
        Undefined = 0,
        R1_UNORM = 1,
        R2_UNORM = 2,
        R4_UNORM = 3,
        R4G4_UNORM = 4,
        G4R4_UNORM = 5,
        A8_UNORM = 6,
        R8_UNORM = 7,
        R8_SNORM = 8,
        R8_UINT = 9,
        R8_SINT = 10,
        R8_SRGB = 11,
        B2G3R3_UNORM = 12,
        R4G4B4A4_UNORM = 13,
        R4G4B4X4_UNORM = 14,
        B4G4R4A4_UNORM = 15,
        B4G4R4X4_UNORM = 16,
        A4R4G4B4_UNORM = 17,
        X4R4G4B4_UNORM = 18,
        A4B4G4R4_UNORM = 19,
        X4B4G4R4_UNORM = 20,
        R5G6B5_UNORM = 21,
        B5G6R5_UNORM = 22,
        R5G5B5A1_UNORM = 23,
        B5G5R5A1_UNORM = 24,
        A1B5G5R5_UNORM = 25,
        A1R5G5B5_UNORM = 26,
        R5G5B5X1_UNORM = 27,
        B5G5R5X1_UNORM = 28,
        X1R5G5B5_UNORM = 29,
        X1B5G5R5_UNORM = 30,
        B2G3R3A8_UNORM = 31,
        R8G8_UNORM = 32,
        R8G8_SNORM = 33,
        G8R8_UNORM = 34,
        G8R8_SNORM = 35,
        R8G8_UINT = 36,
        R8G8_SINT = 37,
        R8G8_SRGB = 38,
        R16_UNORM = 39,
        R16_SNORM = 40,
        R16_UINT = 41,
        R16_SINT = 42,
        R16_SFLOAT = 43,
        R16_SBFLOAT = 44,
        R8G8B8_UNORM = 45,
        R8G8B8_SNORM = 46,
        R8G8B8_UINT = 47,
        R8G8B8_SINT = 48,
        R8G8B8_SRGB = 49,
        B8G8R8_UNORM = 50,
        B8G8R8_SNORM = 51,
        B8G8R8_UINT = 52,
        B8G8R8_SINT = 53,
        B8G8R8_SRGB = 54,
        R8G8B8A8_UNORM = 55,
        R8G8B8A8_SNORM = 56,
        R8G8B8A8_UINT = 57,
        R8G8B8A8_SINT = 58,
        R8G8B8A8_SRGB = 59,
        B8G8R8A8_UNORM = 60,
        B8G8R8A8_SNORM = 61,
        B8G8R8A8_UINT = 62,
        B8G8R8A8_SINT = 63,
        B8G8R8A8_SRGB = 64,
        R8G8B8X8_UNORM = 65,
        B8G8R8X8_UNORM = 66,
        R16G16_UNORM = 67,
        G16R16_UNORM = 68,
        R16G16_SNORM = 69,
        G16R16_SNORM = 70,
        R16G16_UINT = 71,
        R16G16_SINT = 72,
        R16G16_SFLOAT = 73,
        R16G16_SBFLOAT = 74,
        R32_UINT = 75,
        R32_SINT = 76,
        R32_SFLOAT = 77,
        A2R10G10B10_UNORM = 78,
        A2R10G10B10_UINT = 79,
        A2R10G10B10_SNORM = 80,
        A2R10G10B10_SINT = 81,
        A2B10G10R10_UNORM = 82,
        A2B10G10R10_UINT = 83,
        A2B10G10R10_SNORM = 84,
        A2B10G10R10_SINT = 85,
        R10G10B10A2_UNORM = 86,
        R10G10B10A2_UINT = 87,
        R10G10B10A2_SNORM = 88,
        R10G10B10A2_SINT = 89,
        B10G10R10A2_UNORM = 90,
        B10G10R10A2_UINT = 91,
        B10G10R10A2_SNORM = 92,
        B10G10R10A2_SINT = 93,
        B10G11R11_UFLOAT = 94,
        E5B9G9R9_UFLOAT = 95,
        R16G16B16_UNORM = 96,
        R16G16B16_SNORM = 97,
        R16G16B16_UINT = 98,
        R16G16B16_SINT = 99,
        R16G16B16_SFLOAT = 100,
        R16G16B16_SBFLOAT = 101,
        R16G16B16A16_UNORM = 102,
        R16G16B16A16_SNORM = 103,
        R16G16B16A16_UINT = 104,
        R16G16B16A16_SINT = 105,
        R16G16B16A16_SFLOAT = 106,
        R16G16B16A16_SBFLOAT = 107,
        R32G32_UINT = 108,
        R32G32_SINT = 109,
        R32G32_SFLOAT = 110,
        R32G32B32_UINT = 111,
        R32G32B32_SINT = 112,
        R32G32B32_SFLOAT = 113,
        R32G32B32A32_UINT = 114,
        R32G32B32A32_SINT = 115,
        R32G32B32A32_SFLOAT = 116,
        R64_UINT = 117,
        R64_SINT = 118,
        R64_SFLOAT = 119,
        R64G64_UINT = 120,
        R64G64_SINT = 121,
        R64G64_SFLOAT = 122,
        R64G64B64_UINT = 123,
        R64G64B64_SINT = 124,
        R64G64B64_SFLOAT = 125,
        R64G64B64A64_UINT = 126,
        R64G64B64A64_SINT = 127,
        R64G64B64A64_SFLOAT = 128,
        D16_UNORM = 129,
        X8_D24_UNORM = 130,
        D32_SFLOAT = 131,
        S8_UINT = 132,
        D16_UNORM_S8_UINT = 133,
        D24_UNORM_S8_UINT = 134,
        D32_SFLOAT_S8_UINT = 135,
        DXBC1_RGB_UNORM = 136,
        DXBC1_RGB_SRGB = 137,
        DXBC1_RGBA_UNORM = 138,
        DXBC1_RGBA_SRGB = 139,
        DXBC2_UNORM = 140,
        DXBC2_SRGB = 141,
        DXBC3_UNORM = 142,
        DXBC3_SRGB = 143,
        DXBC4_UNORM = 144,
        DXBC4_SNORM = 145,
        DXBC5_UNORM = 146,
        DXBC5_SNORM = 147,
        DXBC6H_UFLOAT = 148,
        DXBC6H_SFLOAT = 149,
        DXBC7_UNORM = 150,
        DXBC7_SRGB = 151,
        PVRTC1_2BPP_UNORM = 152,
        PVRTC1_4BPP_UNORM = 153,
        PVRTC2_2BPP_UNORM = 154,
        PVRTC2_4BPP_UNORM = 155,
        PVRTC1_2BPP_SRGB = 156,
        PVRTC1_4BPP_SRGB = 157,
        PVRTC2_2BPP_SRGB = 158,
        PVRTC2_4BPP_SRGB = 159,
        ETC2_R8G8B8_UNORM = 160,
        ETC2_R8G8B8_SRGB = 161,
        ETC2_R8G8B8A1_UNORM = 162,
        ETC2_R8G8B8A1_SRGB = 163,
        ETC2_R8G8B8A8_UNORM = 164,
        ETC2_R8G8B8A8_SRGB = 165,
        ETC2_EAC_R11_UNORM = 166,
        ETC2_EAC_R11_SNORM = 167,
        ETC2_EAC_R11G11_UNORM = 168,
        ETC2_EAC_R11G11_SNORM = 169,
        ASTC_4x4_UNORM = 170,
        ASTC_4x4_SRGB = 171,
        ASTC_5x4_UNORM = 172,
        ASTC_5x4_SRGB = 173,
        ASTC_5x5_UNORM = 174,
        ASTC_5x5_SRGB = 175,
        ASTC_6x5_UNORM = 176,
        ASTC_6x5_SRGB = 177,
        ASTC_6x6_UNORM = 178,
        ASTC_6x6_SRGB = 179,
        ASTC_8x5_UNORM = 180,
        ASTC_8x5_SRGB = 181,
        ASTC_8x6_UNORM = 182,
        ASTC_8x6_SRGB = 183,
        ASTC_8x8_UNORM = 184,
        ASTC_8x8_SRGB = 185,
        ASTC_10x5_UNORM = 186,
        ASTC_10x5_SRGB = 187,
        ASTC_10x6_UNORM = 188,
        ASTC_10x6_SRGB = 189,
        ASTC_10x8_UNORM = 190,
        ASTC_10x8_SRGB = 191,
        ASTC_10x10_UNORM = 192,
        ASTC_10x10_SRGB = 193,
        ASTC_12x10_UNORM = 194,
        ASTC_12x10_SRGB = 195,
        ASTC_12x12_UNORM = 196,
        ASTC_12x12_SRGB = 197,
        CLUT_P4 = 198,
        CLUT_P4A4 = 199,
        CLUT_P8 = 200,
        CLUT_P8A8 = 201,
        R4G4B4A4_UNORM_PACK16 = 202,
        B4G4R4A4_UNORM_PACK16 = 203,
        R5G6B5_UNORM_PACK16 = 204,
        B5G6R5_UNORM_PACK16 = 205,
        R5G5B5A1_UNORM_PACK16 = 206,
        B5G5R5A1_UNORM_PACK16 = 207,
        A1R5G5B5_UNORM_PACK16 = 208,
        G16B16G16R16_422_UNORM = 209,
        B16G16R16G16_422_UNORM = 210,
        R12X4G12X4B12X4A12X4_UNORM_4PACK16 = 211,
        G12X4B12X4G12X4R12X4_422_UNORM_4PACK16 = 212,
        B12X4G12X4R12X4G12X4_422_UNORM_4PACK16 = 213,
        R10X6G10X6B10X6A10X6_UNORM_4PACK16 = 214,
        G10X6B10X6G10X6R10X6_422_UNORM_4PACK16 = 215,
        B10X6G10X6R10X6G10X6_422_UNORM_4PACK16 = 216,
        G8B8G8R8_422_UNORM = 217,
        B8G8R8G8_422_UNORM = 218,
        G8_B8_R8_3PLANE_420_UNORM = 219,
        G8_B8R8_2PLANE_420_UNORM = 220,
        G8_B8_R8_3PLANE_422_UNORM = 221,
        G8_B8R8_2PLANE_422_UNORM = 222,
        G8_B8_R8_3PLANE_444_UNORM = 223,
        G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 = 224,
        G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 = 225,
        G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 = 226,
        G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 = 227,
        G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 = 228,
        G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 = 229,
        G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 = 230,
        G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 = 231,
        G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 = 232,
        G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 = 233,
        G16_B16_R16_3PLANE_420_UNORM = 234,
        G16_B16_R16_3PLANE_422_UNORM = 235,
        G16_B16_R16_3PLANE_444_UNORM = 236,
        G16_B16R16_2PLANE_420_UNORM = 237,
        G16_B16R16_2PLANE_422_UNORM = 238,

        IMAGE_FORMAT_COUNT = 239
    };

    /// <summary>
    /// Retrieve whether the given image format has only a depth channel.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <returns>True if the format has only depth, false otherwise.</returns>
    [[nodiscard]] inline constexpr bool ImageFormatIsDepthOnly(const ImageFormat fmt) noexcept
    {
        switch (fmt)
        {
        case ImageFormat::D16_UNORM:
            return true;

        case ImageFormat::X8_D24_UNORM:
            return true;

        case ImageFormat::D32_SFLOAT:
            return true;

        default:
            return false;
        }
    }
    /// <summary>
    /// Retrieve whether the given image format has depth and stencil channels.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <returns>True if the format has depth and stencil, false otherwise.</returns>
    [[nodiscard]] inline constexpr bool ImageFormatIsDepthAndStencil(const ImageFormat fmt) noexcept
    {
        switch (fmt)
        {
        case ImageFormat::D16_UNORM_S8_UINT:
            return true;

        case ImageFormat::D24_UNORM_S8_UINT:
            return true;

        case ImageFormat::D32_SFLOAT_S8_UINT:
            return true;

        default:
            return false;
        }
    }
    /// <summary>
    /// Retrieve whether the given image format has only a stencil channel.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <returns>True if the format has only stencil, false otherwise.</returns>
    [[nodiscard]] inline constexpr bool ImageFormatIsStencilOnly(const ImageFormat fmt) noexcept
    {
        switch(fmt)
        {
        case ImageFormat::S8_UINT:
            return true;

        default:
            return false;
        }
    }
    /// <summary>
    /// Retrieve whether the given image format has a stencil channel.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <returns>True if the format has stencil, false otherwise.</returns>
    [[nodiscard]] inline constexpr bool ImageFormatHasStencil(const ImageFormat fmt) noexcept
    {
        return ImageFormatIsStencilOnly(fmt) || ImageFormatIsDepthAndStencil(fmt);
    }
    /// <summary>
    /// Retrieve the bit size of a single block from the given image format.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <returns>Bit size of a single block.</returns>
    [[nodiscard]] inline constexpr uint32_t ImageFormatBitSizeOfBlock(const ImageFormat fmt) noexcept
    {
        switch(fmt)
        {
        case ImageFormat::Undefined: return 0;
        case ImageFormat::R1_UNORM: return 8;
        case ImageFormat::R2_UNORM: return 8;
        case ImageFormat::R4_UNORM: return 8;
        case ImageFormat::R4G4_UNORM: return 8;
        case ImageFormat::G4R4_UNORM: return 8;
        case ImageFormat::A8_UNORM: return 8;
        case ImageFormat::R8_UNORM: return 8;
        case ImageFormat::R8_SNORM: return 8;
        case ImageFormat::R8_UINT: return 8;
        case ImageFormat::R8_SINT: return 8;
        case ImageFormat::R8_SRGB: return 8;
        case ImageFormat::B2G3R3_UNORM: return 8;
        case ImageFormat::R4G4B4A4_UNORM: return 16;
        case ImageFormat::R4G4B4X4_UNORM: return 16;
        case ImageFormat::B4G4R4A4_UNORM: return 16;
        case ImageFormat::B4G4R4X4_UNORM: return 16;
        case ImageFormat::A4R4G4B4_UNORM: return 16;
        case ImageFormat::X4R4G4B4_UNORM: return 16;
        case ImageFormat::A4B4G4R4_UNORM: return 16;
        case ImageFormat::X4B4G4R4_UNORM: return 16;
        case ImageFormat::R5G6B5_UNORM: return 16;
        case ImageFormat::B5G6R5_UNORM: return 16;
        case ImageFormat::R5G5B5A1_UNORM: return 16;
        case ImageFormat::B5G5R5A1_UNORM: return 16;
        case ImageFormat::A1B5G5R5_UNORM: return 16;
        case ImageFormat::A1R5G5B5_UNORM: return 16;
        case ImageFormat::R5G5B5X1_UNORM: return 16;
        case ImageFormat::B5G5R5X1_UNORM: return 16;
        case ImageFormat::X1R5G5B5_UNORM: return 16;
        case ImageFormat::X1B5G5R5_UNORM: return 16;
        case ImageFormat::B2G3R3A8_UNORM: return 16;
        case ImageFormat::R8G8_UNORM: return 16;
        case ImageFormat::R8G8_SNORM: return 16;
        case ImageFormat::G8R8_UNORM: return 16;
        case ImageFormat::G8R8_SNORM: return 16;
        case ImageFormat::R8G8_UINT: return 16;
        case ImageFormat::R8G8_SINT: return 16;
        case ImageFormat::R8G8_SRGB: return 16;
        case ImageFormat::R16_UNORM: return 16;
        case ImageFormat::R16_SNORM: return 16;
        case ImageFormat::R16_UINT: return 16;
        case ImageFormat::R16_SINT: return 16;
        case ImageFormat::R16_SFLOAT: return 16;
        case ImageFormat::R16_SBFLOAT: return 16;
        case ImageFormat::R8G8B8_UNORM: return 24;
        case ImageFormat::R8G8B8_SNORM: return 24;
        case ImageFormat::R8G8B8_UINT: return 24;
        case ImageFormat::R8G8B8_SINT: return 24;
        case ImageFormat::R8G8B8_SRGB: return 24;
        case ImageFormat::B8G8R8_UNORM: return 24;
        case ImageFormat::B8G8R8_SNORM: return 24;
        case ImageFormat::B8G8R8_UINT: return 24;
        case ImageFormat::B8G8R8_SINT: return 24;
        case ImageFormat::B8G8R8_SRGB: return 24;
        case ImageFormat::R16G16B16_UNORM: return 48;
        case ImageFormat::R16G16B16_SNORM: return 48;
        case ImageFormat::R16G16B16_UINT: return 48;
        case ImageFormat::R16G16B16_SINT: return 48;
        case ImageFormat::R16G16B16_SFLOAT: return 48;
        case ImageFormat::R16G16B16_SBFLOAT: return 48;
        case ImageFormat::R16G16B16A16_UNORM: return 64;
        case ImageFormat::R16G16B16A16_SNORM: return 64;
        case ImageFormat::R16G16B16A16_UINT: return 64;
        case ImageFormat::R16G16B16A16_SINT: return 64;
        case ImageFormat::R16G16B16A16_SFLOAT: return 64;
        case ImageFormat::R16G16B16A16_SBFLOAT: return 64;
        case ImageFormat::R32G32_UINT: return 64;
        case ImageFormat::R32G32_SINT: return 64;
        case ImageFormat::R32G32_SFLOAT: return 64;
        case ImageFormat::R32G32B32_UINT: return 96;
        case ImageFormat::R32G32B32_SINT: return 96;
        case ImageFormat::R32G32B32_SFLOAT: return 96;
        case ImageFormat::R32G32B32A32_UINT: return 128;
        case ImageFormat::R32G32B32A32_SINT: return 128;
        case ImageFormat::R32G32B32A32_SFLOAT: return 128;
        case ImageFormat::R64_UINT: return 64;
        case ImageFormat::R64_SINT: return 64;
        case ImageFormat::R64_SFLOAT: return 64;
        case ImageFormat::R64G64_UINT: return 128;
        case ImageFormat::R64G64_SINT: return 128;
        case ImageFormat::R64G64_SFLOAT: return 128;
        case ImageFormat::R64G64B64_UINT: return 192;
        case ImageFormat::R64G64B64_SINT: return 192;
        case ImageFormat::R64G64B64_SFLOAT: return 192;
        case ImageFormat::R64G64B64A64_UINT: return 256;
        case ImageFormat::R64G64B64A64_SINT: return 256;
        case ImageFormat::R64G64B64A64_SFLOAT: return 256;
        case ImageFormat::D16_UNORM: return 16;
        case ImageFormat::S8_UINT: return 8;
        case ImageFormat::D32_SFLOAT_S8_UINT: return 64;
        case ImageFormat::DXBC1_RGB_UNORM: return 64;
        case ImageFormat::DXBC1_RGB_SRGB: return 64;
        case ImageFormat::DXBC1_RGBA_UNORM: return 64;
        case ImageFormat::DXBC1_RGBA_SRGB: return 64;
        case ImageFormat::DXBC2_UNORM: return 128;
        case ImageFormat::DXBC2_SRGB: return 128;
        case ImageFormat::DXBC3_UNORM: return 128;
        case ImageFormat::DXBC3_SRGB: return 128;
        case ImageFormat::DXBC4_UNORM: return 64;
        case ImageFormat::DXBC4_SNORM: return 64;
        case ImageFormat::DXBC5_UNORM: return 128;
        case ImageFormat::DXBC5_SNORM: return 128;
        case ImageFormat::DXBC6H_UFLOAT: return 128;
        case ImageFormat::DXBC6H_SFLOAT: return 128;
        case ImageFormat::DXBC7_UNORM: return 128;
        case ImageFormat::DXBC7_SRGB: return 128;
        case ImageFormat::PVRTC1_2BPP_UNORM: return 64;
        case ImageFormat::PVRTC1_4BPP_UNORM: return 64;
        case ImageFormat::PVRTC2_2BPP_UNORM: return 64;
        case ImageFormat::PVRTC2_4BPP_UNORM: return 64;
        case ImageFormat::PVRTC1_2BPP_SRGB: return 64;
        case ImageFormat::PVRTC1_4BPP_SRGB: return 64;
        case ImageFormat::PVRTC2_2BPP_SRGB: return 64;
        case ImageFormat::PVRTC2_4BPP_SRGB: return 64;
        case ImageFormat::ETC2_R8G8B8_UNORM: return 64;
        case ImageFormat::ETC2_R8G8B8_SRGB: return 64;
        case ImageFormat::ETC2_R8G8B8A1_UNORM: return 64;
        case ImageFormat::ETC2_R8G8B8A1_SRGB: return 64;
        case ImageFormat::ETC2_R8G8B8A8_UNORM: return 64;
        case ImageFormat::ETC2_R8G8B8A8_SRGB: return 64;
        case ImageFormat::ETC2_EAC_R11_UNORM: return 64;
        case ImageFormat::ETC2_EAC_R11_SNORM: return 64;
        case ImageFormat::ETC2_EAC_R11G11_UNORM: return 64;
        case ImageFormat::ETC2_EAC_R11G11_SNORM: return 64;
        case ImageFormat::ASTC_4x4_UNORM: return 128;
        case ImageFormat::ASTC_4x4_SRGB: return 128;
        case ImageFormat::ASTC_5x4_UNORM: return 128;
        case ImageFormat::ASTC_5x4_SRGB: return 128;
        case ImageFormat::ASTC_5x5_UNORM: return 128;
        case ImageFormat::ASTC_5x5_SRGB: return 128;
        case ImageFormat::ASTC_6x5_UNORM: return 128;
        case ImageFormat::ASTC_6x5_SRGB: return 128;
        case ImageFormat::ASTC_6x6_UNORM: return 128;
        case ImageFormat::ASTC_6x6_SRGB: return 128;
        case ImageFormat::ASTC_8x5_UNORM: return 128;
        case ImageFormat::ASTC_8x5_SRGB: return 128;
        case ImageFormat::ASTC_8x6_UNORM: return 128;
        case ImageFormat::ASTC_8x6_SRGB: return 128;
        case ImageFormat::ASTC_8x8_UNORM: return 128;
        case ImageFormat::ASTC_8x8_SRGB: return 128;
        case ImageFormat::ASTC_10x5_UNORM: return 128;
        case ImageFormat::ASTC_10x5_SRGB: return 128;
        case ImageFormat::ASTC_10x6_UNORM: return 128;
        case ImageFormat::ASTC_10x6_SRGB: return 128;
        case ImageFormat::ASTC_10x8_UNORM: return 128;
        case ImageFormat::ASTC_10x8_SRGB: return 128;
        case ImageFormat::ASTC_10x10_UNORM: return 128;
        case ImageFormat::ASTC_10x10_SRGB: return 128;
        case ImageFormat::ASTC_12x10_UNORM: return 128;
        case ImageFormat::ASTC_12x10_SRGB: return 128;
        case ImageFormat::ASTC_12x12_UNORM: return 128;
        case ImageFormat::ASTC_12x12_SRGB: return 128;
        case ImageFormat::CLUT_P4: return 8;
        case ImageFormat::CLUT_P4A4: return 8;
        case ImageFormat::CLUT_P8: return 8;
        case ImageFormat::CLUT_P8A8: return 16;
        case ImageFormat::G16B16G16R16_422_UNORM: return 8;
        case ImageFormat::B16G16R16G16_422_UNORM: return 8;
        case ImageFormat::R12X4G12X4B12X4A12X4_UNORM_4PACK16: return 8;
        case ImageFormat::G12X4B12X4G12X4R12X4_422_UNORM_4PACK16: return 8;
        case ImageFormat::B12X4G12X4R12X4G12X4_422_UNORM_4PACK16: return 8;
        case ImageFormat::R10X6G10X6B10X6A10X6_UNORM_4PACK16: return 8;
        case ImageFormat::G10X6B10X6G10X6R10X6_422_UNORM_4PACK16: return 8;
        case ImageFormat::B10X6G10X6R10X6G10X6_422_UNORM_4PACK16: return 8;
        case ImageFormat::G8B8G8R8_422_UNORM: return 4;
        case ImageFormat::B8G8R8G8_422_UNORM: return 4;
        default: return 32;
        }
    }
    /// <summary>
    /// Retrieve how many pixels are in the x dimension of a block of the given image format.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <returns>Pixel count.</returns>
    [[nodiscard]] inline constexpr uint32_t ImageFormatWidthOfBlock(const ImageFormat fmt) noexcept
    {
        switch(fmt)
        {
        case ImageFormat::Undefined: return 1;
        case ImageFormat::R1_UNORM: return 8;
        case ImageFormat::R2_UNORM: return 4;
        case ImageFormat::R4_UNORM: return 2;
        case ImageFormat::DXBC1_RGB_UNORM: return 4;
        case ImageFormat::DXBC1_RGB_SRGB: return 4;
        case ImageFormat::DXBC1_RGBA_UNORM: return 4;
        case ImageFormat::DXBC1_RGBA_SRGB: return 4;
        case ImageFormat::DXBC2_UNORM: return 4;
        case ImageFormat::DXBC2_SRGB: return 4;
        case ImageFormat::DXBC3_UNORM: return 4;
        case ImageFormat::DXBC3_SRGB: return 4;
        case ImageFormat::DXBC4_UNORM: return 4;
        case ImageFormat::DXBC4_SNORM: return 4;
        case ImageFormat::DXBC5_UNORM: return 4;
        case ImageFormat::DXBC5_SNORM: return 4;
        case ImageFormat::DXBC6H_UFLOAT: return 4;
        case ImageFormat::DXBC6H_SFLOAT: return 4;
        case ImageFormat::DXBC7_UNORM: return 4;
        case ImageFormat::DXBC7_SRGB: return 4;
        case ImageFormat::PVRTC1_2BPP_UNORM: return 8;
        case ImageFormat::PVRTC1_4BPP_UNORM: return 4;
        case ImageFormat::PVRTC2_2BPP_UNORM: return 8;
        case ImageFormat::PVRTC2_4BPP_UNORM: return 4;
        case ImageFormat::PVRTC1_2BPP_SRGB: return 8;
        case ImageFormat::PVRTC1_4BPP_SRGB: return 4;
        case ImageFormat::PVRTC2_2BPP_SRGB: return 8;
        case ImageFormat::PVRTC2_4BPP_SRGB: return 4;
        case ImageFormat::ETC2_R8G8B8_UNORM: return 4;
        case ImageFormat::ETC2_R8G8B8_SRGB: return 4;
        case ImageFormat::ETC2_R8G8B8A1_UNORM: return 4;
        case ImageFormat::ETC2_R8G8B8A1_SRGB: return 4;
        case ImageFormat::ETC2_R8G8B8A8_UNORM: return 4;
        case ImageFormat::ETC2_R8G8B8A8_SRGB: return 4;
        case ImageFormat::ETC2_EAC_R11_UNORM: return 4;
        case ImageFormat::ETC2_EAC_R11_SNORM: return 4;
        case ImageFormat::ETC2_EAC_R11G11_UNORM: return 4;
        case ImageFormat::ETC2_EAC_R11G11_SNORM: return 4;
        case ImageFormat::ASTC_4x4_UNORM: return 4;
        case ImageFormat::ASTC_4x4_SRGB: return 4;
        case ImageFormat::ASTC_5x4_UNORM: return 5;
        case ImageFormat::ASTC_5x4_SRGB: return 5;
        case ImageFormat::ASTC_5x5_UNORM: return 5;
        case ImageFormat::ASTC_5x5_SRGB: return 5;
        case ImageFormat::ASTC_6x5_UNORM: return 6;
        case ImageFormat::ASTC_6x5_SRGB: return 6;
        case ImageFormat::ASTC_6x6_UNORM: return 6;
        case ImageFormat::ASTC_6x6_SRGB: return 6;
        case ImageFormat::ASTC_8x5_UNORM: return 8;
        case ImageFormat::ASTC_8x5_SRGB: return 8;
        case ImageFormat::ASTC_8x6_UNORM: return 8;
        case ImageFormat::ASTC_8x6_SRGB: return 8;
        case ImageFormat::ASTC_8x8_UNORM: return 8;
        case ImageFormat::ASTC_8x8_SRGB: return 8;
        case ImageFormat::ASTC_10x5_UNORM: return 10;
        case ImageFormat::ASTC_10x5_SRGB: return 10;
        case ImageFormat::ASTC_10x6_UNORM: return 10;
        case ImageFormat::ASTC_10x6_SRGB: return 10;
        case ImageFormat::ASTC_10x8_UNORM: return 10;
        case ImageFormat::ASTC_10x8_SRGB: return 10;
        case ImageFormat::ASTC_10x10_UNORM: return 10;
        case ImageFormat::ASTC_10x10_SRGB: return 10;
        case ImageFormat::ASTC_12x10_UNORM: return 12;
        case ImageFormat::ASTC_12x10_SRGB: return 12;
        case ImageFormat::ASTC_12x12_UNORM: return 12;
        case ImageFormat::ASTC_12x12_SRGB: return 12;
        case ImageFormat::CLUT_P4: return 2;
        default: return 1;
        }
    }
    /// <summary>
    /// Retrieve how many pixels are in the y dimension of a block of the given image format.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <returns>Pixel count.</returns>
    [[nodiscard]] inline constexpr uint32_t ImageFormatHeightOfBlock(const ImageFormat fmt) noexcept
    {
        switch(fmt)
        {
        case ImageFormat::Undefined: return 1;
        case ImageFormat::DXBC1_RGB_UNORM: return 4;
        case ImageFormat::DXBC1_RGB_SRGB: return 4;
        case ImageFormat::DXBC1_RGBA_UNORM: return 4;
        case ImageFormat::DXBC1_RGBA_SRGB: return 4;
        case ImageFormat::DXBC2_UNORM: return 4;
        case ImageFormat::DXBC2_SRGB: return 4;
        case ImageFormat::DXBC3_UNORM: return 4;
        case ImageFormat::DXBC3_SRGB: return 4;
        case ImageFormat::DXBC4_UNORM: return 4;
        case ImageFormat::DXBC4_SNORM: return 4;
        case ImageFormat::DXBC5_UNORM: return 4;
        case ImageFormat::DXBC5_SNORM: return 4;
        case ImageFormat::DXBC6H_UFLOAT: return 4;
        case ImageFormat::DXBC6H_SFLOAT: return 4;
        case ImageFormat::DXBC7_UNORM: return 4;
        case ImageFormat::DXBC7_SRGB: return 4;
        case ImageFormat::PVRTC1_2BPP_UNORM: return 4;
        case ImageFormat::PVRTC1_4BPP_UNORM: return 4;
        case ImageFormat::PVRTC2_2BPP_UNORM: return 4;
        case ImageFormat::PVRTC2_4BPP_UNORM: return 4;
        case ImageFormat::PVRTC1_2BPP_SRGB: return 4;
        case ImageFormat::PVRTC1_4BPP_SRGB: return 4;
        case ImageFormat::PVRTC2_2BPP_SRGB: return 4;
        case ImageFormat::PVRTC2_4BPP_SRGB: return 4;
        case ImageFormat::ETC2_R8G8B8_UNORM: return 4;
        case ImageFormat::ETC2_R8G8B8_SRGB: return 4;
        case ImageFormat::ETC2_R8G8B8A1_UNORM: return 4;
        case ImageFormat::ETC2_R8G8B8A1_SRGB: return 4;
        case ImageFormat::ETC2_R8G8B8A8_UNORM: return 4;
        case ImageFormat::ETC2_R8G8B8A8_SRGB: return 4;
        case ImageFormat::ETC2_EAC_R11_UNORM: return 4;
        case ImageFormat::ETC2_EAC_R11_SNORM: return 4;
        case ImageFormat::ETC2_EAC_R11G11_UNORM: return 4;
        case ImageFormat::ETC2_EAC_R11G11_SNORM: return 4;
        case ImageFormat::ASTC_4x4_UNORM: return 4;
        case ImageFormat::ASTC_4x4_SRGB: return 4;
        case ImageFormat::ASTC_5x4_UNORM: return 4;
        case ImageFormat::ASTC_5x4_SRGB: return 4;
        case ImageFormat::ASTC_5x5_UNORM: return 5;
        case ImageFormat::ASTC_5x5_SRGB: return 5;
        case ImageFormat::ASTC_6x5_UNORM: return 5;
        case ImageFormat::ASTC_6x5_SRGB: return 5;
        case ImageFormat::ASTC_6x6_UNORM: return 6;
        case ImageFormat::ASTC_6x6_SRGB: return 6;
        case ImageFormat::ASTC_8x5_UNORM: return 5;
        case ImageFormat::ASTC_8x5_SRGB: return 5;
        case ImageFormat::ASTC_8x6_UNORM: return 6;
        case ImageFormat::ASTC_8x6_SRGB: return 6;
        case ImageFormat::ASTC_8x8_UNORM: return 8;
        case ImageFormat::ASTC_8x8_SRGB: return 8;
        case ImageFormat::ASTC_10x5_UNORM: return 5;
        case ImageFormat::ASTC_10x5_SRGB: return 5;
        case ImageFormat::ASTC_10x6_UNORM: return 6;
        case ImageFormat::ASTC_10x6_SRGB: return 6;
        case ImageFormat::ASTC_10x8_UNORM: return 8;
        case ImageFormat::ASTC_10x8_SRGB: return 8;
        case ImageFormat::ASTC_10x10_UNORM: return 10;
        case ImageFormat::ASTC_10x10_SRGB: return 10;
        case ImageFormat::ASTC_12x10_UNORM: return 10;
        case ImageFormat::ASTC_12x10_SRGB: return 10;
        case ImageFormat::ASTC_12x12_UNORM: return 12;
        case ImageFormat::ASTC_12x12_SRGB: return 12;
        default: return 1;
        }
    }
    /// <summary>
    /// Retrieve wheter the given image format is planar.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <returns>True if planar, false otherwise.</returns>
    [[nodiscard]] inline constexpr bool ImageFormatIsPlanar(const ImageFormat fmt) noexcept
    {
        switch(fmt)
        {
        case ImageFormat::G8_B8R8_2PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G8_B8R8_2PLANE_422_UNORM:
            [[fallthrough]];
        case ImageFormat::G8_B8_R8_3PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G8_B8_R8_3PLANE_422_UNORM:
            [[fallthrough]];
        case ImageFormat::G8_B8_R8_3PLANE_444_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16R16_2PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16R16_2PLANE_422_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16_R16_3PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16_R16_3PLANE_422_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16_R16_3PLANE_444_UNORM:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
            return true;

        default:
            return false;
        }
    }
    /// <summary>
    /// Retrieve the number of planes are strored by the given image format.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <returns>Number of planes.</returns>
    [[nodiscard]] inline constexpr uint32_t ImageFormatNumOfPlanes(const ImageFormat fmt) noexcept
    {
        switch(fmt)
        {
        case ImageFormat::G8_B8_R8_3PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G8_B8_R8_3PLANE_422_UNORM:
            [[fallthrough]];
        case ImageFormat::G8_B8_R8_3PLANE_444_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16_R16_3PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16_R16_3PLANE_422_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16_R16_3PLANE_444_UNORM:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
            return 3;

        case ImageFormat::G8_B8R8_2PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G8_B8R8_2PLANE_422_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16R16_2PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16R16_2PLANE_422_UNORM:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
            return 2;

        default:
            return 1;
        }
    }
    /// <summary>
    /// Retrieve whether the given image format has only a single plane.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <returns>True if single plane, false otherwise.</returns>
    [[nodiscard]] inline constexpr bool ImageFormatIsSinglePlane(const ImageFormat fmt) noexcept
    {
        return !ImageFormatIsPlanar(fmt) || ImageFormatNumOfPlanes(fmt) < 2;
    }
    /// <summary>
    /// Retrieve the width of a specific plane of the given image format.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <param name="plane">Plane index.</param>
    /// <param name="width">Width of the image (plane 0).</param>
    /// <returns>Width of the plane.</returns>
    [[nodiscard]] inline constexpr uint32_t ImageFormatPlaneWidth(const ImageFormat fmt, const uint32_t plane,
                                                                  const uint32_t width) noexcept
    {
        if (plane == 0)
            return width;

        switch(fmt)
        {
        case ImageFormat::G8_B8_R8_3PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G8_B8R8_2PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G8_B8_R8_3PLANE_422_UNORM:
            [[fallthrough]];
        case ImageFormat::G8_B8R8_2PLANE_422_UNORM:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G16_B16_R16_3PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16_R16_3PLANE_422_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16R16_2PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16R16_2PLANE_422_UNORM:
            return width >> 1;

        default:
            return width;
        }
    }
    /// <summary>
    /// Retrieve the height of a specific plane of the given image format.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <param name="plane">Plane index.</param>
    /// <param name="height">Height of the image (plane 0).</param>
    /// <returns>Height of the plane.</returns>
    [[nodiscard]] inline constexpr uint32_t ImageFormatPlaneHeight(const ImageFormat fmt, const uint32_t plane,
                                                                   const uint32_t height) noexcept
    {
        if (plane == 0)
            return height;

        switch(fmt)
        {
        case ImageFormat::G8_B8_R8_3PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G8_B8R8_2PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G16_B16_R16_3PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16R16_2PLANE_420_UNORM:
            return height >> 1;

        default:
            return height;
        }
    }
    /// <summary>
    /// Retrieve the plane size of a single block for the given plane index from the given image format.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <param name="plane">Plane index.</param>
    /// <returns>Plane size of a single block.</returns>
    [[nodiscard]] inline constexpr uint32_t ImageFormatPlaneSizeOfBlock(const ImageFormat fmt, const uint32_t plane) noexcept
    {
        switch(fmt)
        {
        case ImageFormat::G8_B8_R8_3PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G8_B8_R8_3PLANE_422_UNORM:
            [[fallthrough]];
        case ImageFormat::G8_B8_R8_3PLANE_444_UNORM:
            return 1;

        case ImageFormat::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G16_B16_R16_3PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16_R16_3PLANE_422_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16_R16_3PLANE_444_UNORM:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
            return 2;

        case ImageFormat::G8_B8R8_2PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G8_B8R8_2PLANE_422_UNORM:
            return 0 == plane ? 1 : 2;

        case ImageFormat::G16_B16R16_2PLANE_420_UNORM:
            [[fallthrough]];
        case ImageFormat::G16_B16R16_2PLANE_422_UNORM:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
            [[fallthrough]];
        case ImageFormat::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
            return 0 == plane ? 2 : 4;

        default:
            return 1;
        }
    }
    /// <summary>
    /// Retrieve whether the image format is a compresed format.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <returns>True if the image format is a compressed format.</returns>
    [[nodiscard]] inline constexpr bool ImageFormatIsCompressed(const ImageFormat fmt) noexcept
    {
        switch(fmt)
        {
        case ImageFormat::DXBC1_RGB_UNORM:
            [[fallthrough]];
        case ImageFormat::DXBC1_RGB_SRGB:
            [[fallthrough]];
        case ImageFormat::DXBC1_RGBA_UNORM:
            [[fallthrough]];
        case ImageFormat::DXBC1_RGBA_SRGB:
            [[fallthrough]];
        case ImageFormat::DXBC2_UNORM:
            [[fallthrough]];
        case ImageFormat::DXBC2_SRGB:
            [[fallthrough]];
        case ImageFormat::DXBC3_UNORM:
            [[fallthrough]];
        case ImageFormat::DXBC3_SRGB:
            [[fallthrough]];
        case ImageFormat::DXBC4_UNORM:
            [[fallthrough]];
        case ImageFormat::DXBC4_SNORM:
            [[fallthrough]];
        case ImageFormat::DXBC5_UNORM:
            [[fallthrough]];
        case ImageFormat::DXBC5_SNORM:
            [[fallthrough]];
        case ImageFormat::DXBC6H_UFLOAT:
            [[fallthrough]];
        case ImageFormat::DXBC6H_SFLOAT:
            [[fallthrough]];
        case ImageFormat::DXBC7_UNORM:
            [[fallthrough]];
        case ImageFormat::DXBC7_SRGB:
            [[fallthrough]];
        case ImageFormat::PVRTC1_2BPP_UNORM:
            [[fallthrough]];
        case ImageFormat::PVRTC1_4BPP_UNORM:
            [[fallthrough]];
        case ImageFormat::PVRTC2_2BPP_UNORM:
            [[fallthrough]];
        case ImageFormat::PVRTC2_4BPP_UNORM:
            [[fallthrough]];
        case ImageFormat::PVRTC1_2BPP_SRGB:
            [[fallthrough]];
        case ImageFormat::PVRTC1_4BPP_SRGB:
            [[fallthrough]];
        case ImageFormat::PVRTC2_2BPP_SRGB:
            [[fallthrough]];
        case ImageFormat::PVRTC2_4BPP_SRGB:
            [[fallthrough]];
        case ImageFormat::ETC2_R8G8B8_UNORM:
            [[fallthrough]];
        case ImageFormat::ETC2_R8G8B8_SRGB:
            [[fallthrough]];
        case ImageFormat::ETC2_R8G8B8A1_UNORM:
            [[fallthrough]];
        case ImageFormat::ETC2_R8G8B8A1_SRGB:
            [[fallthrough]];
        case ImageFormat::ETC2_R8G8B8A8_UNORM:
            [[fallthrough]];
        case ImageFormat::ETC2_R8G8B8A8_SRGB:
            [[fallthrough]];
        case ImageFormat::ETC2_EAC_R11_UNORM:
            [[fallthrough]];
        case ImageFormat::ETC2_EAC_R11_SNORM:
            [[fallthrough]];
        case ImageFormat::ETC2_EAC_R11G11_UNORM:
            [[fallthrough]];
        case ImageFormat::ETC2_EAC_R11G11_SNORM:
            [[fallthrough]];
        case ImageFormat::ASTC_4x4_UNORM:
            [[fallthrough]];
        case ImageFormat::ASTC_4x4_SRGB:
            [[fallthrough]];
        case ImageFormat::ASTC_5x4_UNORM:
            [[fallthrough]];
        case ImageFormat::ASTC_5x4_SRGB:
            [[fallthrough]];
        case ImageFormat::ASTC_5x5_UNORM:
            [[fallthrough]];
        case ImageFormat::ASTC_5x5_SRGB:
            [[fallthrough]];
        case ImageFormat::ASTC_6x5_UNORM:
            [[fallthrough]];
        case ImageFormat::ASTC_6x5_SRGB:
            [[fallthrough]];
        case ImageFormat::ASTC_6x6_UNORM:
            [[fallthrough]];
        case ImageFormat::ASTC_6x6_SRGB:
            [[fallthrough]];
        case ImageFormat::ASTC_8x5_UNORM:
            [[fallthrough]];
        case ImageFormat::ASTC_8x5_SRGB:
            [[fallthrough]];
        case ImageFormat::ASTC_8x6_UNORM:
            [[fallthrough]];
        case ImageFormat::ASTC_8x6_SRGB:
            [[fallthrough]];
        case ImageFormat::ASTC_8x8_UNORM:
            [[fallthrough]];
        case ImageFormat::ASTC_8x8_SRGB:
            [[fallthrough]];
        case ImageFormat::ASTC_10x5_UNORM:
            [[fallthrough]];
        case ImageFormat::ASTC_10x5_SRGB:
            [[fallthrough]];
        case ImageFormat::ASTC_10x6_UNORM:
            [[fallthrough]];
        case ImageFormat::ASTC_10x6_SRGB:
            [[fallthrough]];
        case ImageFormat::ASTC_10x8_UNORM:
            [[fallthrough]];
        case ImageFormat::ASTC_10x8_SRGB:
            [[fallthrough]];
        case ImageFormat::ASTC_10x10_UNORM:
            [[fallthrough]];
        case ImageFormat::ASTC_10x10_SRGB:
            [[fallthrough]];
        case ImageFormat::ASTC_12x10_UNORM:
            [[fallthrough]];
        case ImageFormat::ASTC_12x10_SRGB:
            [[fallthrough]];
        case ImageFormat::ASTC_12x12_UNORM:
            [[fallthrough]];
        case ImageFormat::ASTC_12x12_SRGB:
            return true;

        default:
            return false;
        }
    }

    /// <summary>
    /// Retrieve the number of channels in the image format.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <returns>Number of channels.</returns>
    [[nodiscard]] inline constexpr uint32_t ImageFormatChannelCount(const ImageFormat fmt) noexcept
    {
        switch(fmt)
        {
        case ImageFormat::Undefined: return 0;
        case ImageFormat::R1_UNORM: return 1;
        case ImageFormat::R2_UNORM: return 1;
        case ImageFormat::R4_UNORM: return 1;
        case ImageFormat::R4G4_UNORM: return 2;
        case ImageFormat::G4R4_UNORM: return 2;
        case ImageFormat::A8_UNORM: return 1;
        case ImageFormat::R8_UNORM: return 1;
        case ImageFormat::R8_SNORM: return 1;
        case ImageFormat::R8_UINT: return 1;
        case ImageFormat::R8_SINT: return 1;
        case ImageFormat::R8_SRGB: return 1;
        case ImageFormat::B2G3R3_UNORM: return 3;
        case ImageFormat::R5G6B5_UNORM: return 3;
        case ImageFormat::B5G6R5_UNORM: return 3;
        case ImageFormat::R8G8_UNORM: return 2;
        case ImageFormat::R8G8_SNORM: return 2;
        case ImageFormat::G8R8_UNORM: return 2;
        case ImageFormat::G8R8_SNORM: return 2;
        case ImageFormat::R8G8_UINT: return 2;
        case ImageFormat::R8G8_SINT: return 2;
        case ImageFormat::R8G8_SRGB: return 2;
        case ImageFormat::R16_UNORM: return 1;
        case ImageFormat::R16_SNORM: return 1;
        case ImageFormat::R16_UINT: return 1;
        case ImageFormat::R16_SINT: return 1;
        case ImageFormat::R16_SFLOAT: return 1;
        case ImageFormat::R16_SBFLOAT: return 1;
        case ImageFormat::R8G8B8_UNORM: return 3;
        case ImageFormat::R8G8B8_SNORM: return 3;
        case ImageFormat::R8G8B8_UINT: return 3;
        case ImageFormat::R8G8B8_SINT: return 3;
        case ImageFormat::R8G8B8_SRGB: return 3;
        case ImageFormat::B8G8R8_UNORM: return 3;
        case ImageFormat::B8G8R8_SNORM: return 3;
        case ImageFormat::B8G8R8_UINT: return 3;
        case ImageFormat::B8G8R8_SINT: return 3;
        case ImageFormat::B8G8R8_SRGB: return 3;
        case ImageFormat::R16G16_UNORM: return 2;
        case ImageFormat::G16R16_UNORM: return 2;
        case ImageFormat::R16G16_SNORM: return 2;
        case ImageFormat::G16R16_SNORM: return 2;
        case ImageFormat::R16G16_UINT: return 2;
        case ImageFormat::R16G16_SINT: return 2;
        case ImageFormat::R16G16_SFLOAT: return 2;
        case ImageFormat::R16G16_SBFLOAT: return 2;
        case ImageFormat::R32_UINT: return 1;
        case ImageFormat::R32_SINT: return 1;
        case ImageFormat::R32_SFLOAT: return 1;
        case ImageFormat::B10G11R11_UFLOAT: return 3;
        case ImageFormat::R16G16B16_UNORM: return 3;
        case ImageFormat::R16G16B16_SNORM: return 3;
        case ImageFormat::R16G16B16_UINT: return 3;
        case ImageFormat::R16G16B16_SINT: return 3;
        case ImageFormat::R16G16B16_SFLOAT: return 3;
        case ImageFormat::R16G16B16_SBFLOAT: return 3;
        case ImageFormat::R32G32_UINT: return 2;
        case ImageFormat::R32G32_SINT: return 2;
        case ImageFormat::R32G32_SFLOAT: return 2;
        case ImageFormat::R32G32B32_UINT: return 3;
        case ImageFormat::R32G32B32_SINT: return 3;
        case ImageFormat::R32G32B32_SFLOAT: return 3;
        case ImageFormat::R64_UINT: return 1;
        case ImageFormat::R64_SINT: return 1;
        case ImageFormat::R64_SFLOAT: return 1;
        case ImageFormat::R64G64_UINT: return 2;
        case ImageFormat::R64G64_SINT: return 2;
        case ImageFormat::R64G64_SFLOAT: return 2;
        case ImageFormat::R64G64B64_UINT: return 3;
        case ImageFormat::R64G64B64_SINT: return 3;
        case ImageFormat::R64G64B64_SFLOAT: return 3;
        case ImageFormat::D16_UNORM: return 1;
        case ImageFormat::X8_D24_UNORM: return 2;
        case ImageFormat::D32_SFLOAT: return 1;
        case ImageFormat::S8_UINT: return 1;
        case ImageFormat::D16_UNORM_S8_UINT: return 2;
        case ImageFormat::D24_UNORM_S8_UINT: return 2;
        case ImageFormat::D32_SFLOAT_S8_UINT: return 2;
        case ImageFormat::DXBC1_RGB_UNORM: return 3;
        case ImageFormat::DXBC1_RGB_SRGB: return 3;
        case ImageFormat::DXBC4_UNORM: return 1;
        case ImageFormat::DXBC4_SNORM: return 1;
        case ImageFormat::DXBC5_UNORM: return 2;
        case ImageFormat::DXBC5_SNORM: return 2;
        case ImageFormat::DXBC6H_UFLOAT: return 3;
        case ImageFormat::DXBC6H_SFLOAT: return 3;
        case ImageFormat::ETC2_R8G8B8_UNORM: return 3;
        case ImageFormat::ETC2_R8G8B8_SRGB: return 3;
        case ImageFormat::ETC2_EAC_R11_UNORM: return 1;
        case ImageFormat::ETC2_EAC_R11_SNORM: return 1;
        case ImageFormat::ETC2_EAC_R11G11_UNORM: return 2;
        case ImageFormat::ETC2_EAC_R11G11_SNORM: return 2;
        case ImageFormat::CLUT_P4: return 1;
        case ImageFormat::CLUT_P4A4: return 2;
        case ImageFormat::CLUT_P8: return 1;
        case ImageFormat::CLUT_P8A8: return 2;
        default: return 4;
        }
    }
    /// <summary>
    /// Retrieve whether the given image format stores data as floating point.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <returns>True if the format stores floating point data.</returns>
    [[nodiscard]] inline constexpr bool ImageFormatIsFloat(const ImageFormat fmt) noexcept
    {
        switch(fmt)
        {
        case ImageFormat::R16_SFLOAT: return true;
        case ImageFormat::R16_SBFLOAT: return true;
        case ImageFormat::R16G16_SFLOAT: return true;
        case ImageFormat::R16G16_SBFLOAT: return true;
        case ImageFormat::R32_SFLOAT: return true;
        case ImageFormat::B10G11R11_UFLOAT: return true;
        case ImageFormat::E5B9G9R9_UFLOAT: return true;
        case ImageFormat::R16G16B16_SFLOAT: return true;
        case ImageFormat::R16G16B16_SBFLOAT: return true;
        case ImageFormat::R16G16B16A16_SFLOAT: return true;
        case ImageFormat::R16G16B16A16_SBFLOAT: return true;
        case ImageFormat::R32G32_SFLOAT: return true;
        case ImageFormat::R32G32B32_SFLOAT: return true;
        case ImageFormat::R32G32B32A32_SFLOAT: return true;
        case ImageFormat::R64_SFLOAT: return true;
        case ImageFormat::R64G64_SFLOAT: return true;
        case ImageFormat::R64G64B64_SFLOAT: return true;
        case ImageFormat::R64G64B64A64_SFLOAT: return true;
        case ImageFormat::D32_SFLOAT: return true;
        case ImageFormat::D32_SFLOAT_S8_UINT: return true;
        case ImageFormat::DXBC6H_UFLOAT: return true;
        case ImageFormat::DXBC6H_SFLOAT: return true;
        default: return false;
        }
    }
    /// <summary>
    /// Retrieve whether the given image format stores data as uint16_t.
    /// </summary>
    /// <param name="fmt">Image format.</param>
    /// <returns>True if the format stores uint16_t data.</returns>
    [[nodiscard]] inline constexpr bool ImageFormatIsU16(const ImageFormat fmt) noexcept
    {
        switch(fmt)
        {
        case ImageFormat::R16_UNORM: return true;
        case ImageFormat::R16_SNORM: return true;
        case ImageFormat::R16_UINT: return true;
        case ImageFormat::R16_SINT: return true;
        case ImageFormat::R16G16_UNORM: return true;
        case ImageFormat::G16R16_UNORM: return true;
        case ImageFormat::R16G16_SNORM: return true;
        case ImageFormat::G16R16_SNORM: return true;
        case ImageFormat::R16G16_UINT: return true;
        case ImageFormat::R16G16_SINT: return true;
        case ImageFormat::R16G16B16_UNORM: return true;
        case ImageFormat::R16G16B16_SNORM: return true;
        case ImageFormat::R16G16B16_UINT: return true;
        case ImageFormat::R16G16B16_SINT: return true;
        case ImageFormat::R16G16B16A16_UNORM: return true;
        case ImageFormat::R16G16B16A16_SNORM: return true;
        case ImageFormat::R16G16B16A16_UINT: return true;
        case ImageFormat::R16G16B16A16_SINT: return true;
        case ImageFormat::D16_UNORM: return true;
        case ImageFormat::D16_UNORM_S8_UINT: return true;
        default: return false;
        }
    }
}

#endif /*TRAP_IMAGEFORMAT_H*/