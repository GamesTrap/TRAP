#include "TRAPPCH.h"
#include "SHA-3.h"

constexpr std::array<uint64_t, 24> RC =
{
	0x0000000000000001, 0x0000000000008082, 0x800000000000808A, 0x8000000080008000, 0x000000000000808B, 0x0000000080000001,
	0x8000000080008081, 0x8000000000008009, 0x000000000000008A, 0x0000000000000088, 0x0000000080008009, 0x000000008000000A,
	0x000000008000808B, 0x800000000000008B, 0x8000000000008089, 0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
	0x000000000000800A, 0x800000008000000A, 0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008
};

//-------------------------------------------------------------------------------------------------------------------//

template<class T> T Rotatel64(T x, int64_t y)
{
	static const uint32_t thisSize = sizeof(T) * 8;
	static const uint32_t mask = thisSize - 1;
	assert(y < thisSize);

	return T((x << y) | (x >> (-y & mask)));
}

//-------------------------------------------------------------------------------------------------------------------//

void DoTransform(std::array<uint64_t, 25>& A)
{
	for (int round = 0; round < 24; round++)
	{
		std::array<uint64_t, 5> C{};
		std::array<uint64_t, 5> D{};
		C[0] = A[0 * 5 + 0] ^ A[1 * 5 + 0] ^ A[2 * 5 + 0] ^ A[3 * 5 + 0] ^ A[4 * 5 + 0];
		C[1] = A[0 * 5 + 1] ^ A[1 * 5 + 1] ^ A[2 * 5 + 1] ^ A[3 * 5 + 1] ^ A[4 * 5 + 1];
		C[2] = A[0 * 5 + 2] ^ A[1 * 5 + 2] ^ A[2 * 5 + 2] ^ A[3 * 5 + 2] ^ A[4 * 5 + 2];
		C[3] = A[0 * 5 + 3] ^ A[1 * 5 + 3] ^ A[2 * 5 + 3] ^ A[3 * 5 + 3] ^ A[4 * 5 + 3];
		C[4] = A[0 * 5 + 4] ^ A[1 * 5 + 4] ^ A[2 * 5 + 4] ^ A[3 * 5 + 4] ^ A[4 * 5 + 4];

		D[0] = C[4] ^ Rotatel64(C[1], 1);
		D[1] = C[0] ^ Rotatel64(C[2], 1);
		D[2] = C[1] ^ Rotatel64(C[3], 1);
		D[3] = C[2] ^ Rotatel64(C[4], 1);
		D[4] = C[3] ^ Rotatel64(C[0], 1);

		const uint64_t B0 = A[0 * 5 + 0] ^ D[0];
		const uint64_t B10 = Rotatel64(A[0 * 5 + 1] ^ D[1], 1);
		const uint64_t B20 = Rotatel64(A[0 * 5 + 2] ^ D[2], 62);
		const uint64_t B5 = Rotatel64(A[0 * 5 + 3] ^ D[3], 28);
		const uint64_t B15 = Rotatel64(A[0 * 5 + 4] ^ D[4], 27);

		const uint64_t B16 = Rotatel64(A[1 * 5 + 0] ^ D[0], 36);
		const uint64_t B1 = Rotatel64(A[1 * 5 + 1] ^ D[1], 44);
		const uint64_t B11 = Rotatel64(A[1 * 5 + 2] ^ D[2], 6);
		const uint64_t B21 = Rotatel64(A[1 * 5 + 3] ^ D[3], 55);
		const uint64_t B6 = Rotatel64(A[1 * 5 + 4] ^ D[4], 20);

		const uint64_t B7 = Rotatel64(A[2 * 5 + 0] ^ D[0], 3);
		const uint64_t B17 = Rotatel64(A[2 * 5 + 1] ^ D[1], 10);
		const uint64_t B2 = Rotatel64(A[2 * 5 + 2] ^ D[2], 43);
		const uint64_t B12 = Rotatel64(A[2 * 5 + 3] ^ D[3], 25);
		const uint64_t B22 = Rotatel64(A[2 * 5 + 4] ^ D[4], 39);

		const uint64_t B23 = Rotatel64(A[3 * 5 + 0] ^ D[0], 41);
		const uint64_t B8 = Rotatel64(A[3 * 5 + 1] ^ D[1], 45);
		const uint64_t B18 = Rotatel64(A[3 * 5 + 2] ^ D[2], 15);
		const uint64_t B3 = Rotatel64(A[3 * 5 + 3] ^ D[3], 21);
		const uint64_t B13 = Rotatel64(A[3 * 5 + 4] ^ D[4], 8);

		const uint64_t B14 = Rotatel64(A[4 * 5 + 0] ^ D[0], 18);
		const uint64_t B24 = Rotatel64(A[4 * 5 + 1] ^ D[1], 2);
		const uint64_t B9 = Rotatel64(A[4 * 5 + 2] ^ D[2], 61);
		const uint64_t B19 = Rotatel64(A[4 * 5 + 3] ^ D[3], 56);
		const uint64_t B4 = Rotatel64(A[4 * 5 + 4] ^ D[4], 14);

		A[0 * 5 + 0] = B0 ^ ((~B1) & B2);
		A[0 * 5 + 1] = B1 ^ ((~B2) & B3);
		A[0 * 5 + 2] = B2 ^ ((~B3) & B4);
		A[0 * 5 + 3] = B3 ^ ((~B4) & B0);
		A[0 * 5 + 4] = B4 ^ ((~B0) & B1);

		A[1 * 5 + 0] = B5 ^ ((~B6) & B7);
		A[1 * 5 + 1] = B6 ^ ((~B7) & B8);
		A[1 * 5 + 2] = B7 ^ ((~B8) & B9);
		A[1 * 5 + 3] = B8 ^ ((~B9) & B5);
		A[1 * 5 + 4] = B9 ^ ((~B5) & B6);

		A[2 * 5 + 0] = B10 ^ ((~B11) & B12);
		A[2 * 5 + 1] = B11 ^ ((~B12) & B13);
		A[2 * 5 + 2] = B12 ^ ((~B13) & B14);
		A[2 * 5 + 3] = B13 ^ ((~B14) & B10);
		A[2 * 5 + 4] = B14 ^ ((~B10) & B11);

		A[3 * 5 + 0] = B15 ^ ((~B16) & B17);
		A[3 * 5 + 1] = B16 ^ ((~B17) & B18);
		A[3 * 5 + 2] = B17 ^ ((~B18) & B19);
		A[3 * 5 + 3] = B18 ^ ((~B19) & B15);
		A[3 * 5 + 4] = B19 ^ ((~B15) & B16);

		A[4 * 5 + 0] = B20 ^ ((~B21) & B22);
		A[4 * 5 + 1] = B21 ^ ((~B22) & B23);
		A[4 * 5 + 2] = B22 ^ ((~B23) & B24);
		A[4 * 5 + 3] = B23 ^ ((~B24) & B20);
		A[4 * 5 + 4] = B24 ^ ((~B20) & B21);

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
	memset(A.data(), 0, A.size());

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
	memcpy(m.data() + pos, dataPtr, length);
	pos += length;

	m[pos++] = 0x06;
	memset(m.data() + pos, 0, r - pos);
	m[r - 1] |= 0x80;
	Transform(m.data(), 1, A, rate);

	std::array<uint8_t, hs / 8> result{};
	memcpy(result.data(), A.data(), result.size());

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<uint8_t, 32> TRAP::Utils::Hash::SHA3_256(const std::string& str)
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
	memset(A.data(), 0, A.size());

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
	memcpy(m.data() + pos, dataPtr, length);
	pos += length;

	m[pos++] = 0x06;
	memset(m.data() + pos, 0, r - pos);
	m[r - 1] |= 0x80;
	Transform(m.data(), 1, A, rate);

	std::array<uint8_t, hs / 8> result{};
	memcpy(result.data(), A.data(), result.size());

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<uint8_t, 64> TRAP::Utils::Hash::SHA3_512(const std::string& str)
{
	return SHA3_512(str.data(), str.length());
}