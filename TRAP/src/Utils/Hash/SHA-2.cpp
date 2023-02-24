#include "TRAPPCH.h"
#include "SHA-2.h"

#include "Utils/Memory.h"

static constexpr std::array<uint32_t, 64> SHA256_K =
{
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

//-------------------------------------------------------------------------------------------------------------------//

static constexpr std::array<uint64_t, 80> SHA512_K =
{
	0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
	0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
	0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
	0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
	0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
	0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
	0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
	0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
	0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
	0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
	0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
	0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
	0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
	0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
	0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
	0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
	0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
	0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
	0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
	0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t Rotr(const uint32_t x, const int32_t n) noexcept
{
	return (x >> n) | (x << (32 - n));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint64_t Rotr(const uint64_t x, const int32_t n) noexcept
{
	return (x >> n) | (x << (64 - n));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t Shr(const uint32_t x, const int32_t n) noexcept
{
	return x >> n;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint64_t Shr(const uint64_t x, const int32_t n) noexcept
{
	return x >> n;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t Ch(const uint32_t x, const uint32_t y, const uint32_t z) noexcept
{
	return (x & y) ^ (~x & z);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint64_t Ch(const uint64_t x, const uint64_t y, const uint64_t z) noexcept
{
	return (x & y) ^ (~x & z);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t Maj(const uint32_t x, const uint32_t y, const uint32_t z) noexcept
{
	return (x & y) ^ (x & z) ^ (y & z);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint64_t Maj(const uint64_t x, const uint64_t y, const uint64_t z) noexcept
{
	return (x & y) ^ (x & z) ^ (y & z);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t Sum0(const uint32_t x) noexcept
{
	return Rotr(x, 2) ^ Rotr(x, 13) ^ Rotr(x, 22);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint64_t Sum0(const uint64_t x) noexcept
{
	return Rotr(x, 28) ^ Rotr(x, 34) ^ Rotr(x, 39);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t Sum1(const uint32_t x) noexcept
{
	return Rotr(x, 6) ^ Rotr(x, 11) ^ Rotr(x, 25);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint64_t Sum1(const uint64_t x) noexcept
{
	return Rotr(x, 14) ^ Rotr(x, 18) ^ Rotr(x, 41);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t Sigma0(const uint32_t x) noexcept
{
	return Rotr(x, 7) ^ Rotr(x, 18) ^ Shr(x, 3);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint64_t Sigma0(const uint64_t x) noexcept
{
	return Rotr(x, 1) ^ Rotr(x, 8) ^ Shr(x, 7);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t Sigma1(const uint32_t x) noexcept
{
	return Rotr(x, 17) ^ Rotr(x, 19) ^ Shr(x, 10);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint64_t Sigma1(const uint64_t x) noexcept
{
	return Rotr(x, 19) ^ Rotr(x, 61) ^ Shr(x, 6);
}

//-------------------------------------------------------------------------------------------------------------------//

void Transform(const void* const mp, const uint64_t numBlks, std::array<uint32_t, 8>& hash)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	for(uint64_t blk = 0; blk < numBlks; blk++)
	{
		std::array<uint32_t, 16> M{};
		for (uint32_t i = 0; i < 64 / 4; i++)
		{
			uint32_t tmp = static_cast<const uint32_t*>(mp)[blk * 16 + i];
			TRAP::Utils::Memory::SwapBytes<uint32_t>(tmp);
			M[i] = tmp;
		}

		std::array<uint32_t, 64> W{};
		for (uint32_t t = 0; t <= 15; t++)
			W[t] = M[t];
		for (uint32_t t = 16; t <= 63; t++)
			W[t] = Sigma1(W[t - 2]) + W[t - 7] + Sigma0(W[t - 15]) + W[t - 16];

		uint32_t a = hash[0];
		uint32_t b = hash[1];
		uint32_t c = hash[2];
		uint32_t d = hash[3];
		uint32_t e = hash[4];
		uint32_t f = hash[5];
		uint32_t g = hash[6];
		uint32_t h = hash[7];

		for (uint32_t t = 0; t <= 63; t++)
		{
			const uint32_t t1 = h + Sum1(e) + Ch(e, f, g) + SHA256_K[t] + W[t];
			const uint32_t t2 = Sum0(a) + Maj(a, b, c);
			h = g;
			g = f;
			f = e;
			e = d + t1;
			d = c;
			c = b;
			b = a;
			a = t1 + t2;
		}

		hash[0] += a;
		hash[1] += b;
		hash[2] += c;
		hash[3] += d;
		hash[4] += e;
		hash[5] += f;
		hash[6] += g;
		hash[7] += h;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void Transform(const void* const mp, const uint64_t numBlks, std::array<uint64_t, 8>& hash)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	for(uint64_t blk = 0; blk < numBlks; blk++)
	{
		std::array<uint64_t, 16> M{};
		for (uint64_t i = 0; i < 128 / 8; i++)
		{
			M[i] = (static_cast<const uint64_t*>(mp)[blk * 16 + i]);
			TRAP::Utils::Memory::SwapBytes<uint64_t>(M[i]);
		}

		std::array<uint64_t, 80> W{};
		for (uint32_t t = 0; t <= 15; t++)
			W[t] = M[t];
		for (uint32_t t = 16; t <= 79; t++)
			W[t] = Sigma1(W[t - 2]) + W[t - 7] + Sigma0(W[t - 15]) + W[t - 16];

		uint64_t a = hash[0];
		uint64_t b = hash[1];
		uint64_t c = hash[2];
		uint64_t d = hash[3];
		uint64_t e = hash[4];
		uint64_t f = hash[5];
		uint64_t g = hash[6];
		uint64_t h = hash[7];

		for(uint32_t t = 0; t <= 79; t++)
		{
			const uint64_t t1 = h + Sum1(e) + Ch(e, f, g) + SHA512_K[t] + W[t];
			const uint64_t t2 = Sum0(a) + Maj(a, b, c);
			h = g;
			g = f;
			f = e;
			e = d + t1;
			d = c;
			c = b;
			b = a;
			a = t1 + t2;
		}

		hash[0] += a;
		hash[1] += b;
		hash[2] += c;
		hash[3] += d;
		hash[4] += e;
		hash[5] += f;
		hash[6] += g;
		hash[7] += h;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<uint8_t, 32> TRAP::Utils::Hash::SHA2_256(const void* const data, uint64_t length)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	std::array<uint32_t, 8> hash =
	{
		0x6a09e667,
		0xbb67ae85,
		0x3c6ef372,
		0xa54ff53a,
		0x510e527f,
		0x9b05688c,
		0x1f83d9ab,
		0x5be0cd19
	};
	std::size_t pos = 0;
	uint64_t total = 0;
	std::array<uint8_t, 64> m{};
	const uint8_t* dataPtr = static_cast<const uint8_t*>(data);

	if(length >= 64)
	{
		const std::size_t blocks = length / 64;
		const std::size_t bytes = blocks * 64;
		Transform(static_cast<const void*>(dataPtr), blocks, hash);
		length -= bytes;
		total += bytes * 8;
		dataPtr += bytes;
	}
	std::copy_n(dataPtr, length, &m[0] + pos);
	pos += length;
	total += length * 8;

	m[pos++] = 0x80;
	if(pos > 56)
	{
		std::fill_n(m.data() + pos, 64 - pos, static_cast<uint8_t>(0u));
		Transform(&m[0], 1, hash);
		pos = 0;
	}
	std::fill_n(m.data() + pos, 56 - pos, static_cast<uint8_t>(0u));
	uint64_t mLength = total;
	TRAP::Utils::Memory::SwapBytes<uint64_t>(mLength);
	std::copy_n(reinterpret_cast<uint8_t*>(&mLength), 64 / 8, &m[0] + (64 - 8));
	Transform(&m[0], 1, hash);
	for(uint32_t i = 0; i < 8; i++)
		TRAP::Utils::Memory::SwapBytes<uint32_t>(hash[i]);

	std::array<uint8_t, 32> result{};
	std::copy_n(reinterpret_cast<const uint8_t*>(hash.data()), result.size(), result.data());

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<uint8_t, 32> TRAP::Utils::Hash::SHA2_256(const std::string_view str)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return SHA2_256(str.data(), str.length());
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<uint8_t, 64> TRAP::Utils::Hash::SHA2_512(const void* const data, uint64_t length)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	std::size_t pos = 0;
	uint64_t total = 0;
	std::array<uint64_t, 8> hash =
	{
		0x6a09e667f3bcc908,
		0xbb67ae8584caa73b,
		0x3c6ef372fe94f82b,
		0xa54ff53a5f1d36f1,
		0x510e527fade682d1,
		0x9b05688c2b3e6c1f,
		0x1f83d9abfb41bd6b,
		0x5be0cd19137e2179
	};
	std::array<uint8_t, 128> m{};
	const uint8_t* dataPtr = static_cast<const uint8_t*>(data);

	if(length >= 128)
	{
		const std::size_t blocks = length / 128;
		const std::size_t bytes = blocks * 128;
		Transform(static_cast<const void*>(dataPtr), blocks, hash);
		length -= bytes;
		total += bytes * 8;
		dataPtr += bytes;
	}
	std::copy_n(dataPtr, length, &m[0] + pos);
	pos += length;
	total += length * 8;

	m[pos++] = 0x80;
	if(pos > 112)
	{
		std::fill_n(m.data() + pos, 128 - pos, static_cast<uint8_t>(0u));
		Transform(&m[0], 1, hash);
		pos = 0;
	}
	std::fill_n(m.data() + pos, 128 - pos, static_cast<uint8_t>(0u));
	uint64_t mLength = total;
	TRAP::Utils::Memory::SwapBytes<uint64_t>(mLength);
	std::copy_n(reinterpret_cast<const uint8_t*>(&mLength), 64 / 8, &m[0] + (128 - 8));
	Transform(&m[0], 1, hash);
	for (uint32_t i = 0; i < 8; i++)
		TRAP::Utils::Memory::SwapBytes<uint64_t>(hash[i]);

	std::array<uint8_t, 64> result{};
	std::copy_n(reinterpret_cast<const uint8_t*>(hash.data()), result.size(), result.data());

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<uint8_t, 64> TRAP::Utils::Hash::SHA2_512(const std::string_view str)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return SHA2_512(str.data(), str.length());
}