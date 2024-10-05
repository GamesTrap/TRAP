#include "TRAPPCH.h"
#include "SHA-2.h"

#include "Utils/Memory.h"

namespace
{
	constexpr std::array<u32, 64u> SHA256_K =
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

	constexpr std::array<u64, 80u> SHA512_K =
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

	[[nodiscard]] constexpr u32 Rotr(const u32 x, const u32 n) noexcept
	{
		return (x >> n) | (x << (32u - n));
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u64 Rotr(const u64 x, const u32 n) noexcept
	{
		return (x >> n) | (x << (64u - n));
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u32 Shr(const u32 x, const u32 n) noexcept
	{
		return x >> n;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u64 Shr(const u64 x, const u32 n) noexcept
	{
		return x >> n;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u32 Ch(const u32 x, const u32 y, const u32 z) noexcept
	{
		return (x & y) ^ (~x & z);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u64 Ch(const u64 x, const u64 y, const u64 z) noexcept
	{
		return (x & y) ^ (~x & z);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u32 Maj(const u32 x, const u32 y, const u32 z) noexcept
	{
		return (x & y) ^ (x & z) ^ (y & z);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u64 Maj(const u64 x, const u64 y, const u64 z) noexcept
	{
		return (x & y) ^ (x & z) ^ (y & z);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u32 Sum0(const u32 x) noexcept
	{
		return Rotr(x, 2u) ^ Rotr(x, 13u) ^ Rotr(x, 22u);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u64 Sum0(const u64 x) noexcept
	{
		return Rotr(x, 28u) ^ Rotr(x, 34u) ^ Rotr(x, 39u);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u32 Sum1(const u32 x) noexcept
	{
		return Rotr(x, 6u) ^ Rotr(x, 11u) ^ Rotr(x, 25u);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u64 Sum1(const u64 x) noexcept
	{
		return Rotr(x, 14u) ^ Rotr(x, 18u) ^ Rotr(x, 41u);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u32 Sigma0(const u32 x) noexcept
	{
		return Rotr(x, 7u) ^ Rotr(x, 18u) ^ Shr(x, 3u);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u64 Sigma0(const u64 x) noexcept
	{
		return Rotr(x, 1u) ^ Rotr(x, 8u) ^ Shr(x, 7u);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u32 Sigma1(const u32 x) noexcept
	{
		return Rotr(x, 17u) ^ Rotr(x, 19u) ^ Shr(x, 10u);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u64 Sigma1(const u64 x) noexcept
	{
		return Rotr(x, 19u) ^ Rotr(x, 61u) ^ Shr(x, 6u);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void Transform(const void* const mp, const u64 numBlks, std::array<u32, 8u>& hash)
	{
		ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

		for(u64 blk = 0u; blk < numBlks; ++blk)
		{
			std::array<u32, 16u> M{};
			for (u32 i = 0u; i < 64u / 4u; ++i)
			{
				u32 tmp = static_cast<const u32*>(mp)[(blk * 16u) + i];
				TRAP::Utils::Memory::SwapBytes<u32>(tmp);
				M[i] = tmp;
			}

			std::array<u32, 64u> W{};
			for (u32 t = 0u; t <= 15u; ++t)
				W[t] = M[t];
			for (u32 t = 16u; t <= 63u; ++t)
				W[t] = Sigma1(W[t - 2u]) + W[t - 7u] + Sigma0(W[t - 15u]) + W[t - 16u];

			u32 a = std::get<0u>(hash);
			u32 b = std::get<1u>(hash);
			u32 c = std::get<2u>(hash);
			u32 d = std::get<3u>(hash);
			u32 e = std::get<4u>(hash);
			u32 f = std::get<5u>(hash);
			u32 g = std::get<6u>(hash);
			u32 h = std::get<7u>(hash);

			for (u32 t = 0u; t <= 63u; ++t)
			{
				const u32 t1 = h + Sum1(e) + Ch(e, f, g) + SHA256_K[t] + W[t];
				const u32 t2 = Sum0(a) + Maj(a, b, c);
				h = g;
				g = f;
				f = e;
				e = d + t1;
				d = c;
				c = b;
				b = a;
				a = t1 + t2;
			}

			std::get<0u>(hash) += a;
			std::get<1u>(hash) += b;
			std::get<2u>(hash) += c;
			std::get<3u>(hash) += d;
			std::get<4u>(hash) += e;
			std::get<5u>(hash) += f;
			std::get<6u>(hash) += g;
			std::get<7u>(hash) += h;
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void Transform(const void* const mp, const u64 numBlks, std::array<u64, 8u>& hash)
	{
		ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

		for(u64 blk = 0u; blk < numBlks; ++blk)
		{
			std::array<u64, 16u> M{};
			for (u64 i = 0u; i < 128u / 8u; ++i)
			{
				M[i] = (static_cast<const u64*>(mp)[(blk * 16u) + i]);
				TRAP::Utils::Memory::SwapBytes<u64>(M[i]);
			}

			std::array<u64, 80u> W{};
			for (u32 t = 0u; t <= 15u; ++t)
				W[t] = M[t];
			for (u32 t = 16u; t <= 79u; ++t)
				W[t] = Sigma1(W[t - 2u]) + W[t - 7u] + Sigma0(W[t - 15u]) + W[t - 16u];

			u64 a = std::get<0u>(hash);
			u64 b = std::get<1u>(hash);
			u64 c = std::get<2u>(hash);
			u64 d = std::get<3u>(hash);
			u64 e = std::get<4u>(hash);
			u64 f = std::get<5u>(hash);
			u64 g = std::get<6u>(hash);
			u64 h = std::get<7u>(hash);

			for(u32 t = 0u; t <= 79u; ++t)
			{
				const u64 t1 = h + Sum1(e) + Ch(e, f, g) + SHA512_K[t] + W[t];
				const u64 t2 = Sum0(a) + Maj(a, b, c);
				h = g;
				g = f;
				f = e;
				e = d + t1;
				d = c;
				c = b;
				b = a;
				a = t1 + t2;
			}

			std::get<0u>(hash) += a;
			std::get<1u>(hash) += b;
			std::get<2u>(hash) += c;
			std::get<3u>(hash) += d;
			std::get<4u>(hash) += e;
			std::get<5u>(hash) += f;
			std::get<6u>(hash) += g;
			std::get<7u>(hash) += h;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<u8, 32u> TRAP::Utils::Hash::SHA2_256(const void* const data, u64 length)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	std::array<u32, 8u> hash =
	{
		0x6a09e667u,
		0xbb67ae85u,
		0x3c6ef372u,
		0xa54ff53au,
		0x510e527fu,
		0x9b05688cu,
		0x1f83d9abu,
		0x5be0cd19u
	};
	usize pos = 0u;
	u64 total = 0u;
	std::array<u8, 64u> m{};
	const u8* dataPtr = static_cast<const u8*>(data);

	if(length >= 64u)
	{
		const usize blocks = length / 64u;
		const usize bytes = blocks * 64u;
		Transform(dataPtr, blocks, hash);
		length -= bytes;
		total += bytes * 8u;
		dataPtr += bytes;
	}
	std::copy_n(dataPtr, length, &std::get<0u>(m) + pos);
	pos += length;
	total += length * 8u;

	m[pos++] = 0x80u;
	if(pos > 56u)
	{
		std::fill_n(m.data() + pos, 64u - pos, NumericCast<u8>(0u));
		Transform(&std::get<0u>(m), 1u, hash);
		pos = 0u;
	}
	std::fill_n(m.data() + pos, 56u - pos, NumericCast<u8>(0u));
	u64 mLength = total;
	TRAP::Utils::Memory::SwapBytes<u64>(mLength);
	std::copy_n(reinterpret_cast<u8*>(&mLength), 64u / 8u, &std::get<0u>(m) + (64u - 8u));
	Transform(&std::get<0u>(m), 1u, hash);
	for(u32 i = 0u; i < 8u; i++)
		TRAP::Utils::Memory::SwapBytes<u32>(hash[i]);

	std::array<u8, 32u> result{};
	std::copy_n(reinterpret_cast<const u8*>(hash.data()), result.size(), result.data());

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<u8, 32u> TRAP::Utils::Hash::SHA2_256(const std::string_view str)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return SHA2_256(str.data(), str.length());
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<u8, 64u> TRAP::Utils::Hash::SHA2_512(const void* const data, u64 length)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	usize pos = 0u;
	u64 total = 0u;
	std::array<u64, 8u> hash =
	{
		0x6a09e667f3bcc908u,
		0xbb67ae8584caa73bu,
		0x3c6ef372fe94f82bu,
		0xa54ff53a5f1d36f1u,
		0x510e527fade682d1u,
		0x9b05688c2b3e6c1fu,
		0x1f83d9abfb41bd6bu,
		0x5be0cd19137e2179u
	};
	std::array<u8, 128u> m{};
	const u8* dataPtr = static_cast<const u8*>(data);

	if(length >= 128u)
	{
		const usize blocks = length / 128u;
		const usize bytes = blocks * 128u;
		Transform(dataPtr, blocks, hash);
		length -= bytes;
		total += bytes * 8u;
		dataPtr += bytes;
	}
	std::copy_n(dataPtr, length, &std::get<0u>(m) + pos);
	pos += length;
	total += length * 8u;

	m[pos++] = 0x80u;
	if(pos > 112u)
	{
		std::fill_n(m.data() + pos, 128u - pos, NumericCast<u8>(0u));
		Transform(&std::get<0u>(m), 1u, hash);
		pos = 0u;
	}
	std::fill_n(m.data() + pos, 128u - pos, NumericCast<u8>(0u));
	u64 mLength = total;
	TRAP::Utils::Memory::SwapBytes<u64>(mLength);
	std::copy_n(reinterpret_cast<const u8*>(&mLength), 64u / 8u, &std::get<0u>(m) + (128u - 8u));
	Transform(&std::get<0u>(m), 1u, hash);
	for (u32 i = 0u; i < 8u; i++)
		TRAP::Utils::Memory::SwapBytes<u64>(hash[i]);

	std::array<u8, 64u> result{};
	std::copy_n(reinterpret_cast<const u8*>(hash.data()), result.size(), result.data());

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<u8, 64u> TRAP::Utils::Hash::SHA2_512(const std::string_view str)
{
    ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return SHA2_512(str.data(), str.length());
}
