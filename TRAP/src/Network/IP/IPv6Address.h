#ifndef TRAP_IPV6ADDRESS_H
#define TRAP_IPV6ADDRESS_H

#include <cstdint>
#include <array>
#include <string>

#include <fmt/ostream.h>

#include "Utils/Time/TimeStep.h"

namespace TRAP::Network
{
	/// @brief TRAP::Network::IPv6Address is a utility class for manipulating network addresses.
	/// It provides a set of implicit constructors and conversion functions to easily build or transform
	/// an IPv6 address from/to various representations.
	class IPv6Address
	{
	public:
		/// @brief This constructor creates an empty (invalid) address.
		constexpr IPv6Address() noexcept = default;

		/// @brief Construct the address from a string.
		///
		/// Here address can be either a hex address (ex: "2001:0db8:85a3:0000:0000:8a2e:0370:7334") or a
		/// network name (ex: "localhost").
		/// @param address IPv6 address or network name.
		explicit IPv6Address(const std::string& address);

		/// @brief Construct the address from a string.
		///
		/// Here address can either be a hex address (ex: "2001:0db8:85a3:0000:0000:8a2e:0370:7334") or a
		/// network name (ex: "localhost").
		/// This is equivalent to the constructor taking a std::string
		/// parameter, it is defined for convenience so that the
		/// implicit conversion from literal strings to IPv6Address work.
		/// @param address IPv6 address or network name.
		explicit IPv6Address(const char* address);

		/// @brief Construct the address from 16 bytes array.
		/// @param addressBytes Array containing 16 bytes address.
		explicit constexpr IPv6Address(const std::array<u8, 16>& addressBytes) noexcept;

		/// @brief Copy constructor.
		constexpr IPv6Address(const IPv6Address&) = default;
		/// @brief Copy assignment operator.
		constexpr IPv6Address& operator=(const IPv6Address&) = default;
		/// @brief Move constructor.
		constexpr IPv6Address(IPv6Address&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr IPv6Address& operator=(IPv6Address&&) noexcept = default;

		/// @brief Destructor.
		constexpr ~IPv6Address() = default;

		/// @brief Get a string representation of the address.
		///
		/// The returned string is the hex representation of the
		/// IPv6 address (like "2001:0db8:85a3:0000:0000:8a2e:0370:7334"), even if it was constructed
		/// from a host name.
		/// @return String representation of the address.
		[[nodiscard]] std::string ToString() const;

		/// @brief Get a byte representation of the address.
		///
		/// The returned array is the byte representation of the
		/// IPv6 address, event if it was constructed
		/// from a host name.
		/// @return Byte representation of the address.
		[[nodiscard]] constexpr std::array<u8, 16u> ToArray() const noexcept;

		/// @brief Get the computer's local address.
		///
		/// The local address is the address of the computer from the
		/// LAN point of view, i.e. something like 2001:0db8:85a3:0000:0000:8a2e:0370:7334.
		/// It is meaningful only for communications over the local network.
		/// Unlike GetPublicAddress, this function is fast and may be used safely anywhere.
		/// @return Local IPv6 address of the computer.
		[[nodiscard]] static IPv6Address GetLocalAddress();

		/// @brief Get the computer's public address.
		///
		/// The public address is the address of the computer from the
		/// internet point of view, i.e. something like 2001:0db8:85a3:0000:0000:8a2e:0370:7334.
		/// It is necessary for communications over the world wide web.
		/// The only way to get a public address is to ask it to a
		/// distant website; as a consequence, this function depends on
		/// both your network connection and the server, and may be very slow.
		/// You should use it as few as possible.
		/// Because this function depends on the network connection and on a distant
		/// sever, you may use a time limit if you don't want your program
		/// to be possibly stuck waiting in case there is a problem; this
		/// limit is deactivated by default.
		/// @param timeout Maximum time to wait.
		/// @return Public IP address of the computer.
		[[nodiscard]] static IPv6Address GetPublicAddress(Utils::TimeStep timeout = Utils::TimeStep(0.0f));

		static const IPv6Address None; //Value representing an empty/invaid address
		static const IPv6Address Any; //Value representing any address (0000:0000:0000:0000:0000:0000:0000:0000)
		static const IPv6Address LocalHost; //The "localhost" address (for connecting a computer to itself locally)

		[[nodiscard]] friend constexpr auto operator<=>(const IPv6Address& lhs, const IPv6Address& rhs)
		{
			return lhs.m_address <=> rhs.m_address;
		};

		[[nodiscard]] friend constexpr bool operator==(const IPv6Address& lhs, const IPv6Address& rhs)
		{
			return lhs.m_address == rhs.m_address;
		}

	private:
		/// @brief Resolve the given address string.
		/// @param address Address string.
		void Resolve(const std::string& address);

		std::array<u8, 16> m_address{}; //Address stored as an 128 bit array
		bool m_valid = false; //Is the address valid?
	};

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Overload of >> operator to extract an IP address from input stream.
	/// @param stream Input stream.
	/// @param address IP address to extract.
	/// @return Reference to the input stream.
	std::istream& operator>>(std::istream& stream, TRAP::Network::IPv6Address& address);
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::IPv6Address::IPv6Address(const std::array<u8, 16>& addressBytes) noexcept
	: m_address(addressBytes), m_valid(true)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::array<u8, 16> TRAP::Network::IPv6Address::ToArray() const noexcept
{
	return m_address;
}

//-------------------------------------------------------------------------------------------------------------------//

inline constexpr TRAP::Network::IPv6Address TRAP::Network::IPv6Address::None{};
inline constexpr TRAP::Network::IPv6Address TRAP::Network::IPv6Address::Any(std::array<u8, 16u>
	{
		0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
	});
inline constexpr TRAP::Network::IPv6Address TRAP::Network::IPv6Address::LocalHost(std::array<u8, 16u>
    {
		0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u
	});

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Network::IPv6Address>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Network::IPv6Address& ip,
	                                            fmt::format_context& ctx)
    {
		return fmt::format_to(ctx.out(), "{}", ip.ToString());
    }
};

#endif /*TRAP_IPV6ADDRESS_H*/
