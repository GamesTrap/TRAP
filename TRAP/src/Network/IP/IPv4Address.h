/*
////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2019 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

Modified by: Jan "GamesTrap" Schuerkamp
*/

#ifndef _TRAP_IPV4ADDRESS_H_
#define _TRAP_IPV4ADDRESS_H_

#include "Utils/Time/TimeStep.h"

namespace TRAP::Network
{
	/// <summary>
	/// TRAP::Network::IPv4Address is a utility class for manipulating network addresses.<br>
	/// It provides a set of implicit constructors and conversion functions to easily build or transform
	/// an IPv4 address from/to various representations.
	/// </summary>
	class IPv4Address
	{
	public:
		/// <summary>
		/// This constructor creates an empty (invalid) address.
		/// </summary>
		IPv4Address();

		/// <summary>
		/// Construct the address from a string.<br>
		/// <br>
		/// Here address can be either a decimal address (ex: "192.168.1.180") or a
		/// network name (ex: "localhost").
		/// </summary>
		/// <param name="address">IPv4 address or network name.</param>
		explicit IPv4Address(std::string_view address);

		/// <summary>
		/// Construct the address from a string.<br>
		/// <br>
		/// Here address can either be a decimal address (ex: "192.168.1.180") or a
		/// network name (ex: "localhost").<br>
		/// This is equivalent to the constructor taking a std::string
		/// parameter, it is defined for convenience so that the
		/// implicit conversion from literal strings to IPv4Address work.
		/// </summary>
		/// <param name="address">IPv4 address or network name.</param>
		explicit IPv4Address(const char* address);

		/// <summary>
		/// Construct the address from 4 bytes.<br>
		/// <br>
		/// Calling IPv4Address(a, b, c, d) is equivalent to calling
		/// IPv4Address("a.b.c.d"), but safer as it doesn't have to
		/// parse a string to get the address components.
		/// </summary>
		/// <param name="byte0">First byte of the address.</param>
		/// <param name="byte1">Second byte of the address.</param>
		/// <param name="byte2">Third byte of the address.</param>
		/// <param name="byte3">Fourth byte of the address.</param>
		IPv4Address(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3);

		/// <summary>
		/// Construct the address from a 32 Bit integer.<br>
		/// <br>
		/// This constructor uses the internal representation of
		/// the address directly.<br>
		/// It should be used for optimization purposes, and only if you
		/// get that representation from IPv4Address::ToInteger().
		/// </summary>
		/// <param name="address">4 Bytes of the address packed into a 32 Bit integer.</param>
		explicit IPv4Address(uint32_t address);

