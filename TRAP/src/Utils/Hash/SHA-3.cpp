#include "TRAPPCH.h"
#include "SHA-3.h"

#include "TRAP_Assert.h"

static constexpr std::array<uint64_t, 24> RC =
{
	0x0000000000000001, 0x0000000000008082, 0x800000000000808A, 0x8000000080008000,
	0x000000000000808B, 0x0000000080000001, 0x8000000080008081, 0x8000000000008009,
	0x000000000000008A, 0x0000000000000088, 0x0000000080008009, 0x000000008000000A,
	0x000000008000808B, 0x800000000000008B, 0x8000000000008089, 0x8000000000008003,
	0x8000000000008002, 0x8000000000000080,	0x000000000000800A, 0x800000008000000A,
	0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008
};

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint64_t Rotatel64(const uint64_t x, const int64_t y)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	static constexpr uint32_t thisSize = sizeof(uint64_t) * 8;
	static constexpr uint32_t mask = thisSize - 1;
	TRAP_ASSERT(y < thisSize, "Rotatel64(): y must be less than the size of the type!");

	return (x << y) | (x >> (-y & mask));
}

//-------------------------------------------------------------------------------------------------------------------//

void DoTransform(std::array<uint64_t, 25>& A)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	for (uint32_t round = 0; round < 24; round++)
	{
		std::array<uint64_t, 5> c{};
		std::array<uint64_t, 5> d{};
		std::get<0>(c) = std::get<0 * 5 + 0>(A) ^ std::get<1 * 5 + 0>(A) ^ std::get<2 * 5 + 0>(A) ^ std::get<3 * 5 + 0>(A) ^ std::get<4 * 5 + 0>(A);
		std::get<1>(c) = std::get<0 * 5 + 1>(A) ^ std::get<1 * 5 + 1>(A) ^ std::get<2 * 5 + 1>(A) ^ std::get<3 * 5 + 1>(A) ^ std::get<4 * 5 + 1>(A);
		std::get<2>(c) = std::get<0 * 5 + 2>(A) ^ std::get<1 * 5 + 2>(A) ^ std::get<2 * 5 + 2>(A) ^ std::get<3 * 5 + 2>(A) ^ std::get<4 * 5 + 2>(A);
		std::get<3>(c) = std::get<0 * 5 + 3>(A) ^ std::get<1 * 5 + 3>(A) ^ std::get<2 * 5 + 3>(A) ^ std::get<3 * 5 + 3>(A) ^ std::get<4 * 5 + 3>(A);
		std::get<4>(c) = std::get<0 * 5 + 4>(A) ^ std::get<1 * 5 + 4>(A) ^ std::get<2 * 5 + 4>(A) ^ std::get<3 * 5 + 4>(A) ^ std::get<4 * 5 + 4>(A);

		std::get<0>(d) = std::get<4>(c) ^ Rotatel64(std::get<1>(c), 1);
		std::get<1>(d) = std::get<0>(c) ^ Rotatel64(std::get<2>(c), 1);
		std::get<2>(d) = std::get<1>(c) ^ Rotatel64(std::get<3>(c), 1);
		std::get<3>(d) = std::get<2>(c) ^ Rotatel64(std::get<4>(c), 1);
		std::get<4>(d) = std::get<3>(c) ^ Rotatel64(std::get<0>(c), 1);

		const uint64_t b0  = std::get<0 * 5 + 0>(A) ^ std::get<0>(d);
		const uint64_t b10 = Rotatel64(std::get<0 * 5 + 1>(A) ^ std::get<1>(d), 1);
		const uint64_t b20 = Rotatel64(std::get<0 * 5 + 2>(A) ^ std::get<2>(d), 62);
		const uint64_t b5  = Rotatel64(std::get<0 * 5 + 3>(A) ^ std::get<3>(d), 28);
		const uint64_t b15 = Rotatel64(std::get<0 * 5 + 4>(A) ^ std::get<4>(d), 27);

		const uint64_t b16 = Rotatel64(std::get<1 * 5 + 0>(A) ^ std::get<0>(d), 36);
		const uint64_t b1  = Rotatel64(std::get<1 * 5 + 1>(A) ^ std::get<1>(d), 44);
		const uint64_t b11 = Rotatel64(std::get<1 * 5 + 2>(A) ^ std::get<2>(d), 6);
		const uint64_t b21 = Rotatel64(std::get<1 * 5 + 3>(A) ^ std::get<3>(d), 55);
		const uint64_t b6  = Rotatel64(std::get<1 * 5 + 4>(A) ^ std::get<4>(d), 20);

		const uint64_t b7  = Rotatel64(std::get<2 * 5 + 0>(A) ^ std::get<0>(d), 3);
		const uint64_t b17 = Rotatel64(std::get<2 * 5 + 1>(A) ^ std::get<1>(d), 10);
		const uint64_t b2  = Rotatel64(std::get<2 * 5 + 2>(A) ^ std::get<2>(d), 43);
		const uint64_t b12 = Rotatel64(std::get<2 * 5 + 3>(A) ^ std::get<3>(d), 25);
		const uint64_t b22 = Rotatel64(std::get<2 * 5 + 4>(A) ^ std::get<4>(d), 39);

		const uint64_t b23 = Rotatel64(std::get<3 * 5 + 0>(A) ^ std::get<0>(d), 41);
		const uint64_t b8  = Rotatel64(std::get<3 * 5 + 1>(A) ^ std::get<1>(d), 45);
		const uint64_t b18 = Rotatel64(std::get<3 * 5 + 2>(A) ^ std::get<2>(d), 15);
		const uint64_t b3  = Rotatel64(std::get<3 * 5 + 3>(A) ^ std::get<3>(d), 21);
		const uint64_t b13 = Rotatel64(std::get<3 * 5 + 4>(A) ^ std::get<4>(d), 8);

		const uint64_t b14 = Rotatel64(std::get<4 * 5 + 0>(A) ^ std::get<0>(d), 18);
		const uint64_t b24 = Rotatel64(std::get<4 * 5 + 1>(A) ^ std::get<1>(d), 2);
		const uint64_t b9  = Rotatel64(std::get<4 * 5 + 2>(A) ^ std::get<2>(d), 61);
		const uint64_t b19 = Rotatel64(std::get<4 * 5 + 3>(A) ^ std::get<3>(d), 56);
		const uint64_t b4  = Rotatel64(std::get<4 * 5 + 4>(A) ^ std::get<4>(d), 14);

		std::get<0 * 5 + 0>(A) = b0 ^ ((~b1) & b2);
		std::get<0 * 5 + 1>(A) = b1 ^ ((~b2) & b3);
		std::get<0 * 5 + 2>(A) = b2 ^ ((~b3) & b4);
		std::get<0 * 5 + 3>(A) = b3 ^ ((~b4) & b0);
		std::get<0 * 5 + 4>(A) = b4 ^ ((~b0) & b1);

		std::get<1 * 5 + 0>(A) = b5 ^ ((~b6) & b7);
		std::get<1 * 5 + 1>(A) = b6 ^ ((~b7) & b8);
		std::get<1 * 5 + 2>(A) = b7 ^ ((~b8) & b9);
		std::get<1 * 5 + 3>(A) = b8 ^ ((~b9) & b5);
		std::get<1 * 5 + 4>(A) = b9 ^ ((~b5) & b6);

		std::get<2 * 5 + 0>(A) = b10 ^ ((~b11) & b12);
		std::get<2 * 5 + 1>(A) = b11 ^ ((~b12) & b13);
		std::get<2 * 5 + 2>(A) = b12 ^ ((~b13) & b14);
		std::get<2 * 5 + 3>(A) = b13 ^ ((~b14) & b10);
		std::get<2 * 5 + 4>(A) = b14 ^ ((~b10) & b11);

		std::get<3 * 5 + 0>(A) = b15 ^ ((~b16) & b17);
		std::get<3 * 5 + 1>(A) = b16 ^ ((~b17) & b18);
		std::get<3 * 5 + 2>(A) = b17 ^ ((~b18) & b19);
		std::get<3 * 5 + 3>(A) = b18 ^ ((~b19) & b15);
		std::get<3 * 5 + 4>(A) = b19 ^ ((~b15) & b16);

		std::get<4 * 5 + 0>(A) = b20 ^ ((~b21) & b22);
		std::get<4 * 5 + 1>(A) = b21 ^ ((~b22) & b23);
		std::get<4 * 5 + 2>(A) = b22 ^ ((~b23) & b24);
		std::get<4 * 5 + 3>(A) = b23 ^ ((~b24) & b20);
		std::get<4 * 5 + 4>(A) = b24 ^ ((~b20) & b21);

		std::get<0>(A) ^= RC[round];
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void Transform(const void* const mp, const uint64_t numBlks, std::array<uint64_t, 25>& A, const std::size_t rate)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	const std::size_t r = rate / 8;
	const std::size_t r64 = rate / 64;
	for(uint64_t blk = 0; blk < numBlks; blk++)
	{
		for (std::size_t i = 0; i < r64; i++)
			A[i] ^= reinterpret_cast<const uint64_t*>(static_cast<const char*>(mp) + blk * r)[i];

		DoTransform(A);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<uint8_t, 32> TRAP::Utils::Hash::SHA3_256(const void* const data, uint64_t length)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	static constexpr std::size_t hs = 256;
	static constexpr std::size_t rate = 1600U - hs * 2;
	std::array<uint8_t, rate / 8> m{};
	std::size_t pos = 0;
	std::array<uint64_t, 25> A{};

	const uint8_t* dataPtr = static_cast<const uint8_t*>(data);
	static constexpr std::size_t r = rate / 8;
	if(length >= r)
	{
		const std::size_t blocks = length / r;
		const std::size_t bytes = blocks * r;
		Transform(dataPtr, blocks, A, rate);
		length -= bytes;
		dataPtr += bytes;
	}
	std::copy_n(dataPtr, length, m.data() + pos);
	pos += length;

	m[pos++] = 0x06;
	std::fill_n(m.data() + pos, r - pos, NumericCast<uint8_t>(0u));
	m[r - 1] |= 0x80;
	Transform(m.data(), 1, A, rate);

	std::array<uint8_t, hs / 8> result{};
	std::copy_n(reinterpret_cast<const uint8_t*>(A.data()), result.size(), result.data());

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<uint8_t, 32> TRAP::Utils::Hash::SHA3_256(const std::string_view str)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return SHA3_256(str.data(), str.length());
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<uint8_t, 64> TRAP::Utils::Hash::SHA3_512(const void* const data, uint64_t length)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	static constexpr std::size_t hs = 512;
	static constexpr std::size_t rate = 1600U - hs * 2;
	std::array<uint8_t, rate / 8> m{};
	std::size_t pos = 0;
	std::array<uint64_t, 25> A{};

	const uint8_t* dataPtr = static_cast<const uint8_t*>(data);
	static constexpr std::size_t r = rate / 8;
	if (length >= r)
	{
		const std::size_t blocks = length / r;
		const std::size_t bytes = blocks * r;
		Transform(dataPtr, blocks, A, rate);
		length -= bytes;
		dataPtr += bytes;
	}
	std::copy_n(dataPtr, length, m.data() + pos);
	pos += length;

	m[pos++] = 0x06;
	std::fill_n(m.data() + pos, r - pos, NumericCast<uint8_t>(0u));
	m[r - 1] |= 0x80;
	Transform(m.data(), 1, A, rate);

	std::array<uint8_t, hs / 8> result{};
	std::copy_n(reinterpret_cast<const uint8_t*>(A.data()), result.size(), result.data());

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<uint8_t, 64> TRAP::Utils::Hash::SHA3_512(const std::string_view str)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return SHA3_512(str.data(), str.length());
}