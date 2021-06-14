#ifndef _TRAP_UTILS_H_
#define _TRAP_UTILS_H_

namespace TRAP::Utils
{
	/// <summary>
	/// Convert a 16 byte long UUID to a string.
	/// </summary>
	/// <param name="uuid">UUID.</param>
	/// <returns>String representation of UUID.</returns>
	std::string UUIDToString(const std::array<uint8_t, 16>& uuid);
	/// <summary>
	/// Convert a string to a 16 byte long UUID.
	/// </summary>
	/// <param name="uuid">String representation of a 16 byte long UUID.</param>
	/// <returns>16 byte long UUID.</returns>
	std::array<uint8_t, 16> UUIDFromString(std::string_view uuid);

	inline void HashCombine(std::size_t&)
	{}

	template<typename T, typename... Rest>
	void HashCombine(std::size_t& seed, const T& v, Rest... rest)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		HashCombine(seed, rest...);
	}
}

#endif /*_TRAP_UTILS_H_*/