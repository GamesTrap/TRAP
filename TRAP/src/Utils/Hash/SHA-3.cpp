#include "TRAPPCH.h"
#include "SHA-3.h"
#include "TRAP_Assert.h"

constexpr std::array<uint64_t, 24> RC =
{
	0x0000000000000001, 0x0000000000008082, 0x800000000000808A, 0x8000000080008000,
	0x000000000000808B, 0x0000000080000001, 0x8000000080008081, 0x8000000000008009,
	0x000000000000008A, 0x0000000000000088, 0x0000000080008009, 0x000000008000000A,
	0x000000008000808B, 0x800000000000008B, 0x8000000000008089, 0x8000000000008003,
	0x8000000000008002, 0x8000000000000080,	0x000000000000800A, 0x800000008000000A,
	0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008
};

//-------------------------------------------------------------------------------------------------------------------//

template<class T> T Rotatel64(T x, int64_t y)
{
	static const uint32_t thisSize = sizeof(T) * 8;
	static const uint32_t mask = thisSize - 1;
	TRAP_ASSERT(y < thisSize);

	return T((x << y) | (x >> (-y & mask)));
}

//-------------------------------------------------------------------------------------------------------------------//

void DoTransform(std::array<uint64_t, 25>& A)
{
	for (uint32_t round = 0; round < 24; round++)
	{
		std::array<uint64_t, 5> c{};
		std::array<uint64_t, 5> d{};
		c[0] = A[0 * 5 + 0] ^ A[1 * 5 + 0] ^ A[2 * 5 + 0] ^ A[3 * 5 + 0] ^ A[4 * 5 + 0];
		c[1] = A[0 * 5 + 1] ^ A[1 * 5 + 1] ^ A[2 * 5 + 1] ^ A[3 * 5 + 1] ^ A[4 * 5 + 1];
		c[2] = A[0 * 5 + 2] ^ A[1 * 5 + 2] ^ A[2 * 5 + 2] ^ A[3 * 5 + 2] ^ A[4 * 5 + 2];
		c[3] = A[0 * 5 + 3] ^ A[1 * 5 + 3] ^ A[2 * 5 + 3] ^ A[3 * 5 + 3] ^ A[4 * 5 + 3];
		c[4] = A[0 * 5 + 4] ^ A[1 * 5 + 4] ^ A[2 * 5 + 4] ^ A[3 * 5 + 4] ^ A[4 * 5 + 4];

		d[0] = c[4] ^ Rotatel64(c[1], 1);
		d[1] = c[0] ^ Rotatel64(c[2], 1);
		d[2] = c[1] ^ Rotatel64(c[3], 1);
		d[3] = c[2] ^ Rotatel64(c[4], 1);
		d[4] = c[3] ^ Rotatel64(c[0], 1);

		const uint64_t b0 = A[0 * 5 + 0] ^ d[0];
		const uint64_t b10 = Rotatel64(A[0 * 5 + 1] ^ d[1], 1);
		const uint64_t b20 = Rotatel64(A[0 * 5 + 2] ^ d[2], 62);
		const uint64_t b5 = Rotatel64(A[0 * 5 + 3] ^ d[3], 28);
		const uint64_t b15 = Rotatel64(A[0 * 5 + 4] ^ d[4], 27);

		const uint64_t b16 = Rotatel64(A[1 * 5 + 0] ^ d[0], 36);
		const uint64_t b1 = Rotatel64(A[1 * 5 + 1] ^ d[1], 44);
		const uint64_t b11 = Rotatel64(A[1 * 5 + 2] ^ d[2], 6);
		const uint64_t b21 = Rotatel64(A[1 * 5 + 3] ^ d[3], 55);
		const uint64_t b6 = Rotatel64(A[1 * 5 + 4] ^ d[4], 20);

		const uint64_t b7 = Rotatel64(A[2 * 5 + 0] ^ d[0], 3);
		const uint64_t b17 = Rotatel64(A[2 * 5 + 1] ^ d[1], 10);
		const uint64_t b2 = Rotatel64(A[2 * 5 + 2] ^ d[2], 43);
		const uint64_t b12 = Rotatel64(A[2 * 5 + 3] ^ d[3], 25);
		const uint64_t b22 = Rotatel64(A[2 * 5 + 4] ^ d[4], 39);

		const uint64_t b23 = Rotatel64(A[3 * 5 + 0] ^ d[0], 41);
		const uint64_t b8 = Rotatel64(A[3 * 5 + 1] ^ d[1], 45);
		const uint64_t b18 = Rotatel64(A[3 * 5 + 2] ^ d[2], 15);
		const uint64_t b3 = Rotatel64(A[3 * 5 + 3] ^ d[3], 21);
		const uint64_t b13 = Rotatel64(A[3 * 5 + 4] ^ d[4], 8);

		const uint64_t b14 = Rotatel64(A[4 * 5 + 0] ^ d[0], 18);
		const uint64_t b24 = Rotatel64(A[4 * 5 + 1] ^ d[1], 2);
		const uint64_t b9 = Rotatel64(A[4 * 5 + 2] ^ d[2], 61);
		const uint64_t b19 = Rotatel64(A[4 * 5 + 3] ^ d[3], 56);
		const uint64_t b4 = Rotatel64(A[4 * 5 + 4] ^ d[4], 14);

		A[0 * 5 + 0] = b0 ^ ((~b1) & b2);
		A[0 * 5 + 1] = b1 ^ ((~b2) & b3);
		A[0 * 5 + 2] = b2 ^ ((~b3) & b4);
		A[0 * 5 + 3] = b3 ^ ((~b4) & b0);
		A[0 * 5 + 4] = b4 ^ ((~b0) & b1);

		A[1 * 5 + 0] = b5 ^ ((~b6) & b7);
		A[1 * 5 + 1] = b6 ^ ((~b7) & b8);
		A[1 * 5 + 2] = b7 ^ ((~b8) & b9);
		A[1 * 5 + 3] = b8 ^ ((~b9) & b5);
		A[1 * 5 + 4] = b9 ^ ((~b5) & b6);

		A[2 * 5 + 0] = b10 ^ ((~b11) & b12);
		A[2 * 5 + 1] = b11 ^ ((~b12) & b13);
		A[2 * 5 + 2] = b12 ^ ((~b13) & b14);
		A[2 * 5 + 3] = b13 ^ ((~b14) & b10);
		A[2 * 5 + 4] = b14 ^ ((~b10) & b11);

		A[3 * 5 + 0] = b15 ^ ((~b16) & b17);
		A[3 * 5 + 1] = b16 ^ ((~b17) & b18);
		A[3 * 5 + 2] = b17 ^ ((~b18) & b19);
		A[3 * 5 + 3] = b18 ^ ((~b19) & b15);
		A[3 * 5 + 4] = b19 ^ ((~b15) & b16);

		A[4 * 5 + 0] = b20 ^ ((~b21) & b22);
		A[4 * 5 + 1] = b21 ^ ((~b22) & b23);
		A[4 * 5 + 2] = b22 ^ ((~b23) & b24);
		A[4 * 5 + 3] = b23 ^ ((~b24) & b20);
		A[4 * 5 + 4] = b24 ^ ((~b20) & b21);

		A[0] ^= RC[round];
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void Transform(const void* mp, const uint64_t numBlks, std::array<uint64_t, 25>& A, const std::size_t rate)
{
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

std::array<uint8_t, 32> TRAP::Utils::Hash::SHA3_256(const void* data, uint64_t length)
{
	constexpr std::size_t hs = 256;
	constexpr std::size_t rate = 1600U - hs * 2;
	std::array<uint8_t, rate / 8> m{};
	std::size_t pos = 0;
	std::array<uint64_t, 25> A{};

	const uint8_t* dataPtr = static_cast<const uint8_t*>(data);
	constexpr std::size_t r = rate / 8;
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
	std::fill_n(m.data() + pos, r - pos, 0u);
	m[r - 1] |= 0x80;
	Transform(m.data(), 1, A, rate);

	std::array<uint8_t, hs / 8> result{};
	std::copy_n(reinterpret_cast<const uint8_t*>(A.data()), result.size(), result.data());

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<uint8_t, 32> TRAP::Utils::Hash::SHA3_256(const std::string_view str)
{
	return SHA3_256(str.data(), str.length());
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<uint8_t, 64> TRAP::Utils::Hash::SHA3_512(const void* data, uint64_t length)
{
	constexpr std::size_t hs = 512;
	constexpr std::size_t rate = 1600U - hs * 2;
	std::array<uint8_t, rate / 8> m{};
	std::size_t pos = 0;
	std::array<uint64_t, 25> A{};

	const uint8_t* dataPtr = static_cast<const uint8_t*>(data);
	constexpr std::size_t r = rate / 8;
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
	std::fill_n(m.data() + pos, r - pos, 0u);
	m[r - 1] |= 0x80;
	Transform(m.data(), 1, A, rate);

	std::array<uint8_t, hs / 8> result{};
	std::copy_n(reinterpret_cast<const uint8_t*>(A.data()), result.size(), result.data());

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<uint8_t, 64> TRAP::Utils::Hash::SHA3_512(const std::string_view str)
{
	return SHA3_512(str.data(), str.length());
}