		/// <summary>
		/// Default Destructor.
		/// </summary>
		~IPv4Address() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		IPv4Address(const IPv4Address&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		IPv4Address& operator=(const IPv4Address&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		IPv4Address(IPv4Address&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		IPv4Address& operator=(IPv4Address&&) = default;

		/// <summary>
		/// Get a string representation of the address.<br>
		/// <br>
		/// The returned string is the decimal representation of the
		/// IPv4 address (like "192.168.1.180"), even if it was constructed
		/// from a host name.
		/// </summary>
		/// <returns>String representation of the address.</returns>
		std::string ToString() const;

		/// <summary>
		/// Get an integer representation of the address.<br>
		/// <br>
		/// The returned number is in the internal representation of the
		/// address, and should be used for optimization purposes only
		/// (like sending the address through a socket).<br>
		/// The integer produced by this function can then be converted
		/// back to a TRAP::Network::IPv4Address with the proper constructor.
		/// </summary>
		/// <returns>32 Bit unsigned integer representation of the address.</returns>
		uint32_t ToInteger() const;

		/// <summary>
		/// Get the computer's local address.<br>
		/// <br>
		/// The local address is the address of the computer from the
		/// LAN point of view, i.e. something like 192.168.1.180.<br>
		/// It is meaningful only for communications over the local network.<br>
		/// Unlike GetPublicAddress, this function is fast and may be used safely anywhere.
		/// </summary>
		/// <returns>Local IPv4 address of the computer.</returns>
		static IPv4Address GetLocalAddress();

		/// <summary>
		/// Get the computer's public address.<br>
		/// <br>
		/// The public address is the address of the computer from the
		/// internet point of view, i.e. something like 89.54.1.169.<br>
		/// It is necessary for communications over the world wide web.<br>
		/// The only way to get a public address is to ask it to a
		/// distant website; as a consequence, this function depends on
		/// both your network connection and the server, and may be
		/// very slow.<br>
		/// You should use it as few as possible.<br>
		/// Because this function depends on the network connection and on a distant
		/// server, you may use a time limit if you don't want your program
		/// to be possibly stuck waiting in case there is a problem; this
		/// limit is deactivated by default.
		/// </summary>
		/// <param name="timeout">Maximum time to wait.</param>
		/// <returns>Public IP address of the computer.</returns>
		static IPv4Address GetPublicAddress(Utils::TimeStep timeout = Utils::TimeStep(0.0f));

		static const IPv4Address None; //Value representing an empty/invalid address
		static const IPv4Address Any; //Value representing any address (0.0.0.0)
		static const IPv4Address LocalHost; //The "localhost" address (for connecting a computer to itself locally)
		static const IPv4Address Broadcast; //The "broadcast" address (for sending UDP messages to everyone on a local network)

	private:
		friend bool operator<(const IPv4Address& left, const IPv4Address& right);

		/// <summary>
		/// Resolve the given address string.
		/// </summary>
		/// <param name="address">Address string.</param>
		void Resolve(std::string_view address);

		uint32_t m_address; //Address stored as an unsigned 32 Bit integer
		bool m_valid; //Is the address valid?
	};

	/// <summary>
	/// Overload of == operator to compare two IP addresses.
	/// </summary>
	/// <param name="left">Left operand (a IP address).</param>
	/// <param name="right">Right operand (a IP address).</param>
	/// <returns>True if both addresses are equal.</returns>
	bool operator==(const IPv4Address& left, const IPv4Address& right);

	/// <summary>
	/// Overload of != operator to compare two IP addresses.
	/// </summary>
	/// <param name="left">Left operand (a IP address).</param>
	/// <param name="right">Right operand (a IP address).</param>
	/// <returns>True if both addresses are not equal.</returns>
	bool operator!=(const IPv4Address& left, const IPv4Address& right);

	/// <summary>
	/// Overload of < operator to compare two IP addresses.
	/// </summary>
	/// <param name="left">Left operand (a IP address).</param>
	/// <param name="right">Right operand (a IP address).</param>
	/// <returns>True if left is lesser than right.</returns>
	bool operator<(const IPv4Address& left, const IPv4Address& right);

	/// <summary>
	/// Overload of > operator to compare two IP addresses.
	/// </summary>
	/// <param name="left">Left operand (a IP address).</param>
	/// <param name="right">Right operand (a IP address).</param>
	/// <returns>True if left is greater than right.</returns>
	bool operator>(const IPv4Address& left, const IPv4Address& right);

	/// <summary>
	/// Overload of <= operator to compare two IP addresses.
	/// </summary>
	/// <param name="left">Left operand (a IP address).</param>
	/// <param name="right">Right operand (a IP address).</param>
	/// <returns>True if left is lesser or equal than right.</returns>
	bool operator <=(const IPv4Address& left, const IPv4Address& right);

	/// <summary>
	/// Overload of >= operator to compare two IP addresses.
	/// </summary>
	/// <param name="left">Left operand (a IP address).</param>
	/// <param name="right">Right operand (a IP address).</param>
	/// <returns>True if left is greater or equal than right.</returns>
	bool operator >=(const IPv4Address& left, const IPv4Address& right);

	/// <summary>
	/// Overload of >> operator to extract an IP address from input stream.
	/// </summary>
	/// <param name="stream">Input stream.</param>
	/// <param name="address">IP address to extract.</param>
	/// <returns>Reference to the input stream.</returns>
	std::istream& operator>>(std::istream& stream, IPv4Address& address);

	/// <summary>
	/// Overload of << operator to print an IP address to an output stream.
	/// </summary>
	/// <param name="stream">Output stream.</param>
	/// <param name="address">IP address to print.</param>
	/// <returns>Reference to the output stream.</returns>
	std::ostream& operator<<(std::ostream& stream, const IPv4Address& address);
}

#endif /*_TRAP_NETWORK_IPADDRESS_H*/