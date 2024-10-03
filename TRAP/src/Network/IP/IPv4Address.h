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

//Modified by: Jan "GamesTrap" Schuerkamp

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
	/// @brief TRAP::Network::IPv4Address is a utility class for manipulating network addresses.
	/// It provides a set of implicit constructors and conversion functions to easily build or transform
	/// an IPv4 address from/to various representations.
	class IPv4Address
	{
	public:
		/// @brief This constructor creates an empty (invalid) address.
		constexpr IPv4Address() noexcept = default;

		/// @brief Construct the address from a string.
		///
		/// Here address can be either a decimal address (ex: "192.168.1.180") or a
		/// network name (ex: "localhost").
		/// @param address IPv4 address or network name.
		explicit IPv4Address(const std::string& address);

		/// @brief Construct the address from 4 bytes.
		///
		/// Calling IPv4Address(a, b, c, d) is equivalent to calling
		/// IPv4Address("a.b.c.d"), but safer as it doesn't have to
		/// parse a string to get the address components.
		/// @param byte0 First byte of the address.
		/// @param byte1 Second byte of the address.
		/// @param byte2 Third byte of the address.
		/// @param byte3 Fourth byte of the address.
		constexpr IPv4Address(u8 byte0, u8 byte1, u8 byte2, u8 byte3);

		/// @brief Construct the address from a 32 bit integer.
		///
		/// This constructor uses the internal representation of
		/// the address directly.
		/// It should be used for optimization purposes, and only if you
		/// get that representation from IPv4Address::ToInteger().
		/// @param address 4 Bytes of the address packed into a 32 bit integer.
		constexpr explicit IPv4Address(u32 address);

		/// @brief Copy constructor.
		constexpr IPv4Address(const IPv4Address&) = default;
		/// @brief Copy assignment operator.
		constexpr IPv4Address& operator=(const IPv4Address&) = default;
		/// @brief Move constructor.
		constexpr IPv4Address(IPv4Address&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr IPv4Address& operator=(IPv4Address&&) noexcept = default;

		/// @brief Destructor.
		constexpr ~IPv4Address() = default;

		/// @brief Get a string representation of the address.
		///
		/// The returned string is the decimal representation of the
		/// IPv4 address (like "192.168.1.180"), even if it was constructed
		/// from a host name.
		/// @return String representation of the address.
		[[nodiscard]] std::string ToString() const;

		/// @brief Get an integer representation of the address.
		///
		/// The returned number is in the internal representation of the
		/// address, and should be used for optimization purposes only
		/// (like sending the address through a socket).
		/// The integer produced by this function can then be converted
		/// back to a TRAP::Network::IPv4Address with the proper constructor.
		/// @return 32 bit unsigned integer representation of the address.
		[[nodiscard]] constexpr u32 ToInteger() const;

		/// @brief Get the computer's local address.
		///
		/// The local address is the address of the computer from the
		/// LAN point of view, i.e. something like 192.168.1.180.
		/// It is meaningful only for communications over the local network.
		/// Unlike GetPublicAddress, this function is fast and may be used safely anywhere.
		/// @return Local IPv4 address of the computer.
		[[nodiscard]] static IPv4Address GetLocalAddress();

		/// @brief Get the computer's public address.
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
		/// @param timeout Maximum time to wait.
		/// @return Public IP address of the computer.
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
		/// @brief Resolve the given address string.
		/// @param address Address string.
		void Resolve(const std::string& address);

		u32 m_address = 0u; //Address stored as an unsigned 32 bit integer
		bool m_valid = false; //Is the address valid?
	};

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Overload of >> operator to extract an IP address from input stream.
	/// @param stream Input stream.
	/// @param address IP address to extract.
	/// @return Reference to the input stream.
	std::istream& operator>>(std::istream& stream, TRAP::Network::IPv4Address& address);
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
struct fmt::formatter<TRAP::Network::IPv4Address>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Network::IPv4Address& ip,
	                                            fmt::format_context& ctx)
    {
		return fmt::format_to(ctx.out(), "{}", ip.ToString());
    }
};

#endif /*TRAP_IPV4ADDRESS_H*/
