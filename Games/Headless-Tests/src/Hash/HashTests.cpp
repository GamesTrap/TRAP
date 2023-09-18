#include "HashTests.h"
#include "Utils/Hash/Adler32.h"
#include "Utils/Hash/CRC32.h"
#include "Utils/Hash/SHA-2.h"

void HashTests::OnAttach()
{
    TestAdler32();
    TestCRC32();
    TestSHA2_256();
    TestSHA2_512();
    TestSHA3_256();
    TestSHA3_512();

	TRAP::Application::Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void HashTests::TestAdler32()
{
    static constexpr std::array<uint8_t, 4> emptyHash = {0x00, 0x00, 0x00, 0x01};
    assert(TRAP::Utils::Hash::Adler32("") == emptyHash);

    static constexpr std::array<uint8_t, 4> test1Hash = {0x36, 0xC4, 0x05, 0xFE};
    assert(TRAP::Utils::Hash::Adler32("Eagles are great!") == test1Hash);

    static constexpr std::array<uint8_t, 4> test2Hash = {0x04, 0x0F, 0x0F, 0xC1};
    assert(TRAP::Utils::Hash::Adler32("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~") == test2Hash);
}

//-------------------------------------------------------------------------------------------------------------------//

void HashTests::TestCRC32()
{
    static constexpr std::array<uint8_t, 4> emptyHash = {0x00, 0x00, 0x00, 0x00};
    assert(TRAP::Utils::Hash::CRC32("") == emptyHash);

    static constexpr std::array<uint8_t, 4> foxHash = {0x41, 0x4F, 0xA3, 0x39};
    assert(TRAP::Utils::Hash::CRC32("The quick brown fox jumps over the lazy dog") == foxHash);

    static constexpr std::array<uint8_t, 4> testData1Hash{0x9B, 0xD3, 0x66, 0xAE};
    static constexpr std::array<uint8_t, 33> testData1 =
    {
        0x76, 0x61, 0x72, 0x69, 0x6F, 0x75, 0x73, 0x20, 0x43, 0x52, 0x43, 0x20, 0x61, 0x6C, 0x67, 0x6F, 0x72, 0x69,
        0x74, 0x68, 0x6D, 0x73, 0x20, 0x69, 0x6E, 0x70, 0x75, 0x74, 0x20, 0x64, 0x61, 0x74, 0x61
    };
    assert(TRAP::Utils::Hash::CRC32(testData1.data(), testData1.size()) == testData1Hash);
}

//-------------------------------------------------------------------------------------------------------------------//

void HashTests::TestSHA2_256()
{
    static constexpr std::array<uint8_t, 32> abcHash{0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA, 0x41, 0x41, 0x40,
                                              0xDE, 0x5D, 0xAE, 0x22, 0x23, 0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17,
                                              0x7A, 0x9C, 0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD};
    assert(TRAP::Utils::Hash::SHA2_256("abc") == abcHash);
    static constexpr std::array<uint8_t, 32> emptyHash{0xE3, 0xB0, 0xC4, 0x42, 0x98, 0xFC, 0x1C, 0x14, 0x9A, 0xFB, 0xF4,
                                               0xC8, 0x99, 0x6F, 0xB9, 0x24, 0x27, 0xAE, 0x41, 0xE4, 0x64, 0x9B,
                                               0x93, 0x4C, 0xA4, 0x95, 0x99, 0x1B, 0x78, 0x52, 0xB8, 0x55};
    assert(TRAP::Utils::Hash::SHA2_256("") == emptyHash);
    static constexpr std::array<uint8_t, 32> longHash{0x24, 0x8D, 0x6A, 0x61, 0xD2, 0x06, 0x38, 0xB8, 0xE5, 0xC0, 0x26,
                                               0x93, 0x0C, 0x3E, 0x60, 0x39, 0xA3, 0x3C, 0xE4, 0x59, 0x64, 0xFF,
                                               0x21, 0x67, 0xF6, 0xEC, 0xED, 0xD4, 0x19, 0xDB, 0x06, 0xC1};
    assert(TRAP::Utils::Hash::SHA2_256("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") == longHash);
    static constexpr std::array<uint8_t, 32> longerHash{0xCF, 0x5B, 0x16, 0xA7, 0x78, 0xAF, 0x83, 0x80, 0x03, 0x6C, 0xE5,
                                                 0x9E, 0x7B, 0x04, 0x92, 0x37, 0x0B, 0x24, 0x9B, 0x11, 0xE8, 0xF0,
                                                 0x7A, 0x51, 0xAF, 0xAC, 0x45, 0x03, 0x7A, 0xFE, 0xE9, 0xD1};
    assert(TRAP::Utils::Hash::SHA2_256("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu") == longerHash);

    static constexpr std::array<uint8_t, 32> aHash{0xCD, 0xC7, 0x6E, 0x5C, 0x99, 0x14, 0xFB, 0x92, 0x81, 0xA1, 0xC7,
                                            0xE2, 0x84, 0xD7, 0x3E, 0x67, 0xF1, 0x80, 0x9A, 0x48, 0xA4, 0x97,
                                            0x20, 0x0E, 0x04, 0x6D, 0x39, 0xCC, 0xC7, 0x11, 0x2C, 0xD0};
    const std::string aTest(1'000'000, 'a');
    assert(TRAP::Utils::Hash::SHA2_256(aTest) == aHash);

    static constexpr std::array<uint8_t, 32> extremeHash{0x50, 0xE7, 0x2A, 0x0E, 0x26, 0x44, 0x2F, 0xE2, 0x55, 0x2D, 0xC3,
                                                  0x93, 0x8A, 0xC5, 0x86, 0x58, 0x22, 0x8C, 0x0C, 0xBF, 0xB1, 0xD2,
                                                  0xCA, 0x87, 0x2A, 0xE4, 0x35, 0x26, 0x6F, 0xCD, 0x05, 0x5E};
    std::string extremeTest;
    extremeTest.reserve(16'777'216ull * 64);
    for(uint64_t i = 0; i < 16'777'216; ++i)
        extremeTest += "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno";
    assert(TRAP::Utils::Hash::SHA2_256(extremeTest) == extremeHash);
}

//-------------------------------------------------------------------------------------------------------------------//

void HashTests::TestSHA2_512()
{
    static constexpr std::array<uint8_t, 64> abcHash{0xDD, 0xAF, 0x35, 0xA1, 0x93, 0x61, 0x7A, 0xBA, 0xCC, 0x41, 0x73,
                                              0x49, 0xAE, 0x20, 0x41, 0x31, 0x12, 0xE6, 0xFA, 0x4E, 0x89, 0xA9,
                                              0x7E, 0xA2, 0x0A, 0x9E, 0xEE, 0xE6, 0x4B, 0x55, 0xD3, 0x9A, 0x21,
                                              0x92, 0x99, 0x2A, 0x27, 0x4F, 0xC1, 0xA8, 0x36, 0xBA, 0x3C, 0x23,
                                              0xA3, 0xFE, 0xEB, 0xBD, 0x45, 0x4D, 0x44, 0x23, 0x64, 0x3C, 0xE8,
                                              0x0E, 0x2A, 0x9A, 0xC9, 0x4F, 0xA5, 0x4C, 0xA4, 0x9F};
    assert(TRAP::Utils::Hash::SHA2_512("abc") == abcHash);
    static constexpr std::array<uint8_t, 64> emptyHash{0xCF, 0x83, 0xE1, 0x35, 0x7E, 0xEF, 0xB8, 0xBD, 0xF1, 0x54, 0x28,
                                                0x50, 0xD6, 0x6D, 0x80, 0x07, 0xD6, 0x20, 0xE4, 0x05, 0x0B, 0x57,
                                                0x15, 0xDC, 0x83, 0xF4, 0xA9, 0x21, 0xD3, 0x6C, 0xE9, 0xCE, 0x47,
                                                0xD0, 0xD1, 0x3C, 0x5D, 0x85, 0xF2, 0xB0, 0xFF, 0x83, 0x18, 0xD2,
                                                0x87, 0x7E, 0xEC, 0x2F, 0x63, 0xB9, 0x31, 0xBD, 0x47, 0x41, 0x7A,
                                                0x81, 0xA5, 0x38, 0x32, 0x7A, 0xF9, 0x27, 0xDA, 0x3E};
    assert(TRAP::Utils::Hash::SHA2_512("") == emptyHash);
    static constexpr std::array<uint8_t, 64> longHash{0x20, 0x4A, 0x8F, 0xC6, 0xDD, 0xA8, 0x2F, 0x0A, 0x0C, 0xED, 0x7B,
                                               0xEB, 0x8E, 0x08, 0xA4, 0x16, 0x57, 0xC1, 0x6E, 0xF4, 0x68, 0xB2,
                                               0x28, 0xA8, 0x27, 0x9B, 0xE3, 0x31, 0xA7, 0x03, 0xC3, 0x35, 0x96,
                                               0xFD, 0x15, 0xC1, 0x3B, 0x1B, 0x07, 0xF9, 0xAA, 0x1D, 0x3B, 0xEA,
                                               0x57, 0x78, 0x9C, 0xA0, 0x31, 0xAD, 0x85, 0xC7, 0xA7, 0x1D, 0xD7,
                                               0x03, 0x54, 0xEC, 0x63, 0x12, 0x38, 0xCA, 0x34, 0x45};
    assert(TRAP::Utils::Hash::SHA2_512("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") == longHash);
    static constexpr std::array<uint8_t, 64> longerHash{0x8E, 0x95, 0x9B, 0x75, 0xDA, 0xE3, 0x13, 0xDA, 0x8C, 0xF4, 0xF7,
                                                 0x28, 0x14, 0xFC, 0x14, 0x3F, 0x8F, 0x77, 0x79, 0xC6, 0xEB, 0x9F,
                                                 0x7F, 0xA1, 0x72, 0x99, 0xAE, 0xAD, 0xB6, 0x88, 0x90, 0x18, 0x50,
                                                 0x1D, 0x28, 0x9E, 0x49, 0x00, 0xF7, 0xE4, 0x33, 0x1B, 0x99, 0xDE,
                                                 0xC4, 0xB5, 0x43, 0x3A, 0xC7, 0xD3, 0x29, 0xEE, 0xB6, 0xDD, 0x26,
                                                 0x54, 0x5E, 0x96, 0xE5, 0x5B, 0x87, 0x4B, 0xE9, 0x09};
    assert(TRAP::Utils::Hash::SHA2_512("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu") == longerHash);

    static constexpr std::array<uint8_t, 64> aHash{0xE7, 0x18, 0x48, 0x3D, 0x0C, 0xE7, 0x69, 0x64, 0x4E, 0x2E, 0x42,
                                            0xC7, 0xBC, 0x15, 0xB4, 0x63, 0x8E, 0x1F, 0x98, 0xB1, 0x3B, 0x20,
                                            0x44, 0x28, 0x56, 0x32, 0xA8, 0x03, 0xAF, 0xA9, 0x73, 0xEB, 0xDE,
                                            0x0F, 0xF2, 0x44, 0x87, 0x7E, 0xA6, 0x0A, 0x4C, 0xB0, 0x43, 0x2C,
                                            0xE5, 0x77, 0xC3, 0x1B, 0xEB, 0x00, 0x9C, 0x5C, 0x2C, 0x49, 0xAA,
                                            0x2E, 0x4E, 0xAD, 0xB2, 0x17, 0xAD, 0x8C, 0xC0, 0x9B};
    const std::string aTest(1'000'000, 'a');
    assert(TRAP::Utils::Hash::SHA2_512(aTest) == aHash);

    static constexpr std::array<uint8_t, 64> extremeHash{0xB4, 0x7C, 0x93, 0x34, 0x21, 0xEA, 0x2D, 0xB1, 0x49, 0xAD, 0x6E,
                                                  0x10, 0xFC, 0xE6, 0xC7, 0xF9, 0x3D, 0x07, 0x52, 0x38, 0x01, 0x80,
                                                  0xFF, 0xD7, 0xF4, 0x62, 0x9A, 0x71, 0x21, 0x34, 0x83, 0x1D, 0x77,
                                                  0xBE, 0x60, 0x91, 0xB8, 0x19, 0xED, 0x35, 0x2C, 0x29, 0x67, 0xA2,
                                                  0xE2, 0xD4, 0xFA, 0x50, 0x50, 0x72, 0x3C, 0x96, 0x30, 0x69, 0x1F,
                                                  0x1A, 0x05, 0xA7, 0x28, 0x1D, 0xBE, 0x6C, 0x10, 0x86};
    std::string extremeTest;
    extremeTest.reserve(16'777'216ull * 64);
    for(uint64_t i = 0; i < 16'777'216; ++i)
        extremeTest += "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno";
    assert(TRAP::Utils::Hash::SHA2_512(extremeTest) == extremeHash);
}

//-------------------------------------------------------------------------------------------------------------------//

void HashTests::TestSHA3_256()
{
    static constexpr std::array<uint8_t, 32> abcHash{0x3A, 0x98, 0x5D, 0xA7, 0x4F, 0xE2, 0x25, 0xB2, 0x04, 0x5C, 0x17,
                                              0x2D, 0x6B, 0xD3, 0x90, 0xBD, 0x85, 0x5F, 0x08, 0x6E, 0x3E, 0x9D,
                                              0x52, 0x5B, 0x46, 0xBF, 0xE2, 0x45, 0x11, 0x43, 0x15, 0x32};
    assert(TRAP::Utils::Hash::SHA3_256("abc") == abcHash);
    static constexpr std::array<uint8_t, 32> emptyHash{0xA7, 0xFF, 0xC6, 0xF8, 0xBF, 0x1E, 0xD7, 0x66, 0x51, 0xC1, 0x47,
                                                0x56, 0xA0, 0x61, 0xD6, 0x62, 0xF5, 0x80, 0xFF, 0x4D, 0xE4, 0x3B,
                                                0x49, 0xFA, 0x82, 0xD8, 0x0A, 0x4B, 0x80, 0xF8, 0x43, 0x4A};
    assert(TRAP::Utils::Hash::SHA3_256("") == emptyHash);
    static constexpr std::array<uint8_t, 32> longHash{0x41, 0xC0, 0xDB, 0xA2, 0xA9, 0xD6, 0x24, 0x08, 0x49, 0x10, 0x03,
                                               0x76, 0xA8, 0x23, 0x5E, 0x2C, 0x82, 0xE1, 0xB9, 0x99, 0x8A, 0x99,
                                               0x9E, 0x21, 0xDB, 0x32, 0xDD, 0x97, 0x49, 0x6D, 0x33, 0x76};
    assert(TRAP::Utils::Hash::SHA3_256("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") == longHash);
    static constexpr std::array<uint8_t, 32> longerHash{0x91, 0x6F, 0x60, 0x61, 0xFE, 0x87, 0x97, 0x41, 0xCA, 0x64, 0x69,
                                                 0xB4, 0x39, 0x71, 0xDF, 0xDB, 0x28, 0xB1, 0xA3, 0x2D, 0xC3, 0x6C,
                                                 0xB3, 0x25, 0x4E, 0x81, 0x2B, 0xE2, 0x7A, 0xAD, 0x1D, 0x18};
    assert(TRAP::Utils::Hash::SHA3_256("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu") == longerHash);

    static constexpr std::array<uint8_t, 32> aHash{0x5C, 0x88, 0x75, 0xAE, 0x47, 0x4A, 0x36, 0x34, 0xBA, 0x4F, 0xD5,
                                            0x5E, 0xC8, 0x5B, 0xFF, 0xD6, 0x61, 0xF3, 0x2A, 0xCA, 0x75, 0xC6,
                                            0xD6, 0x99, 0xD0, 0xCD, 0xCB, 0x6C, 0x11, 0x58, 0x91, 0xC1};
    const std::string aTest(1'000'000, 'a');
    assert(TRAP::Utils::Hash::SHA3_256(aTest) == aHash);

    static constexpr std::array<uint8_t, 32> extremeHash{0xEC, 0xBB, 0xC4, 0x2C, 0xBF, 0x29, 0x66, 0x03, 0xAC, 0xB2, 0xC6,
                                                  0xBC, 0x04, 0x10, 0xEF, 0x43, 0x78, 0xBA, 0xFB, 0x24, 0xB7, 0x10,
                                                  0x35, 0x7F, 0x12, 0xDF, 0x60, 0x77, 0x58, 0xB3, 0x3E, 0x2B};
    std::string extremeTest;
    extremeTest.reserve(16'777'216ull * 64);
    for(uint64_t i = 0; i < 16'777'216; ++i)
        extremeTest += "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno";
    assert(TRAP::Utils::Hash::SHA3_256(extremeTest) == extremeHash);
}

//-------------------------------------------------------------------------------------------------------------------//

void HashTests::TestSHA3_512()
{
    static constexpr std::array<uint8_t, 64> abcHash{0xB7, 0x51, 0x85, 0x0B, 0x1A, 0x57, 0x16, 0x8A, 0x56, 0x93, 0xCD,
                                              0x92, 0x4B, 0x6B, 0x09, 0x6E, 0x08, 0xF6, 0x21, 0x82, 0x74, 0x44,
                                              0xF7, 0x0D, 0x88, 0x4F, 0x5D, 0x02, 0x40, 0xD2, 0x71, 0x2E, 0x10,
                                              0xE1, 0x16, 0xE9, 0x19, 0x2A, 0xF3, 0xC9, 0x1A, 0x7E, 0xC5, 0x76,
                                              0x47, 0xE3, 0x93, 0x40, 0x57, 0x34, 0x0B, 0x4C, 0xF4, 0x08, 0xD5,
                                              0xA5, 0x65, 0x92, 0xF8, 0x27, 0x4E, 0xEC, 0x53, 0xF0};
    assert(TRAP::Utils::Hash::SHA3_512("abc") == abcHash);
    static constexpr std::array<uint8_t, 64> emptyHash{0xA6, 0x9F, 0x73, 0xCC, 0xA2, 0x3A, 0x9A, 0xC5, 0xC8, 0xB5, 0x67,
                                                0xDC, 0x18, 0x5A, 0x75, 0x6E, 0x97, 0xC9, 0x82, 0x16, 0x4F, 0xE2,
                                                0x58, 0x59, 0xE0, 0xD1, 0xDC, 0xC1, 0x47, 0x5C, 0x80, 0xA6, 0x15,
                                                0xB2, 0x12, 0x3A, 0xF1, 0xF5, 0xF9, 0x4C, 0x11, 0xE3, 0xE9, 0x40,
                                                0x2C, 0x3A, 0xC5, 0x58, 0xF5, 0x00, 0x19, 0x9D, 0x95, 0xB6, 0xD3,
                                                0xE3, 0x01, 0x75, 0x85, 0x86, 0x28, 0x1D, 0xCD, 0x26};
    assert(TRAP::Utils::Hash::SHA3_512("") == emptyHash);
    static constexpr std::array<uint8_t, 64> longHash{0x04, 0xA3, 0x71, 0xE8, 0x4E, 0xCF, 0xB5, 0xB8, 0xB7, 0x7C, 0xB4,
                                               0x86, 0x10, 0xFC, 0xA8, 0x18, 0x2D, 0xD4, 0x57, 0xCE, 0x6F, 0x32,
                                               0x6A, 0x0F, 0xD3, 0xD7, 0xEC, 0x2F, 0x1E, 0x91, 0x63, 0x6D, 0xEE,
                                               0x69, 0x1F, 0xBE, 0x0C, 0x98, 0x53, 0x02, 0xBA, 0x1B, 0x0D, 0x8D,
                                               0xC7, 0x8C, 0x08, 0x63, 0x46, 0xB5, 0x33, 0xB4, 0x9C, 0x03, 0x0D,
                                               0x99, 0xA2, 0x7D, 0xAF, 0x11, 0x39, 0xD6, 0xE7, 0x5E};
    assert(TRAP::Utils::Hash::SHA3_512("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") == longHash);
    static constexpr std::array<uint8_t, 64> longerHash{0xAF, 0xEB, 0xB2, 0xEF, 0x54, 0x2E, 0x65, 0x79, 0xC5, 0x0C, 0xAD,
                                                 0x06, 0xD2, 0xE5, 0x78, 0xF9, 0xF8, 0xDD, 0x68, 0x81, 0xD7, 0xDC,
                                                 0x82, 0x4D, 0x26, 0x36, 0x0F, 0xEE, 0xBF, 0x18, 0xA4, 0xFA, 0x73,
                                                 0xE3, 0x26, 0x11, 0x22, 0x94, 0x8E, 0xFC, 0xFD, 0x49, 0x2E, 0x74,
                                                 0xE8, 0x2E, 0x21, 0x89, 0xED, 0x0F, 0xB4, 0x40, 0xD1, 0x87, 0xF3,
                                                 0x82, 0x27, 0x0C, 0xB4, 0x55, 0xF2, 0x1D, 0xD1, 0x85};
    assert(TRAP::Utils::Hash::SHA3_512("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu") == longerHash);

    static constexpr std::array<uint8_t, 64> aHash{0x3C, 0x3A, 0x87, 0x6D, 0xA1, 0x40, 0x34, 0xAB, 0x60, 0x62, 0x7C, 0x07,
                                            0x7B, 0xB9, 0x8F, 0x7E, 0x12, 0x0A, 0x2A, 0x53, 0x70, 0x21, 0x2D, 0xFF,
                                            0xB3, 0x38, 0x5A, 0x18, 0xD4, 0xF3, 0x88, 0x59, 0xED, 0x31, 0x1D, 0x0A,
                                            0x9D, 0x51, 0x41, 0xCE, 0x9C, 0xC5, 0xC6, 0x6E, 0xE6, 0x89, 0xB2, 0x66,
                                            0xA8, 0xAA, 0x18, 0xAC, 0xE8, 0x28, 0x2A, 0x0E, 0x0D, 0xB5, 0x96, 0xC9,
                                            0x0B, 0x0A, 0x7B, 0x87};
    const std::string aTest(1'000'000, 'a');
    assert(TRAP::Utils::Hash::SHA3_512(aTest) == aHash);

    static constexpr std::array<uint8_t, 64> extremeHash{0x23, 0x5F, 0xFD, 0x53, 0x50, 0x4E, 0xF8, 0x36, 0xA1, 0x34, 0x2B,
                                                  0x48, 0x8F, 0x48, 0x3B, 0x39, 0x6E, 0xAB, 0xBF, 0xE6, 0x42, 0xCF,
                                                  0x78, 0xEE, 0x0D, 0x31, 0xFE, 0xEC, 0x78, 0x8B, 0x23, 0xD0, 0xD1,
                                                  0x8D, 0x5C, 0x33, 0x95, 0x50, 0xDD, 0x59, 0x58, 0xA5, 0x00, 0xD4,
                                                  0xB9, 0x53, 0x63, 0xDA, 0x1B, 0x5F, 0xA1, 0x8A, 0xFF, 0xC1, 0xBA,
                                                  0xB2, 0x29, 0x2D, 0xC6, 0x3B, 0x7D, 0x85, 0x09, 0x7C};
    std::string extremeTest;
    extremeTest.reserve(16'777'216ull * 64);
    for(uint64_t i = 0; i < 16'777'216; ++i)
        extremeTest += "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno";
    assert(TRAP::Utils::Hash::SHA3_512(extremeTest) == extremeHash);
}
