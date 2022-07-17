#ifndef TRAP_UTILS_H
#define TRAP_UTILS_H

#include <array>
#include <string>
#include <type_traits>

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

	inline constexpr void HashCombine(std::size_t&)
	{}

	/// <summary>
	/// Called repeatedly to incrementally create a hash value from several variables.
	/// Based on https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
	/// </summary>
	/// <param name="seed">Seed.</param>
	/// <param name="v">Value to hash.</param>
	/// <param name="rest">Optional variadic for more values.</param>
	template<typename T, typename... Rest>
	constexpr void HashCombine(std::size_t& seed, const T& v, Rest... rest)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9E3779B9 + (seed << 6) + (seed >> 2);
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
	/// CPUInfo is a struct which contains information about the CPU of the system which is used by TRAP.
	/// </summary>
	struct CPUInfo
	{
		std::string Model{};
		uint32_t Cores = 0;
		uint32_t LogicalCores = 0;
		bool HyperThreaded = false;
	};

	/// <summary>
	/// Get information about the CPU that runs the engine.
	/// </summary>
	/// <returns>Constant reference to the TRAP::CPUInfo.</returns>
	const CPUInfo& GetCPUInfo();

	/// <summary>
	/// Enum used to indicate which window manager is used by Linux based systems.
	/// </summary>
	enum class LinuxWindowManager
	{
		Unknown,

		X11,
		Wayland
	};

	/// <summary>
	/// Get the window manager used by Linux based systems.
	/// </summary>
	/// <returns>
	/// TRAP::Application::LinuxWindowManager::X11, TRAP::Application::LinuxWindowManager::Wayland or
	/// TRAP::Application::LinuxWindowManager::Unknown(If window manager is unknown or system OS
	/// is not Linux based Windows).
	/// </returns>
	LinuxWindowManager GetLinuxWindowManager();

	/// <summary>
	/// Get a string representation of the Linux window manager.
	/// </summary>
	/// <param name="lwm">LinuxWindowManager to convert to a string.</param>
	/// <returns>String representation.</returns>
	std::string LinuxWindowManagerToString(LinuxWindowManager lwm);

#ifdef TRAP_PLATFORM_LINUX
	/// <summary>
	/// Retrieve the errno string in a thread safe way.
	/// </summary>
	/// <returns>errno string.</returns>
	std::string GetStrError();
#endif

#ifdef __cpp_lib_bit_cast
	using BitCast = std::bit_cast;
#else
	template<typename From, typename To>
	inline typename std::enable_if<std::integral_constant<bool, (sizeof(From) == sizeof(To)) &&
	                                                            std::is_trivially_copyable<From>::value &&
																std::is_trivially_copyable<To>::value>::value, To>::type
	BitCast(const From& from) noexcept
	{
		union U{U(){}; char storage[sizeof(To)]; typename std::remove_const<To>::type dest;} u; //Instead of To dest; because To doesn't require DefaultConstructible.
		memcpy(&u.dest, &from, sizeof(from));
		return u.dest;
	}
#endif
}

#endif /*TRAP_UTILS_H*/