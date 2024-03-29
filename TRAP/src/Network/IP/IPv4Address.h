/*
////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2023 Laurent Gomila (laurent@sfml-dev.org)
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

#ifndef TRAP_IPV4ADDRESS_H
#define TRAP_IPV4ADDRESS_H

#include <cstdint>
#include <string_view>

#include <fmt/ostream.h>

#include "Core/Base.h"
#include "Utils/Utils.h"
#include "Utils/Memory.h"
#include "Utils/Time/TimeStep.h"
#include "Utils/NumericCasts.h"

namespace TRAP::Network
{
	/// <summary>
	/// TRAP::Network::IPv4Address is a utility class for manipulating network addresses.
	/// It provides a set of implicit constructors and conversion functions to easily build or transform
	/// an IPv4 address from/to various representations.
	/// </summary>
	class IPv4Address
	{
	public:
		/// <summary>
		/// This constructor creates an empty (invalid) address.
		/// </summary>
		constexpr IPv4Address() noexcept = default;

		/// <summary>
		/// Construct the address from a string.
		///
		/// Here address can be either a decimal address (ex: "192.168.1.180") or a
		/// network name (ex: "localhost").
		/// </summary>
		/// <param name="address">IPv4 address or network name.</param>
		explicit IPv4Address(std::string_view address);

		/// <summary>
		/// Construct the address from 4 bytes.
		///
		/// Calling IPv4Address(a, b, c, d) is equivalent to calling
		/// IPv4Address("a.b.c.d"), but safer as it doesn't have to
		/// parse a string to get the address components.
		/// </summary>
		/// <param name="byte0">First byte of the address.</param>
		/// <param name="byte1">Second byte of the address.</param>
		/// <param name="byte2">Third byte of the address.</param>
		/// <param name="byte3">Fourth byte of the address.</param>
		constexpr IPv4Address(u8 byte0, u8 byte1, u8 byte2, u8 byte3);

		/// <summary>
		/// Construct the address from a 32 bit integer.
		///
		/// This constructor uses the internal representation of
		/// the address directly.
		/// It should be used for optimization purposes, and only if you
		/// get that representation from IPv4Address::ToInteger().
		/// </summary>
		/// <param name="address">4 Bytes of the address packed into a 32 bit integer.</param>
		constexpr explicit IPv4Address(u32 address);

		/// <summary>
		/// Get a string representation of the address.
		///
		/// The returned string is the decimal representation of the
		/// IPv4 address (like "192.168.1.180"), even if it was constructed
		/// from a host name.
		/// </summary>
		/// <returns>String representation of the address.</returns>
		[[nodiscard]] std::string ToString() const;

		/// <summary>
		/// Get an integer representation of the address.
		///
		/// The returned number is in the internal representation of the
		/// address, and should be used for optimization purposes only
		/// (like sending the address through a socket).
		/// The integer produced by this function can then be converted
		/// back to a TRAP::Network::IPv4Address with the proper constructor.
		/// </summary>
		/// <returns>32 bit unsigned integer representation of the address.</returns>
		[[nodiscard]] constexpr u32 ToInteger() const;

		/// <summary>
		/// Get the computer's local address.
		///
		/// The local address is the address of the computer from the
		/// LAN point of view, i.e. something like 192.168.1.180.
		/// It is meaningful only for communications over the local network.
		/// Unlike GetPublicAddress, this function is fast and may be used safely anywhere.
		/// </summary>
		/// <returns>Local IPv4 address of the computer.</returns>
		[[nodiscard]] static IPv4Address GetLocalAddress();

		/// <summary>
		/// Get the computer's public address.
		///
		/// The public address is the address of the computer from the
		/// internet point of view, i.e. something like 89.54.1.169.
		/// It is necessary for communications over the world wide web.
		/// The only way to get a public address is to ask it to a
		/// distant website; as a consequence, this function depends on
		/// both your network connection and the server, and may be
		/// very slow.
		/// You should use it as few as possible.
		/// Because this function depends on the network connection and on a distant
		/// server, you may use a time limit if you don't want your program
		/// to be possibly stuck waiting in case there is a problem; this
		/// limit is deactivated by default.
		/// </summary>
		/// <param name="timeout">Maximum time to wait.</param>
		/// <returns>Public IP address of the computer.</returns>
		[[nodiscard]] static IPv4Address GetPublicAddress(Utils::TimeStep timeout = Utils::TimeStep(0.0f));

		static const IPv4Address None;      //Value representing an empty/invalid address
		static const IPv4Address Any;       //Value representing any address (0.0.0.0)
		static const IPv4Address LocalHost; //The "localhost" address (for connecting a computer to itself locally)
		static const IPv4Address Broadcast; //The "broadcast" address (for sending UDP messages to everyone on a local network)

		[[nodiscard]] friend constexpr auto operator<=>(const IPv4Address& lhs, const IPv4Address& rhs)
		{
			return lhs.m_address <=> rhs.m_address;
		}

		[[nodiscard]] friend constexpr auto operator==(const IPv4Address& lhs, const IPv4Address& rhs)
		{
			return lhs.m_address == rhs.m_address;
		}

	private:
		/// <summary>
		/// Resolve the given address string.
		/// </summary>
		/// <param name="address">Address string.</param>
		void Resolve(std::string_view address);

		u32 m_address; //Address stored as an unsigned 32 bit integer
		bool m_valid; //Is the address valid?
	};

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Overload of >> operator to extract an IP address from input stream.
	/// </summary>
	/// <param name="stream">Input stream.</param>
	/// <param name="address">IP address to extract.</param>
	/// <returns>Reference to the input stream.</returns>
	std::istream& operator>>(std::istream& stream, TRAP::Network::IPv4Address& address);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Overload of << operator to print an IP address to an output stream.
	/// </summary>
	/// <param name="stream">Output stream.</param>
	/// <param name="address">IP address to print.</param>
	/// <returns>Reference to the output stream.</returns>
	std::ostream& operator<<(std::ostream& stream, const TRAP::Network::IPv4Address& address);
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::IPv4Address::IPv4Address(const u8 byte0, const u8 byte1, const u8 byte2,
                                                  const u8 byte3)
	: m_address((NumericCast<u32>(byte0) << 24u) | (NumericCast<u32>(byte1) << 16u) |
	             (NumericCast<u32>(byte2) << 8u) | NumericCast<u32>(byte3)), m_valid(true)
{
	if constexpr (Utils::GetEndian() != Utils::Endian::Big)
		TRAP::Utils::Memory::SwapBytes(m_address);
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::IPv4Address::IPv4Address(const u32 address)
	: m_address(address), m_valid(true)
{
	if constexpr (Utils::GetEndian() != Utils::Endian::Big)
		TRAP::Utils::Memory::SwapBytes(m_address);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Network::IPv4Address::ToInteger() const
{
	u32 address = m_address;

	if constexpr (Utils::GetEndian() != Utils::Endian::Big)
		TRAP::Utils::Memory::SwapBytes(address);

	return address;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Network::IPv4Address> : fmt::ostream_formatter
{};

#endif /*TRAP_IPV4ADDRESS_H*/