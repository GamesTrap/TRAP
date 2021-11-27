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

#include "TRAPPCH.h"
#include "Packet.h"

#include "Utils/Utils.h"
#include "Utils/ByteSwap.h"

TRAP::Network::Packet::Packet()
	: m_readPos(0), m_sendPos(0), m_isValid(true)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::Packet::Append(const void* data, const std::size_t sizeInBytes)
{
	if(!data || (sizeInBytes == 0))
		return;

	const std::size_t start = m_data.size();
	m_data.resize(start + sizeInBytes);
	std::memcpy(&m_data[start], data, sizeInBytes);
}

//-------------------------------------------------------------------------------------------------------------------//

std::size_t TRAP::Network::Packet::GetReadPosition() const
{
	return m_readPos;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::Packet::Clear()
{
	m_data.clear();
	m_readPos = 0;
	m_isValid = true;
}

//-------------------------------------------------------------------------------------------------------------------//

const void* TRAP::Network::Packet::GetData() const
{
	return !m_data.empty() ? &m_data[0] : nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

std::size_t TRAP::Network::Packet::GetDataSize() const
{
	return m_data.size();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::Packet::EndOfPacket() const
{
	return m_readPos >= m_data.size();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet::operator TRAP::Network::Packet::BoolType() const
{
	return m_isValid ? &Packet::CheckSize : nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(bool& data)
{
	uint8_t value;
	if (*this >> value)
		data = (value != 0);

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(int8_t& data)
{
	if(CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const int8_t*>(&m_data[m_readPos]);
		m_readPos += sizeof(data);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(uint8_t& data)
{
	if(CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const uint8_t*>(&m_data[m_readPos]);
		m_readPos += sizeof(data);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(int16_t& data)
{
	if(CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<int16_t*>(&m_data[m_readPos]);

		if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(data);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(uint16_t& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<uint16_t*>(&m_data[m_readPos]);

		if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(data);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(int32_t& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<int32_t*>(&m_data[m_readPos]);

		if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(data);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(uint32_t& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<uint32_t*>(&m_data[m_readPos]);

		if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(data);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(int64_t& data)
{
	if(CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<int64_t*>(&m_data[m_readPos]);

		if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(data);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(uint64_t& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<uint64_t*>(&m_data[m_readPos]);

		if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(data);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(float& data)
{
	if(CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<float*>(&m_data[m_readPos]);
		m_readPos += sizeof(data);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(double& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<double*>(&m_data[m_readPos]);
		m_readPos += sizeof(data);
	}

	return *this;
}


//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(char* data)
{
	//First extract string length
	uint32_t length = 0;
	*this >> length;

	if((length > 0) && CheckSize(length))
	{
		//Then extract characters
		std::memcpy(data, &m_data[m_readPos], length);
		data[length] = '\0';

		//Update reading position
		m_readPos += length;
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(std::string& data)
{
	//First extract string length
	uint32_t length = 0;
	*this >> length;

	data.clear();
	if((length > 0) && CheckSize(length))
	{
		//Then extract characters
		data.assign(&m_data[m_readPos], length);

		//Update reading position
		m_readPos += length;
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(wchar_t* data)
{
	//First extract string length
	uint32_t length = 0;
	*this >> length;

	if((length > 0) && CheckSize(length * sizeof(uint32_t)))
	{
		//Then extract characters
		for(uint32_t i = 0; i < length; ++i)
		{
			uint32_t character = 0;
			*this >> character;
			data[i] = static_cast<wchar_t>(character);
		}
		data[length] = L'\0';
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(std::wstring& data)
{
	//First extract the string length
	uint32_t length = 0;
	*this >> length;

	data.clear();
	if((length > 0) && CheckSize(length * sizeof(uint32_t)))
	{
		//Then extract characters
		for(uint32_t i = 0; i < length; ++i)
		{
			uint32_t character = 0;
			*this >> character;
			data += static_cast<wchar_t>(character);
		}
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const bool data)
{
	*this << static_cast<uint8_t>(data);
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(int8_t data)
{
	Append(&data, sizeof(data));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(uint8_t data)
{
	Append(&data, sizeof(data));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const int16_t data)
{
	int16_t toWrite = data;

	if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(toWrite); //Need to convert to big endian

	Append(&toWrite, sizeof(toWrite));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const uint16_t data)
{
	uint16_t toWrite = data;

	if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(toWrite); //Need to convert to big endian

	Append(&toWrite, sizeof(toWrite));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const int32_t data)
{
	int32_t toWrite = data;

	if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(toWrite); //Need to convert to big endian

	Append(&toWrite, sizeof(toWrite));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const uint32_t data)
{
	uint32_t toWrite = data;

	if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(toWrite); //Need to convert to big endian

	Append(&toWrite, sizeof(toWrite));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const int64_t data)
{
	int64_t toWrite = data;

	if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(toWrite); //Need to convert to big endian

	Append(&toWrite, sizeof(toWrite));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const uint64_t data)
{
	uint64_t toWrite = data;

	if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(toWrite); //Need to convert to big endian

	Append(&toWrite, sizeof(toWrite));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(float data)
{
	Append(&data, sizeof(data));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(double data)
{
	Append(&data, sizeof(data));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const char* data)
{
	//First insert string length
	const uint32_t length = static_cast<uint32_t>(std::strlen(data));
	*this << length;

	//Then insert characters
	Append(data, length * sizeof(char));

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const std::string_view data)
{
	//First insert string length
	const uint32_t length = static_cast<uint32_t>(data.size());
	*this << length;

	//Then insert characters
	if (length > 0)
		Append(data.data(), length * sizeof(std::string::value_type));

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const wchar_t* data)
{
	//First insert string length
	const uint32_t length = static_cast<uint32_t>(std::wcslen(data));
	*this << length;

	//Then insert characters
	for (const wchar_t* c = data; *c != L'\0'; ++c)
		*this << static_cast<uint32_t>(*c);

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const std::wstring& data)
{
	//First insert string length
	const uint32_t length = static_cast<uint32_t>(data.size());
	*this << length;

	//Then insert characters
	if(length > 0)
	{
		for (std::wstring::const_iterator c = data.begin(); c != data.end(); ++c)
			*this << static_cast<uint32_t>(*c);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::Packet::CheckSize(const std::size_t size)
{
	m_isValid = m_isValid && (m_readPos + size <= m_data.size());

	return m_isValid;
}

//-------------------------------------------------------------------------------------------------------------------//

const void* TRAP::Network::Packet::OnSend(std::size_t& size)
{
	size = GetDataSize();
	return GetData();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::Packet::OnReceive(const void* data, const std::size_t size)
{
	Append(data, size);
}