#ifndef TRAP_UTILS_H
#define TRAP_UTILS_H

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

	//TODO Document
	inline void HashCombine(std::size_t&)
	{}

	//TODO Document
	template<typename T, typename... Rest>
	void HashCombine(std::size_t& seed, const T& v, Rest... rest)
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
}

#endif /*TRAP_UTILS_H*/