#ifndef TRAP_IPV6ADDRESS_H
#define TRAP_IPV6ADDRESS_H

#include <array>
#include <string>

#include "Utils/Time/TimeStep.h"

namespace TRAP::Network
{
	/// <summary>
	/// TRAP::Network::IPv6Address is a utility class for manipulating network addresses.
	/// It provides a set of implicit constructors and conversion functions to easily build or transform
	/// an IPv6 address from/to various representations.
	/// </summary>
	class IPv6Address
	{
	public:
		/// <summary>
		/// This constructor creates an empty (invalid) address.
		/// </summary>
		IPv6Address() noexcept = default;

		/// <summary>
		/// Construct the address from a string.
		///
		/// Here address can be either a hex address (ex: "2001:0db8:85a3:0000:0000:8a2e:0370:7334") or a
		/// network name (ex: "localhost").
		/// </summary>
		/// <param name="address">IPv6 address or network name.</param>
		explicit IPv6Address(const std::string& address);

		/// <summary>
		/// Construct the address from a string.
		///
		/// Here address can either be a hex address (ex: "2001:0db8:85a3:0000:0000:8a2e:0370:7334") or a
		/// network name (ex: "localhost").
		/// This is equivalent to the constructor taking a std::string
		/// parameter, it is defined for convenience so that the
		/// implicit conversion from literal strings to IPv6Address work.
		/// </summary>
		/// <param name="address">IPv6 address or network name.</param>
		explicit IPv6Address(const char* address);

		/// <summary>
		/// Construct the address from 16 bytes array.
		/// </summary>
		/// <param name="addressBytes">Array containing 16 bytes address.</param>
		explicit constexpr IPv6Address(const std::array<uint8_t, 16>& addressBytes) noexcept;

		/// <summary>
		/// Get a string representation of the address.
		///
		/// The returned string is the hex representation of the
		/// IPv6 address (like "2001:0db8:85a3:0000:0000:8a2e:0370:7334"), even if it was constructed
		/// from a host name.
		/// </summary>
		/// <returns>String representation of the address.</returns>
		[[nodiscard]] std::string ToString() const;

		/// <summary>
		/// Get a byte representation of the address.
		///
		/// The returned array is the byte representation of the
		/// IPv6 address, event if it was constructed
		/// from a host name.
		/// </summary>
		/// <returns>Byte representation of the address.</returns>
		[[nodiscard]] std::array<uint8_t, 16> ToArray() const noexcept;

		/// <summary>
		/// Get the computer's local address.
		///
		/// The local address is the address of the computer from the
		/// LAN point of view, i.e. something like 2001:0db8:85a3:0000:0000:8a2e:0370:7334.
		/// It is meaningful only for communications over the local network.
		/// Unlike GetPublicAddress, this function is fast and may be used safely anywhere.
		/// </summary>
		/// <returns>Local IPv6 address of the computer.</returns>
		[[nodiscard]] static IPv6Address GetLocalAddress();

		/// <summary>
		/// Get the computer's public address.
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
		/// </summary>
		/// <param name="timeout">Maximum time to wait.</param>
		/// <returns>Public IP address of the computer.</returns>
		[[nodiscard]] static IPv6Address GetPublicAddress(Utils::TimeStep timeout = Utils::TimeStep(0.0f));

		static const IPv6Address None; //Value representing an empty/invaid address
		static const IPv6Address Any; //Value representing any address (0000:0000:0000:0000:0000:0000:0000:0000)
		static const IPv6Address LocalHost; //The "localhost" address (for connecting a computer to itself locally)

	private:
		friend bool operator<(const IPv6Address& left, const IPv6Address& right);

		/// <summary>
		/// Resolve the given address string.
		/// </summary>
		/// <param name="address">Address string.</param>
		void Resolve(const std::string& address);

		std::array<uint8_t, 16> m_address; //Address stored as an 128 bit array
		bool m_valid; //Is the address valid?
	};

	/// <summary>
	/// Overload of == operator to compare two IP addresses.
	/// </summary>
	/// <param name="left">Left operand (a IP address).</param>
	/// <param name="right">Right operand (a IP address).</param>
	/// <returns>True if both addresses are equal.</returns>
	bool operator==(const IPv6Address& left, const IPv6Address& right);

	/// <summary>
	/// Overload of != operator to compare two IP addresses.
	/// </summary>
	/// <param name="left">Left operand (a IP address).</param>
	/// <param name="right">Right operand (a IP address).</param>
	/// <returns>True if both addresses are not equal.</returns>
	bool operator!=(const IPv6Address& left, const IPv6Address& right);

	/// <summary>
	/// Overload of < operator to compare two IP addresses.
	/// </summary>
	/// <param name="left">Left operand (a IP address).</param>
	/// <param name="right">Right operand (a IP address).</param>
	/// <returns>True if left is lesser than right.</returns>
	bool operator<(const IPv6Address& left, const IPv6Address& right);

	/// <summary>
	/// Overload of > operator to compare two IP addresses.
	/// </summary>
	/// <param name="left">Left operand (a IP address).</param>
	/// <param name="right">Right operand (a IP address).</param>
	/// <returns>True if left is greater than right.</returns>
	bool operator>(const IPv6Address& left, const IPv6Address& right);

	/// <summary>
	/// Overload of <= operator to compare two IP addresses.
	/// </summary>
	/// <param name="left">Left operand (a IP address).</param>
	/// <param name="right">Right operand (a IP address).</param>
	/// <returns>True if left is lesser or equal than right.</returns>
	bool operator<=(const IPv6Address& left, const IPv6Address& right);

	/// <summary>
	/// Overload of >= operator to compare two IP addresses.
	/// </summary>
	/// <param name="left">Left operand (a IP address).</param>
	/// <param name="right">Right operand (a IP address).</param>
	/// <returns>True if left is greater or equal than right.</returns>
	bool operator>=(const IPv6Address& left, const IPv6Address& right);

	/// <summary>
	/// Overload of >> operator to extract an IP address from input stream.
	/// </summary>
	/// <param name="stream">Input stream.</param>
	/// <param name="address">IP address to extract.</param>
	/// <returns>Reference to the input stream.</returns>
	std::istream& operator>>(std::istream& stream, IPv6Address& address);

	/// <summary>
	/// Overload of << operator to print an IP address to an output stream.
	/// </summary>
	/// <param name="stream">Output stream.</param>
	/// <param name="address">IP address to print.</param>
	/// <returns>Reference to the output stream.</returns>
	std::ostream& operator<<(std::ostream& stream, const IPv6Address& address);
}

//-------------------------------------------------------------------------------------------------------------------//

inline constexpr TRAP::Network::IPv6Address::IPv6Address(const std::array<uint8_t, 16>& addressBytes) noexcept
	: m_address(addressBytes), m_valid(true)
{
}

#endif /*TRAP_IPV6ADDRESS_H*/