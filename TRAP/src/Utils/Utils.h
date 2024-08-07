#ifndef TRAP_UTILS_H
#define TRAP_UTILS_H

#include <array>
#include <string>

#include <fmt/core.h>
#include <unordered_map>

#include "Core/Types.h"
#include "TRAP_Assert.h"

namespace TRAP::Utils
{
	using UUID = std::array<u8, 16>;
	constexpr UUID EMPTY_UUID = UUID{};

	/// @brief Convert a 16 byte long UUID to a string.
	/// @param uuid UUID.
	/// @return String representation of UUID.
	[[nodiscard]] std::string UUIDToString(const TRAP::Utils::UUID& uuid);
	/// @brief Convert a string to a 16 byte long UUID.
	/// @param uuid String representation of a 16 byte long UUID.
	/// @return 16 byte long UUID.
	[[nodiscard]] TRAP::Utils::UUID UUIDFromString(std::string_view uuid);

	//-------------------------------------------------------------------------------------------------------------------//

	template<typename T>
	concept Hashable = requires(T obj) { {std::hash<T>{}(obj) } -> std::convertible_to<usize>; };

	/// @brief Called repeatedly to incrementally create a hash value from several variables.
	/// Based on https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
	/// @param seed Seed.
	/// @param v Value to hash.
	/// @param rest Optional variadic for more values.
	template<typename T, typename... Rest>
	requires (Hashable<T> && ... && Hashable<Rest>)
	constexpr void HashCombine(usize& seed, const T& v, Rest... rest) noexcept
	{
		seed ^= std::hash<T>()(v) + 0x9E3779B9u + (seed << 6u) + (seed >> 2u);
    	((seed ^= std::hash<Rest>()(rest) + 0x9E3779B9u + (seed << 6u) + (seed >> 2u)), ...);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Hasher for string like types with enabled heterogeneous lookup.
	struct StringHasher
	{
		using is_transparent = void; //Enable heterogeneous lookup.

		[[nodiscard]] usize operator()(const std::string_view sv) const
		{
			std::hash<std::string_view> hasher{};
			return hasher(sv);
		}
	};

	template<typename Value>
	using UnorderedStringMap = std::unordered_map<std::string, Value, TRAP::Utils::StringHasher, std::equal_to<>>;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Enum used to describe endianness.
	enum class Endian
	{
		Little = 1,
		Big = 0
	};

	/// @brief Get the endianness of the system.
	/// @return TRAP::Utils::Endian::Little or TRAP::Utils::Endian::Big.
	[[nodiscard]] consteval Endian GetEndian();

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Obtains a view to the object representation of the elements of the span s.
	///        If Extent is std::dynamic_extent, the extent of the returned span S is also std::dynamic_extent;
	///        otherwise it is sizeof(T) * Extent.
	/// @return Span as bytes.
	template<typename T, usize Extent>
	[[nodiscard]] constexpr auto AsBytes(std::span<T, Extent> s) noexcept;

	/// @brief Obtains a view to the object representation of the elements of the span s.
	///        If Extent is std::dynamic_extent, the extent of the returned span S is also std::dynamic_extent;
	///        otherwise it is sizeof(T) * Extent.
	/// @return Span as writable bytes.
	template<typename T, usize Extent>
	requires (!std::is_const_v<T>)
	[[nodiscard]] constexpr auto AsWritableBytes(std::span<T, Extent> s) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	template<typename T>
	struct IsStdSpan : std::false_type
	{};

	template<typename T, usize Extent>
	struct IsStdSpan<std::span<T, Extent>> : std::true_type{};

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief CPUInfo is a struct which contains information about the CPU of the system which is used by TRAP.
	struct CPUInfo
	{
		std::string Model{};
		u32 Cores = 0;
		u32 LogicalCores = 0;
		bool HyperThreaded = false;
	};

	/// @brief Get information about the CPU that runs the engine.
	/// @return Constant reference to the TRAP::CPUInfo.
	[[nodiscard]] const CPUInfo& GetCPUInfo();

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Enum used to indicate which window manager is used by Linux based systems.
	enum class LinuxWindowManager
	{
		Unknown,

		X11,
		Wayland
	};

	/// @brief Get the window manager used by Linux based systems.
	///
	/// On Linux if no known window manager is found then the engine will be terminated.
	/// @return
	/// TRAP::Application::LinuxWindowManager::X11, TRAP::Application::LinuxWindowManager::Wayland or
	/// TRAP::Application::LinuxWindowManager::Unknown(If window manager is unknown or system OS
	/// is not Linux based).
	[[nodiscard]] LinuxWindowManager GetLinuxWindowManager();

	//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_WINDOWS
	/// @brief Replacement for IsWindowsVersionOrGreater, as we cannot rely on the application
	/// having a correct embedded manifest.
	/// @param major Major Windows version.
	/// @param minor Minor Windows version.
	/// @param sp Service pack.
	/// @return Whether Windows version is given version or newer.
	[[nodiscard]] BOOL IsWindowsVersionOrGreaterWin32(const WORD major, const WORD minor, const WORD sp);
	/// @brief Checks whether we are on at least the specified build of Windows 10.
	/// @param build Build number.
	/// @return Whether Windows 10 version is given build or newer.
	[[nodiscard]] BOOL IsWindows10BuildOrGreaterWin32(const WORD build);
	/// @brief Checks whether we are on at least the specified build of Windows 11.
	/// @param build Build number.
	/// @return Whether Windows 11 version is given build or newer.
	[[nodiscard]] BOOL IsWindows11BuildOrGreaterWin32(const WORD build);
	/// @brief Checks whether we are on at least Windows 11 21H2 "Sun Valley" (10.0.22000).
	/// @return Whether Windows 11 version is given build or newer.
	[[nodiscard]] BOOL IsWindows11OrGreaterWin32();

	/// @brief Checks whether we are on at least Windows 10 Anniversary Update ("Redstone 1" / 1607 / 10.0.14393).
	/// @return Whether Windows 10 Anniversary version or newer.
	[[nodiscard]] BOOL IsWindows10Version1607OrGreaterWin32();
	/// @brief Checks whether we are on at least Windows 10 Creators Update ("Redstone 2" / 1703 / 10.0.15063).
	/// @return Whether Window 10 Creators version or newer.
	[[nodiscard]] BOOL IsWindows10Version1703OrGreaterWin32();
	/// @brief Checks whether we are on at least Windows 10 Threshold 1 (1507 / 10.0.10240).
	/// @return Whether Windows 10 version is given build or newer.
	[[nodiscard]] BOOL IsWindows10OrGreaterWin32();
#endif /*TRAP_PLATFORM_WINDOWS*/

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Checks if another instance of the engine is already running.
	/// If another instance is already running then this instance will be closed with an error code.
	void CheckSingleProcess();

	//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
	/// @brief Register the SIGINT callback function.
	///        Used in Headless mode to handle CTRL+C.
	/// @remark This function is only available in headless mode.
	void RegisterSIGINTCallback();
#endif /*TRAP_HEADLESS_MODE*/

	//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRACY_ENABLE
	/// @brief Set the name of the current thread.
	/// @param name Name to set
	/// @note Only used when compiling with profiling configuration, no-op otherwise.
    void SetThreadName(const std::string_view name);
#else
	/// @brief Set the name of the current thread.
	/// @param name Name to set
	/// @note Only used when compiling with profiling configuration, no-op otherwise.
    constexpr void SetThreadName([[maybe_unused]] const std::string_view name)
    {}
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline consteval TRAP::Utils::Endian TRAP::Utils::GetEndian()
{
	//Check if machine is using little-endian or big-endian
	return static_cast<Endian>(std::endian::native == std::endian::little);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T, usize Extent>
[[nodiscard]] constexpr auto TRAP::Utils::AsBytes(const std::span<T, Extent> s) noexcept
{
	if constexpr(Extent != std::dynamic_extent)
	{
		return std::span<const u8, Extent * sizeof(T)>{reinterpret_cast<const u8*>(s.data()), s.size_bytes()};
	}
	else
	{
		return std::span<const u8>{reinterpret_cast<const u8*>(s.data()), s.size_bytes()};
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T, usize Extent>
requires (!std::is_const_v<T>)
[[nodiscard]] constexpr auto TRAP::Utils::AsWritableBytes(const std::span<T, Extent> s) noexcept
{
	if constexpr(Extent != std::dynamic_extent)
	{
		return std::span<u8, Extent * sizeof(T)>{reinterpret_cast<u8*>(s.data()), s.size_bytes()};
	}
	else
	{
		return std::span<u8>{reinterpret_cast<u8*>(s.data()), s.size_bytes()};
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Utils::LinuxWindowManager>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Utils::LinuxWindowManager linuxWM,
	                                            fmt::format_context& ctx)
    {
        std::string enumStr{};
        switch(linuxWM)
        {
        case TRAP::Utils::LinuxWindowManager::Unknown:
            enumStr = "Off";
            break;
        case TRAP::Utils::LinuxWindowManager::X11:
            enumStr = "X11";
            break;
        case TRAP::Utils::LinuxWindowManager::Wayland:
            enumStr = "Wayland";
            break;

        default:
            TRAP_ASSERT(false, "fmt::formatter<TRAP::Utils::LinuxWindowManager>: Missing enum value!");
            enumStr = "<MISSING ENUM VALUE>";
            break;
        }

        return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

#endif /*TRAP_UTILS_H*/
