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
	[[nodiscard]] std::string UUIDToString(const std::array<uint8_t, 16>& uuid);
	/// <summary>
	/// Convert a string to a 16 byte long UUID.
	/// </summary>
	/// <param name="uuid">String representation of a 16 byte long UUID.</param>
	/// <returns>16 byte long UUID.</returns>
	[[nodiscard]] std::array<uint8_t, 16> UUIDFromString(std::string_view uuid);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Called repeatedly to incrementally create a hash value from several variables.
	/// Based on https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
	/// </summary>
	/// <param name="seed">Seed.</param>
	/// <param name="v">Value to hash.</param>
	/// <param name="rest">Optional variadic for more values.</param>
	template<typename T, typename... Rest>
	constexpr void HashCombine(std::size_t& seed, const T& v, Rest... rest) noexcept
	{
		seed ^= std::hash<T>()(v) + 0x9E3779B9u + (seed << 6u) + (seed >> 2u);
    	((seed ^= std::hash<Rest>()(rest) + 0x9E3779B9u + (seed << 6u) + (seed >> 2u)), ...);
	}

	//-------------------------------------------------------------------------------------------------------------------//

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
	[[nodiscard]] Endian GetEndian();

	//-------------------------------------------------------------------------------------------------------------------//

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
	[[nodiscard]] const CPUInfo& GetCPUInfo();

	//-------------------------------------------------------------------------------------------------------------------//

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
	///
	/// On Linux if no known window manager is found then the engine will be terminated.
	/// </summary>
	/// <returns>
	/// TRAP::Application::LinuxWindowManager::X11, TRAP::Application::LinuxWindowManager::Wayland or
	/// TRAP::Application::LinuxWindowManager::Unknown(If window manager is unknown or system OS
	/// is not Linux based).
	/// </returns>
	LinuxWindowManager GetLinuxWindowManager();

	//-------------------------------------------------------------------------------------------------------------------//

#ifdef __cpp_lib_bit_cast
	using BitCast = std::bit_cast;
#else
	template<typename To, typename From>
	[[nodiscard]] inline To BitCast(const From& from) noexcept
	{
		static_assert(sizeof(From) == sizeof(To));
		static_assert(std::is_trivially_copyable_v<From>);
		static_assert(std::is_trivially_copyable_v<To>);

		union U
		{
			U() = default;
			char storage[sizeof(To)]{};
			typename std::remove_const<To>::type dest;
		} u; //Instead of To dest; because To doesn't require DefaultConstructible.
		memcpy(&u.dest, &from, sizeof(from));
		return u.dest;
	}
#endif

	//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_WINDOWS
	using PFN_RtlVerifyVersionInfo = LONG(WINAPI*)(OSVERSIONINFOEXW*, ULONG, ULONGLONG);
	struct NTDLL
	{
		HINSTANCE Instance = nullptr;
		PFN_RtlVerifyVersionInfo RtlVerifyVersionInfo = nullptr;
	};

	/// <summary>
	/// Replacement for IsWindowsVersionOrGreater, as we cannot rely on the application
	/// having a correct embedded manifest.
	/// </summary>
	/// <param name="major">Major Windows version.</param>
	/// <param name="minor">Minor Windows version.</param>
	/// <param name="sp">Service pack.</param>
	/// <returns>Whether Windows version is given version or newer.</returns>
	[[nodiscard]] BOOL IsWindowsVersionOrGreaterWin32(const WORD major, const WORD minor, const WORD sp);
	/// <summary>
	/// Checks whether we are on at least the specified build of Windows 10.
	/// </summary>
	/// <param name="build">Build number.</param>
	/// <returns>Whether Windows 10 version is given build or newer.</returns>
	[[nodiscard]] BOOL IsWindows10BuildOrGreaterWin32(const WORD build);
	/// <summary>
	/// Checks whether we are on at least the specified build of Windows 11.
	/// </summary>
	/// <param name="build">Build number.</param>
	/// <returns>Whether Windows 11 version is given build or newer.</returns>
	[[nodiscard]] BOOL IsWindows11BuildOrGreaterWin32(const WORD build);
	/// <summary>
	/// Checks whether we are on at least Windows 10 Anniversary Update.
	/// </summary>
	/// <returns>Whether Windows 10 Anniversary version or newer.</returns>
	[[nodiscard]] BOOL IsWindows10Version1607OrGreaterWin32();
	/// <summary>
	/// Checks whether we are on at least Windows 10 Creators Update.
	/// </summary>
	/// <returns>Whether Window 10 Creators version or newer.</returns>
	[[nodiscard]] BOOL IsWindows10Version1703OrGreaterWin32();
	/// <summary>
	/// Checks whether we are on at least Windows 8.1.
	/// </summary>
	/// <returns>Whether Windows 8.1 version or newer.</returns>
	[[nodiscard]] BOOL IsWindows8Point1OrGreaterWin32();
	/// <summary>
	/// Checks whether we are on at least Windows 8.
	/// </summary>
	/// <returns>Whether Windows 8 version or newer.</returns>
	[[nodiscard]] BOOL IsWindows8OrGreaterWin32();
	/// <summary>
	/// Checks whether we are on at least Windows 7.
	/// </summary>
	/// <returns>Whether Windows 7 version or newer.</returns>
	[[nodiscard]] BOOL IsWindows7OrGreaterWin32();
#endif

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Checks if another instance of the engine is already running.
	/// If another instance is already running then this instance will be closed with an error code.
	/// </summary>
	void CheckSingleProcess();

	//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
	/// <summary>
	/// Register the SIGINT callback function.
	/// Used in Headless mode to handle CTRL+C.
	/// </summary>
	void RegisterSIGINTCallback();
#endif /*TRAP_HEADLESS_MODE*/
}

#endif /*TRAP_UTILS_H*/