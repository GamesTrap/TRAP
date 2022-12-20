/*
////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2022 Laurent Gomila (laurent@sfml-dev.org)
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
#include "Utils/Memory.h"

TRAP::Network::Packet::Packet() noexcept
	: m_readPos(0), m_sendPos(0), m_isValid(true)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::Packet::Append(const void* const data, const std::size_t sizeInBytes)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if(!data || (sizeInBytes == 0))
		return;

	const std::size_t start = m_data.size();
	m_data.resize(start + sizeInBytes);
	memcpy(&m_data[start], data, sizeInBytes);
}

//-------------------------------------------------------------------------------------------------------------------//

std::size_t TRAP::Network::Packet::GetReadPosition() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_readPos;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::Packet::Clear() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	m_data.clear();
	m_readPos = 0;
	m_isValid = true;
}

//-------------------------------------------------------------------------------------------------------------------//

const void* TRAP::Network::Packet::GetData() const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return !m_data.empty() ? &m_data[0] : nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

std::size_t TRAP::Network::Packet::GetDataSize() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_data.size();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::Packet::EndOfPacket() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_readPos >= m_data.size();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet::operator bool() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_isValid;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(bool& data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	uint8_t value = 0;
	if (*this >> value)
		data = (value != 0);

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(int8_t& data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if(CheckSize(sizeof(int8_t)))
	{
		memcpy(&data, &m_data[m_readPos], sizeof(int8_t));
		m_readPos += sizeof(int8_t);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(uint8_t& data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if(CheckSize(sizeof(uint8_t)))
	{
		memcpy(&data, &m_data[m_readPos], sizeof(uint8_t));
		m_readPos += sizeof(uint8_t);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(int16_t& data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if(CheckSize(sizeof(int16_t)))
	{
		memcpy(&data, &m_data[m_readPos], sizeof(int16_t));

		if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(int16_t);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(uint16_t& data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if (CheckSize(sizeof(uint16_t)))
	{
		memcpy(&data, &m_data[m_readPos], sizeof(uint16_t));

		if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(uint16_t);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(int32_t& data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if (CheckSize(sizeof(int32_t)))
	{
		memcpy(&data, &m_data[m_readPos], sizeof(int32_t));

		if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(int32_t);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(uint32_t& data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if (CheckSize(sizeof(uint32_t)))
	{
		memcpy(&data, &m_data[m_readPos], sizeof(uint32_t));

		if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(uint32_t);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(int64_t& data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if(CheckSize(sizeof(int64_t)))
	{
		memcpy(&data, &m_data[m_readPos], sizeof(int64_t));

		if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(int64_t);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(uint64_t& data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if (CheckSize(sizeof(uint64_t)))
	{
		memcpy(&data, &m_data[m_readPos], sizeof(uint64_t));

		if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little) //Need to convert to little endian
			TRAP::Utils::Memory::SwapBytes(data);

		m_readPos += sizeof(uint64_t);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(float& data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if(CheckSize(sizeof(float)))
	{
		memcpy(&data, &m_data[m_readPos], sizeof(float));
		m_readPos += sizeof(float);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(double& data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	if (CheckSize(sizeof(double)))
	{
		memcpy(&data, &m_data[m_readPos], sizeof(double));
		m_readPos += sizeof(double);
	}

	return *this;
}


//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(char* const data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//First extract string length
	uint32_t length = 0;
	*this >> length;

	if((length > 0) && CheckSize(length))
	{
		//Then extract characters
		memcpy(data, &m_data[m_readPos], length);
		data[length] = '\0';

		//Update reading position
		m_readPos += length;
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(std::string& data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

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

TRAP::Network::Packet& TRAP::Network::Packet::operator>>(wchar_t* const data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

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
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

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
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	*this << static_cast<uint8_t>(data);
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const int8_t data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	Append(&data, sizeof(int8_t));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const uint8_t data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	Append(&data, sizeof(uint8_t));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const int16_t data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	int16_t toWrite = data;

	if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(toWrite); //Need to convert to big endian

	Append(&toWrite, sizeof(int16_t));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const uint16_t data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	uint16_t toWrite = data;

	if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(toWrite); //Need to convert to big endian

	Append(&toWrite, sizeof(uint16_t));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const int32_t data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	int32_t toWrite = data;

	if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(toWrite); //Need to convert to big endian

	Append(&toWrite, sizeof(int32_t));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const uint32_t data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	uint32_t toWrite = data;

	if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(toWrite); //Need to convert to big endian

	Append(&toWrite, sizeof(uint32_t));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const int64_t data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	int64_t toWrite = data;

	if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(toWrite); //Need to convert to big endian

	Append(&toWrite, sizeof(int64_t));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const uint64_t data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	uint64_t toWrite = data;

	if(TRAP::Utils::GetEndian() == TRAP::Utils::Endian::Little)
		TRAP::Utils::Memory::SwapBytes(toWrite); //Need to convert to big endian

	Append(&toWrite, sizeof(uint64_t));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(float data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	Append(&data, sizeof(float));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(double data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	Append(&data, sizeof(double));
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const std::string_view data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//First insert string length
	const uint32_t length = static_cast<uint32_t>(data.size());
	*this << length;

	//Then insert characters
	if (length > 0)
		Append(data.data(), length * sizeof(std::string_view::value_type));

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::Packet& TRAP::Network::Packet::operator<<(const std::wstring_view data)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//First insert string length
	const uint32_t length = static_cast<uint32_t>(data.size());
	*this << length;

	//Then insert characters
	if (length > 0)
		Append(data.data(), length * sizeof(std::wstring::value_type));

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::Packet::CheckSize(const std::size_t size) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	m_isValid = m_isValid && (m_readPos + size <= m_data.size());

	return m_isValid;
}

//-------------------------------------------------------------------------------------------------------------------//

const void* TRAP::Network::Packet::OnSend(std::size_t& size)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	size = GetDataSize();
	return GetData();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::Packet::OnReceive(const void* const data, const std::size_t size)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	Append(data, size);
}