#include "TRAPPCH.h"
#include "SHA-3.h"

#include "TRAP_Assert.h"

namespace
{
	constexpr std::array<u64, 24u> RC =
	{
		0x0000000000000001, 0x0000000000008082, 0x800000000000808A, 0x8000000080008000,
		0x000000000000808B, 0x0000000080000001, 0x8000000080008081, 0x8000000000008009,
		0x000000000000008A, 0x0000000000000088, 0x0000000080008009, 0x000000008000000A,
		0x000000008000808B, 0x800000000000008B, 0x8000000000008089, 0x8000000000008003,
		0x8000000000008002, 0x8000000000000080,	0x000000000000800A, 0x800000008000000A,
		0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008
	};

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u64 Rotatel64(const u64 x, const i64 y)
	{
		constexpr u32 thisSize = sizeof(u64) * 8u;
		constexpr u32 mask = thisSize - 1u;
		TRAP_ASSERT(y < thisSize, "Rotatel64(): y must be less than the size of the type!");

		return (x << y) | (x >> (-y & mask));
	}

	//-------------------------------------------------------------------------------------------------------------------//

	constexpr void DoTransform(std::array<u64, 25u>& A)
	{
		for (u32 round = 0u; round < 24u; ++round)
		{
			std::array<u64, 5u> c{};
			std::array<u64, 5u> d{};
			std::get<0u>(c) = std::get<(0u * 5u) + 0u>(A) ^ std::get<(1u * 5u) + 0u>(A) ^ std::get<(2u * 5u) + 0u>(A) ^ std::get<(3u * 5u) + 0u>(A) ^ std::get<(4u * 5u) + 0u>(A);
			std::get<1u>(c) = std::get<(0u * 5u) + 1u>(A) ^ std::get<(1u * 5u) + 1u>(A) ^ std::get<(2u * 5u) + 1u>(A) ^ std::get<(3u * 5u) + 1u>(A) ^ std::get<(4u * 5u) + 1u>(A);
			std::get<2u>(c) = std::get<(0u * 5u) + 2u>(A) ^ std::get<(1u * 5u) + 2u>(A) ^ std::get<(2u * 5u) + 2u>(A) ^ std::get<(3u * 5u) + 2u>(A) ^ std::get<(4u * 5u) + 2u>(A);
			std::get<3u>(c) = std::get<(0u * 5u) + 3u>(A) ^ std::get<(1u * 5u) + 3u>(A) ^ std::get<(2u * 5u) + 3u>(A) ^ std::get<(3u * 5u) + 3u>(A) ^ std::get<(4u * 5u) + 3u>(A);
			std::get<4u>(c) = std::get<(0u * 5u) + 4u>(A) ^ std::get<(1u * 5u) + 4u>(A) ^ std::get<(2u * 5u) + 4u>(A) ^ std::get<(3u * 5u) + 4u>(A) ^ std::get<(4u * 5u) + 4u>(A);

			std::get<0u>(d) = std::get<4u>(c) ^ Rotatel64(std::get<1u>(c), 1);
			std::get<1u>(d) = std::get<0u>(c) ^ Rotatel64(std::get<2u>(c), 1);
			std::get<2u>(d) = std::get<1u>(c) ^ Rotatel64(std::get<3u>(c), 1);
			std::get<3u>(d) = std::get<2u>(c) ^ Rotatel64(std::get<4u>(c), 1);
			std::get<4u>(d) = std::get<3u>(c) ^ Rotatel64(std::get<0u>(c), 1);

			const u64 b0  = std::get<(0u * 5u) + 0u>(A) ^ std::get<0u>(d);
			const u64 b10 = Rotatel64(std::get<(0u * 5u) + 1u>(A) ^ std::get<1u>(d), 1);
			const u64 b20 = Rotatel64(std::get<(0u * 5u) + 2u>(A) ^ std::get<2u>(d), 62);
			const u64 b5  = Rotatel64(std::get<(0u * 5u) + 3u>(A) ^ std::get<3u>(d), 28);
			const u64 b15 = Rotatel64(std::get<(0u * 5u) + 4u>(A) ^ std::get<4u>(d), 27);

			const u64 b16 = Rotatel64(std::get<(1u * 5u) + 0u>(A) ^ std::get<0u>(d), 36);
			const u64 b1  = Rotatel64(std::get<(1u * 5u) + 1u>(A) ^ std::get<1u>(d), 44);
			const u64 b11 = Rotatel64(std::get<(1u * 5u) + 2u>(A) ^ std::get<2u>(d), 6);
			const u64 b21 = Rotatel64(std::get<(1u * 5u) + 3u>(A) ^ std::get<3u>(d), 55);
			const u64 b6  = Rotatel64(std::get<(1u * 5u) + 4u>(A) ^ std::get<4u>(d), 20);

			const u64 b7  = Rotatel64(std::get<(2u * 5u) + 0u>(A) ^ std::get<0u>(d), 3);
			const u64 b17 = Rotatel64(std::get<(2u * 5u) + 1u>(A) ^ std::get<1u>(d), 10);
			const u64 b2  = Rotatel64(std::get<(2u * 5u) + 2u>(A) ^ std::get<2u>(d), 43);
			const u64 b12 = Rotatel64(std::get<(2u * 5u) + 3u>(A) ^ std::get<3u>(d), 25);
			const u64 b22 = Rotatel64(std::get<(2u * 5u) + 4u>(A) ^ std::get<4u>(d), 39);

			const u64 b23 = Rotatel64(std::get<(3u * 5u) + 0u>(A) ^ std::get<0u>(d), 41);
			const u64 b8  = Rotatel64(std::get<(3u * 5u) + 1u>(A) ^ std::get<1u>(d), 45);
			const u64 b18 = Rotatel64(std::get<(3u * 5u) + 2u>(A) ^ std::get<2u>(d), 15);
			const u64 b3  = Rotatel64(std::get<(3u * 5u) + 3u>(A) ^ std::get<3u>(d), 21);
			const u64 b13 = Rotatel64(std::get<(3u * 5u) + 4u>(A) ^ std::get<4u>(d), 8);

			const u64 b14 = Rotatel64(std::get<(4u * 5u) + 0u>(A) ^ std::get<0u>(d), 18);
			const u64 b24 = Rotatel64(std::get<(4u * 5u) + 1u>(A) ^ std::get<1u>(d), 2);
			const u64 b9  = Rotatel64(std::get<(4u * 5u) + 2u>(A) ^ std::get<2u>(d), 61);
			const u64 b19 = Rotatel64(std::get<(4u * 5u) + 3u>(A) ^ std::get<3u>(d), 56);
			const u64 b4  = Rotatel64(std::get<(4u * 5u) + 4u>(A) ^ std::get<4u>(d), 14);

			std::get<(0u * 5u) + 0u>(A) = b0 ^ ((~b1) & b2);
			std::get<(0u * 5u) + 1u>(A) = b1 ^ ((~b2) & b3);
			std::get<(0u * 5u) + 2u>(A) = b2 ^ ((~b3) & b4);
			std::get<(0u * 5u) + 3u>(A) = b3 ^ ((~b4) & b0);
			std::get<(0u * 5u) + 4u>(A) = b4 ^ ((~b0) & b1);

			std::get<(1u * 5u) + 0u>(A) = b5 ^ ((~b6) & b7);
			std::get<(1u * 5u) + 1u>(A) = b6 ^ ((~b7) & b8);
			std::get<(1u * 5u) + 2u>(A) = b7 ^ ((~b8) & b9);
			std::get<(1u * 5u) + 3u>(A) = b8 ^ ((~b9) & b5);
			std::get<(1u * 5u) + 4u>(A) = b9 ^ ((~b5) & b6);

			std::get<(2u * 5u) + 0u>(A) = b10 ^ ((~b11) & b12);
			std::get<(2u * 5u) + 1u>(A) = b11 ^ ((~b12) & b13);
			std::get<(2u * 5u) + 2u>(A) = b12 ^ ((~b13) & b14);
			std::get<(2u * 5u) + 3u>(A) = b13 ^ ((~b14) & b10);
			std::get<(2u * 5u) + 4u>(A) = b14 ^ ((~b10) & b11);

			std::get<(3u * 5u) + 0u>(A) = b15 ^ ((~b16) & b17);
			std::get<(3u * 5u) + 1u>(A) = b16 ^ ((~b17) & b18);
			std::get<(3u * 5u) + 2u>(A) = b17 ^ ((~b18) & b19);
			std::get<(3u * 5u) + 3u>(A) = b18 ^ ((~b19) & b15);
			std::get<(3u * 5u) + 4u>(A) = b19 ^ ((~b15) & b16);

			std::get<(4u * 5u) + 0u>(A) = b20 ^ ((~b21) & b22);
			std::get<(4u * 5u) + 1u>(A) = b21 ^ ((~b22) & b23);
			std::get<(4u * 5u) + 2u>(A) = b22 ^ ((~b23) & b24);
			std::get<(4u * 5u) + 3u>(A) = b23 ^ ((~b24) & b20);
			std::get<(4u * 5u) + 4u>(A) = b24 ^ ((~b20) & b21);

			std::get<0u>(A) ^= RC[round];
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void Transform(const void* const mp, const u64 numBlks, std::array<u64, 25u>& A, const usize rate)
	{
		ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

		const usize r = rate / 8u;
		const usize r64 = rate / 64u;
		for(u64 blk = 0u; blk < numBlks; ++blk)
		{
			for (usize i = 0u; i < r64; ++i)
				A[i] ^= reinterpret_cast<const u64*>(static_cast<const char*>(mp) + blk * r)[i];

			DoTransform(A);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<u8, 32u> TRAP::Utils::Hash::SHA3_256(const void* const data, u64 length)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	static constexpr usize hs = 256u;
	static constexpr usize rate = 1600u - hs * 2u;
	std::array<u8, rate / 8u> m{};
	usize pos = 0u;
	std::array<u64, 25u> A{};

	const u8* dataPtr = static_cast<const u8*>(data);
	static constexpr usize r = rate / 8u;
	if(length >= r)
	{
		const usize blocks = length / r;
		const usize bytes = blocks * r;
		Transform(dataPtr, blocks, A, rate);
		length -= bytes;
		dataPtr += bytes;
	}
	std::copy_n(dataPtr, length, m.data() + pos);
	pos += length;

	m[pos++] = 0x06u;
	std::fill_n(m.data() + pos, r - pos, NumericCast<u8>(0u));
	m[r - 1u] |= 0x80u;
	Transform(m.data(), 1u, A, rate);

	std::array<u8, hs / 8u> result{};
	std::copy_n(reinterpret_cast<const u8*>(A.data()), result.size(), result.data());

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<u8, 32u> TRAP::Utils::Hash::SHA3_256(const std::string_view str)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return SHA3_256(str.data(), str.length());
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<u8, 64u> TRAP::Utils::Hash::SHA3_512(const void* const data, u64 length)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	static constexpr usize hs = 512u;
	static constexpr usize rate = 1600u - hs * 2u;
	std::array<u8, rate / 8u> m{};
	usize pos = 0u;
	std::array<u64, 25u> A{};

	const u8* dataPtr = static_cast<const u8*>(data);
	static constexpr usize r = rate / 8u;
	if (length >= r)
	{
		const usize blocks = length / r;
		const usize bytes = blocks * r;
		Transform(dataPtr, blocks, A, rate);
		length -= bytes;
		dataPtr += bytes;
	}
	std::copy_n(dataPtr, length, m.data() + pos);
	pos += length;

	m[pos++] = 0x06u;
	std::fill_n(m.data() + pos, r - pos, NumericCast<u8>(0u));
	m[r - 1u] |= 0x80u;
	Transform(m.data(), 1u, A, rate);

	std::array<u8, hs / 8u> result{};
	std::copy_n(reinterpret_cast<const u8*>(A.data()), result.size(), result.data());

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<u8, 64u> TRAP::Utils::Hash::SHA3_512(const std::string_view str)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return SHA3_512(str.data(), str.length());
}
