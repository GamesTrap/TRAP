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

#include "TRAPPCH.h"
#include "SocketSelector.h"

#include "Socket.h"
#include "SocketHandle.h"
#include "SocketImpl.h"
#include "Utils/Time/TimeStep.h"

#ifdef TRAP_PLATFORM_WINDOWS
#define far
#endif /*TRAP_PLATFORM_WINDOWS*/

namespace TRAP::Network
{
	struct SocketSelector::SocketSelectorImpl
	{
		fd_set AllSockets;   //Set containing all the sockets handles
		fd_set SocketsReady; //Set containing handles of the sockets that are ready
		int32_t MaxSockets;  //Maximum socket handle
		int32_t SocketCount; //Number of socket handles
	};
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::SocketSelector::SocketSelector()
	: m_impl(TRAP::MakeScope<SocketSelectorImpl>())
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	Clear();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::SocketSelector::SocketSelector(const SocketSelector& copy)
	: m_impl(TRAP::MakeScope<SocketSelectorImpl>(*copy.m_impl))
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::SocketSelector::SocketSelector(SocketSelector&& other) noexcept
	: m_impl(std::move(other.m_impl))
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::SocketSelector& TRAP::Network::SocketSelector::operator=(SocketSelector&& other) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	m_impl = std::move(other.m_impl);
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::SocketSelector::Add(Socket& socket)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	const SocketHandle handle = socket.GetHandle();
	if(handle != INTERNAL::Network::SocketImpl::InvalidSocket())
	{
#ifdef TRAP_PLATFORM_WINDOWS
		if(m_impl->SocketCount >= FD_SETSIZE)
		{
			TP_ERROR(Log::NetworkSocketPrefix, "The socket can't be added to the selector because the ",
				"selector is full. This is a limitation of your operating system's FD_SETSIZE setting.");
			return;
		}

		if (FD_ISSET(handle, &m_impl->AllSockets))
			return;

		m_impl->SocketCount++;
#else
		if(handle >= FD_SETSIZE)
		{
			TP_ERROR(Log::NetworkSocketPrefix, "The socket can't be added to the selector because its ",
				"ID is too high. This is a limitation of your operating system's FD_SETSIZE setting.");
			return;
		}

		//SocketHandle is an int in POSIX
		m_impl->MaxSockets = std::max(m_impl->MaxSockets, handle);
#endif

		FD_SET(handle, &m_impl->AllSockets);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::SocketSelector::Remove(Socket& socket) const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	const SocketHandle handle = socket.GetHandle();
	if(handle != INTERNAL::Network::SocketImpl::InvalidSocket())
	{
#ifdef TRAP_PLATFORM_WINDOWS
		if (!FD_ISSET(handle, &m_impl->AllSockets))
			return;

		m_impl->SocketCount--;
#else
		if (handle >= FD_SETSIZE)
			return;
#endif

		FD_CLR(handle, &m_impl->AllSockets);
		FD_CLR(handle, &m_impl->SocketsReady);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::SocketSelector::Clear() const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	FD_ZERO(&m_impl->AllSockets);
	FD_ZERO(&m_impl->SocketsReady);

	m_impl->MaxSockets = 0;
	m_impl->SocketCount = 0;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::SocketSelector::Wait(const Utils::TimeStep timeout) const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	//Setup the timeout
	timeval time{};
	time.tv_sec = static_cast<time_t>(timeout.GetSeconds());
	time.tv_usec = static_cast<time_t>(timeout.GetSeconds());

	//initialize the set that will contain the sockets that are ready
	m_impl->SocketsReady = m_impl->AllSockets;

	//Wait until one of the sockets is ready for reading, or timeout is reached
	//The first parameter is ignored on Windows
	const int32_t count = select(m_impl->MaxSockets + 1, &m_impl->SocketsReady, nullptr, nullptr,
	                             timeout != Utils::TimeStep(0.0f) ? &time : nullptr);

	return count > 0;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Network::SocketSelector::IsReady(Socket& socket) const
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	const SocketHandle handle = socket.GetHandle();
	if(handle != INTERNAL::Network::SocketImpl::InvalidSocket())
	{
#ifndef TRAP_PLATFORM_WINDOWS
		if (handle >= FD_SETSIZE)
			return false;
#else
		return FD_ISSET(handle, &m_impl->SocketsReady) != 0;
#endif
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::SocketSelector& TRAP::Network::SocketSelector::operator=(const SocketSelector& right)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Network);

	SocketSelector temp(right);

	std::swap(m_impl, temp.m_impl);

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_WINDOWS
#undef far
#endif /*TRAP_PLATFORM_WINDOWS*/