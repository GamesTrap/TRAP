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

#ifndef TRAP_NETWORK_PACKET_H
#define TRAP_NETWORK_PACKET_H

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
		/// <summary>
		/// A bool-like type that cannot be converted to integer or pointer types.
		/// </summary>
		typedef bool (Packet::*BoolType)(std::size_t);

	public:
		/// <summary>
		/// Constructor.<br>
		/// Creates an empty packet.
		/// </summary>
		Packet();

		/// <summary>
		/// Default virtual destructor.
		/// </summary>
		virtual ~Packet() = default;

		/// <summary>
		/// Append data to the end of the packet.
		/// </summary>
		/// <param name="data">Pointer to the sequence of bytes to append.</param>
		/// <param name="sizeInBytes">Number of bytes to append.</param>
		void Append(const void* data, std::size_t sizeInBytes);

		/// <summary>
		/// Get the current reading position in the packet.<br>
		/// The next read operation will read data from this position.
		/// </summary>
		/// <returns>The bytes offset of the current read position.</returns>
		std::size_t GetReadPosition() const;

		/// <summary>
		/// Clear the packet.<br>
		/// After calling Clear, the packet is empty.
		/// </summary>
		void Clear();

		/// <summary>
		/// Get a pointer to the data contained in the packet.<br>
		/// <br>
		/// Warning: the returned pointer may become invalid after
		/// you append data to the packet, therefore it should never
		/// be stored.<br>
		/// The return pointer is nullptr if the packet is empty.
		/// </summary>
		/// <returns>Pointer to the data.</returns>
		const void* GetData() const;

		/// <summary>
		/// Get the size of the data contained in the packet.<br>
		/// <br>
		/// This function returns the number of bytes pointer to by
		/// what GetData returns.
		/// </summary>
		/// <returns>Data size, in bytes.</returns>
		std::size_t GetDataSize() const;

		/// <summary>
		/// Tell if the reading position has reached the
		/// end of the packet.<br>
		/// <br>
		/// This function is useful to know if there is some data
		/// left to be read, without actually reading it.
		/// </summary>
		/// <returns>True if all data was read, false otherwise.</returns>
		bool EndOfPacket() const;

	public:
		/// <summary>
		/// Test the validity of the packet, for reading.<br>
		/// <br>
		/// This operator allows to test the packet as a boolean
		/// variable, to check if a reading operation was successful.<br>
		/// <br>
		/// A packet will be in an invalid state if it has no more
		/// data to read.<br>
		/// <br>
		/// This behavior is the same as standard C++ streams.<br>
		/// <br>
		/// Don't focus on the return type, it's equivalent to bool but
		/// it disallows unwanted implicit conversions to integer or
		/// pointer types.
		/// </summary>
		/// <returns>True if last data extraction from packet was successful.</returns>
		operator BoolType() const;

		/// <summary>
		/// Overload of operator >> to read data from the packet.
		/// </summary>
		Packet& operator>>(bool& data);
		Packet& operator>>(int8_t& data);
		Packet& operator>>(uint8_t& data);
		Packet& operator>>(int16_t& data);
		Packet& operator>>(uint16_t& data);
		Packet& operator>>(int32_t& data);
		Packet& operator>>(uint32_t& data);
		Packet& operator>>(int64_t& data);
		Packet& operator>>(uint64_t& data);
		Packet& operator>>(float& data);
		Packet& operator>>(double& data);
		Packet& operator>>(char* data);
		Packet& operator>>(std::string& data);
		Packet& operator>>(wchar_t* data);
		Packet& operator>>(std::wstring& data);

		/// <summary>
		/// Overload of operator << to write data into the packet.
		/// </summary>
		Packet& operator<<(bool data);
		Packet& operator<<(int8_t data);
		Packet& operator<<(uint8_t data);
		Packet& operator<<(int16_t data);
		Packet& operator<<(uint16_t data);
		Packet& operator<<(int32_t data);
		Packet& operator<<(uint32_t data);
		Packet& operator<<(int64_t data);
		Packet& operator<<(uint64_t data);
		Packet& operator<<(float data);
		Packet& operator<<(double data);
		Packet& operator<<(const char* data);
		Packet& operator<<(const std::string& data);
		Packet& operator<<(const wchar_t* data);
		Packet& operator<<(const std::wstring& data);

	protected:
		friend class TCPSocket;
		friend class UDPSocket;
		friend class TCPSocketIPv6;
		friend class UDPSocketIPv6;

		/// <summary>
		/// Called before the packet is sent over the network.<br>
		/// <br>
		/// This function can be defined by derived classes to
		/// transform the data before it is sent; this can be
		/// used for compression encryption, etc.<br>
		/// The function must return a pointer to the modified data,
		/// as well as the number of bytes pointed.<br>
		/// The default implementation provides the packet's data
		/// without transforming it.
		/// </summary>
		/// <param name="size">Variable to fill with the size of data to send.</param>
		/// <returns>Pointer to the array of bytes to send.</returns>
		virtual const void* OnSend(std::size_t& size);

		/// <summary>
		/// Called after the packet is received over the network.<br>
		/// <br>
		/// This function can be defined by derived classes to
		/// transform the data after it is received; this can be
		/// used for decompression, decryption, etc.<br>
		/// The function receives a pointer to the received data,
		/// and must fill the packet with the transformed bytes.<br>
		/// The default implementation fills the packet directly
		/// without transforming the data.
		/// </summary>
		/// <param name="data">Pointer to the received bytes.</param>
		/// <param name="size">Number of bytes.</param>
		virtual void OnReceive(const void* data, std::size_t size);

	public:
		/// <summary>
		/// Disallow comparisons between packets.
		/// </summary>
		bool operator==(const Packet& right) const = delete;
		/// <summary>
		/// Disallow comparisons between packets.
		/// </summary>
		bool operator!=(const Packet & right) const = delete;

	private:
		/// <summary>
		/// Check if the packet can extract a given number of bytes.<br>
		/// <br>
		/// This function updates accordingly the state of the packet.
		/// </summary>
		/// <param name="size">Size to check.</param>
		/// <returns>True if size bytes can be read from the packet.</returns>
		bool CheckSize(std::size_t size);

		std::vector<char> m_data; //Data stored in the packet
		std::size_t m_readPos; //Current reading position in the packet
		std::size_t m_sendPos; //Current send position in the packet (for handling partial sends)
		bool m_isValid; //Reading state of the packet
	};
}

#endif /*TRAP_NETWORK_PACKET_H*/