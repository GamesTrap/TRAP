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

	/// <summary>
	/// Enum used to describe endianness.
	/// </summary>
	enum class Endian
	{
		Little = 1,
		Big = 0
	};

	/// <summary>
	/// Get the endianness of the system.
	/// </summary>
	/// <returns>TRAP::Utils::Endian::Little or TRAP::Utils::Endian::Big.</returns>
	Endian GetEndian();

	/// <summary>
	/// CPUInfo is a struct which contains information about the CPU of the system which is used by TRAP Engine.
	/// </summary>
	struct CPUInfo
	{
		std::string Model{};
		uint32_t Cores = 0;
		uint32_t LogicalCores = 0;
		bool HyperThreaded = false;
	};

	/// <summary>
	/// Get information about the CPU that runs the Engine.
	/// </summary>
	/// <returns>Constant reference to the TRAP::CPUInfo.</returns>
	const CPUInfo& GetCPUInfo();
}

#endif /*_TRAP_UTILS_H_*/