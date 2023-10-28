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

#ifndef TRAP_NETWORK_PACKET_H
#define TRAP_NETWORK_PACKET_H

#include "Utils/Utils.h"
#include "Utils/Memory.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace TRAP::Network
{
	class TCPSocket;
	class UDPSocket;
	class TCPSocketIPv6;
	class UDPSocketIPv6;

	/// <summary>
	/// Utility class to build blocks of data to transfer.
	/// over the network
	/// </summary>
	class Packet
	{
	public:
		/// <summary>
		/// Constructor.
		/// Creates an empty packet.
		/// </summary>
		constexpr Packet() noexcept = default;

		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr virtual ~Packet() = default;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr Packet(const Packet&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr Packet& operator=(const Packet&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr Packet(Packet&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr Packet& operator=(Packet&&) noexcept = default;

		/// <summary>
		/// Append data to the end of the packet.
		/// </summary>
		/// <param name="data">Pointer to the sequence of bytes to append.</param>
		/// <param name="sizeInBytes">Number of bytes to append.</param>
		constexpr void Append(const void* data, usize sizeInBytes);

		/// <summary>
		/// Get the current reading position in the packet.
		/// The next read operation will read data from this position.
		/// </summary>
		/// <returns>The bytes offset of the current read position.</returns>
		[[nodiscard]] constexpr usize GetReadPosition() const noexcept;

		/// <summary>
		/// Clear the packet.
		/// After calling Clear, the packet is empty.
		/// </summary>
		constexpr void Clear() noexcept;

		/// <summary>
		/// Get a pointer to the data contained in the packet.
		///
		/// Warning: The returned pointer may become invalid after
		/// you append data to the packet, therefore it should never
		/// be stored.
		/// The return pointer is nullptr if the packet is empty.
		/// </summary>
		/// <returns>Pointer to the data.</returns>
		[[nodiscard]] constexpr const void* GetData() const;

		/// <summary>
		/// Get the size of the data contained in the packet.
		///
		/// This function returns the number of bytes pointer to by
		/// what GetData returns.
		/// </summary>
		/// <returns>Data size, in bytes.</returns>
		[[nodiscard]] constexpr usize GetDataSize() const noexcept;

		/// <summary>
		/// Tell if the reading position has reached the
		/// end of the packet.
		///
		/// This function is useful to know if there is some data
		/// left to be read, without actually reading it.
		/// </summary>
		/// <returns>True if all data was read, false otherwise.</returns>
		[[nodiscard]] constexpr bool EndOfPacket() const noexcept;

		/// <summary>
		/// Test the validity of the packet, for reading.
		///
		/// This operator allows to test the packet as a boolean
		/// variable, to check if a reading operation was successful.
		///
		/// A packet will be in an invalid state if it has no more
		/// data to read.
		///
		/// This behavior is the same as standard C++ streams.
		///
		/// Don't focus on the return type, it's equivalent to bool but
		/// it disallows unwanted implicit conversions to integer or
		/// pointer types.
		/// </summary>
		/// <returns>True if last data extraction from packet was successful.</returns>
		constexpr explicit operator bool() const noexcept;

		/// <summary>
		/// Overload of operator >> to read data from the packet.
		/// </summary>
		constexpr Packet& operator>>(bool& data);
		constexpr Packet& operator>>(i8& data);
		constexpr Packet& operator>>(u8& data);
		constexpr Packet& operator>>(i16& data);
		constexpr Packet& operator>>(u16& data);
		constexpr Packet& operator>>(i32& data);
		constexpr Packet& operator>>(u32& data);
		constexpr Packet& operator>>(i64& data);
		constexpr Packet& operator>>(u64& data);
		constexpr Packet& operator>>(f32& data);
		constexpr Packet& operator>>(f64& data);
		constexpr Packet& operator>>(char* data);
		constexpr Packet& operator>>(std::string& data);
		constexpr Packet& operator>>(wchar_t* data);
		constexpr Packet& operator>>(std::wstring& data);

		/// <summary>
		/// Overload of operator << to write data into the packet.
		/// </summary>
		constexpr Packet& operator<<(bool data);
		constexpr Packet& operator<<(i8 data);
		constexpr Packet& operator<<(u8 data);
		constexpr Packet& operator<<(i16 data);
		constexpr Packet& operator<<(u16 data);
		constexpr Packet& operator<<(i32 data);
		constexpr Packet& operator<<(u32 data);
		constexpr Packet& operator<<(i64 data);
		constexpr Packet& operator<<(u64 data);
		constexpr Packet& operator<<(f32 data);
		constexpr Packet& operator<<(f64 data);
		constexpr Packet& operator<<(std::string_view data);
		constexpr Packet& operator<<(std::wstring_view data);

	protected:
		friend class TCPSocket;
		friend class UDPSocket;
		friend class TCPSocketIPv6;
		friend class UDPSocketIPv6;

		/// <summary>
		/// Called before the packet is sent over the network.
		///
		/// This function can be defined by derived classes to
		/// transform the data before it is sent; this can be
		/// used for compression encryption, etc.
		/// The function must return a pointer to the modified data,
		/// as well as the number of bytes pointed.
		/// The default implementation provides the packet's data
		/// without transforming it.
		/// </summary>
		/// <param name="size">Variable to fill with the size of data to send.</param>
		/// <returns>Pointer to the array of bytes to send.</returns>
		[[nodiscard]] virtual const void* OnSend(usize& size);

		/// <summary>
		/// Called after the packet is received over the network.
		///
		/// This function can be defined by derived classes to
		/// transform the data after it is received; this can be
		/// used for decompression, decryption, etc.
		/// The function receives a pointer to the received data,
		/// and must fill the packet with the transformed bytes.
		/// The default implementation fills the packet directly
		/// without transforming the data.
		/// </summary>
		/// <param name="data">Pointer to the received bytes.</param>
		/// <param name="size">Number of bytes.</param>
		constexpr virtual void OnReceive(const void* data, usize size);

	public:
		/// <summary>
		/// Disallow comparisons between packets.
		/// </summary>
		[[nodiscard]] constexpr bool operator==(const Packet& right) const noexcept = delete;
		/// <summary>
		/// Disallow comparisons between packets.
		/// </summary>
		[[nodiscard]] constexpr bool operator!=(const Packet& right) const noexcept = delete;

	private:
		/// <summary>
		/// Check if the packet can extract a given number of bytes.
		///
		/// This function updates accordingly the state of the packet.
		/// </summary>
		/// <param name="size">Size to check.</param>
		/// <returns>True if size bytes can be read from the packet.</returns>
		[[nodiscard]] constexpr bool CheckSize(usize size) noexcept;

		std::vector<u8> m_data{}; //Data stored in the packet
		usize m_readPos = 0;  //Current reading position in the packet
		usize m_sendPos = 0;  //Current send position in the packet (for handling partial sends)
		bool m_isValid = true;      //Reading state of the packet
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Network::Packet::Append(const void* const data, const usize sizeInBytes)
{
	if((data == nullptr) || (sizeInBytes == 0))
		return;

	const auto start = std::ssize(m_data);
	m_data.resize(m_data.size() + sizeInBytes);
	std::copy_n(static_cast<const u8*>(data), sizeInBytes, std::next(m_data.begin(), start));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr usize TRAP::Network::Packet::GetReadPosition() const noexcept
{
	return m_readPos;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Network::Packet::Clear() noexcept
{
	m_data.clear();
	m_readPos = 0;
	m_isValid = true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const void* TRAP::Network::Packet::GetData() const
{
	return !m_data.empty() ? m_data.data() : nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr usize TRAP::Network::Packet::GetDataSize() const noexcept
{
	return m_data.size();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Network::Packet::EndOfPacket() const noexcept
{
	return m_readPos >= m_data.size();
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet::operator bool() const noexcept
{
	return m_isValid;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Network::Packet::CheckSize(const usize size) noexcept
{
	m_isValid = m_isValid && (m_readPos + size <= m_data.size());

	return m_isValid;
}

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator>>(bool& data)
{
	u8 value = 0;
	if (*this >> value)
		data = (value != 0);

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator>>(i8& data)
{
	if(CheckSize(sizeof(i8)))
	{
		std::copy_n(&m_data[m_readPos], sizeof(i8), &data);
		m_readPos += sizeof(i8);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator>>(u8& data)
{
	if(CheckSize(sizeof(u8)))
	{
		std::copy_n(&m_data[m_readPos], sizeof(u8), &data);
		m_readPos += sizeof(u8);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator>>(i16& data)
{
	if(CheckSize(sizeof(i16)))
	{
		std::copy_n(&m_data[m_readPos], sizeof(i16), &data);

		if constexpr (Utils::GetEndian() == Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(i16);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator>>(u16& data)
{
	if (CheckSize(sizeof(u16)))
	{
		std::copy_n(&m_data[m_readPos], sizeof(u16), &data);

		if constexpr (Utils::GetEndian() == Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(u16);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator>>(i32& data)
{
	if (CheckSize(sizeof(i32)))
	{
		std::copy_n(&m_data[m_readPos], sizeof(i32), &data);

		if constexpr (Utils::GetEndian() == Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(i32);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator>>(u32& data)
{
	if (CheckSize(sizeof(u32)))
	{
		std::copy_n(&m_data[m_readPos], sizeof(u32), &data);

		if constexpr (Utils::GetEndian() == Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(u32);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator>>(i64& data)
{
	if(CheckSize(sizeof(i64)))
	{
		std::copy_n(&m_data[m_readPos], sizeof(i64), &data);

		if constexpr (Utils::GetEndian() == Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(i64);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator>>(u64& data)
{
	if (CheckSize(sizeof(u64)))
	{
		std::copy_n(&m_data[m_readPos], sizeof(u64), &data);

		if constexpr (Utils::GetEndian() == Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(u64);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator>>(f32& data)
{
	if(CheckSize(sizeof(f32)))
	{
		std::copy_n(&m_data[m_readPos], sizeof(f32), &data);
		m_readPos += sizeof(f32);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator>>(f64& data)
{
	if (CheckSize(sizeof(f64)))
	{
		std::copy_n(&m_data[m_readPos], sizeof(f64), &data);
		m_readPos += sizeof(f64);
	}

	return *this;
}


//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator>>(char* const data)
{
	//First extract string length
	u32 length = 0;
	*this >> length;

	if((length > 0) && CheckSize(length))
	{
		//Then extract characters
		std::copy_n(&m_data[m_readPos], length, data);
		data[length] = '\0';

		//Update reading position
		m_readPos += length;
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator>>(std::string& data)
{
	//First extract string length
	u32 length = 0;
	*this >> length;

	data.clear();
	if((length > 0) && CheckSize(length))
	{
		//Then extract characters
		data.assign(reinterpret_cast<const char*>(&m_data[m_readPos]), length);

		//Update reading position
		m_readPos += length;
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator>>(wchar_t* const data)
{
	//First extract string length
	u32 length = 0;
	*this >> length;

	if((length > 0) && CheckSize(length * sizeof(u32)))
	{
		//Then extract characters
		for(u32 i = 0; i < length; ++i)
		{
			u32 character = 0;
			*this >> character;
			data[i] = static_cast<wchar_t>(character);
		}
		data[length] = L'\0';
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator>>(std::wstring& data)
{
	//First extract the string length
	u32 length = 0;
	*this >> length;

	data.clear();
	if((length > 0) && CheckSize(length * sizeof(u32)))
	{
		//Then extract characters
		for(u32 i = 0; i < length; ++i)
		{
			u32 character = 0;
			*this >> character;
			data += static_cast<wchar_t>(character);
		}
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const bool data)
{
	*this << static_cast<u8>(data);
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const i8 data)
{
	Append(&data, sizeof(i8));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const u8 data)
{
	Append(&data, sizeof(u8));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator<<(i16 data)
{
	if constexpr (Utils::GetEndian() == Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(data); //Need to convert to big endian

	Append(&data, sizeof(i16));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator<<(u16 data)
{
	if constexpr (Utils::GetEndian() == Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(data); //Need to convert to big endian

	Append(&data, sizeof(u16));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator<<(i32 data)
{
	if constexpr (Utils::GetEndian() == Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(data); //Need to convert to big endian

	Append(&data, sizeof(i32));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator<<(u32 data)
{
	if constexpr (Utils::GetEndian() == Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(data); //Need to convert to big endian

	Append(&data, sizeof(u32));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator<<(i64 data)
{
	if constexpr (Utils::GetEndian() == Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(data); //Need to convert to big endian

	Append(&data, sizeof(i64));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator<<(u64 data)
{
	if constexpr (Utils::GetEndian() == Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(data); //Need to convert to big endian

	Append(&data, sizeof(u64));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const f32 data)
{
	Append(&data, sizeof(f32));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const f64 data)
{
	Append(&data, sizeof(f64));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const std::string_view data)
{
	//First insert string length
	const usize length = data.size();
	*this << length;

	//Then insert characters
	if (length > 0)
		Append(data.data(), length * sizeof(std::string_view::value_type));

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const std::wstring_view data)
{
	//First insert string length
	const usize length = data.size();
	*this << length;

	//Then insert characters
	if (length > 0)
		Append(data.data(), length * sizeof(std::wstring::value_type));

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Network::Packet::OnReceive(const void* const data, const usize size)
{
	Append(data, size);
}

#endif /*TRAP_NETWORK_PACKET_H*/
