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
#include "HTTP.h"

#include "Utils/String/String.h"

TRAP::Network::HTTP::Request::Request(const std::string uri, const Method method, std::string body)
	: m_method(method), m_majorVersion(1), m_minorVersion(0), m_body(std::move(body))
{
	SetURI(std::move(uri));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::HTTP::Request::SetField(const std::string& field, const std::string& value)
{
	m_fields[Utils::String::ToLower(field)] = value;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::HTTP::Request::SetMethod(const Method method)
{
	m_method = method;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::HTTP::Request::SetURI(std::string uri)
{
	m_uri = std::move(uri);

	//Make sure it starts with a '/'
	if (m_uri.empty() || (m_uri[0] != '/'))
		m_uri.insert(0, "/");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::HTTP::Request::SetHTTPVersion(const uint32_t major, const uint32_t minor)
{
	m_majorVersion = major;
	m_minorVersion = minor;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::HTTP::Request::SetBody(std::string body)
{
	m_body = std::move(body);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Network::HTTP::Request::Prepare() const
{
	std::ostringstream out;

	//Convert the method to its string representation
	std::string method;
	switch (m_method)
	{
	case Method::GET:
		method = "GET";
		break;

	case Method::POST:
		method = "POST";
		break;

	case Method::HEAD:
		method = "HEAD";
		break;

	case Method::PUT:
		method = "PUT";
		break;

	case Method::DELETE:
		method = "DELETE";
		break;

	default:
		method = "GET";
		break;
	}

	//Write the first line containing the request type
	out << method << " " << m_uri << " ";
	out << "HTTP/" << m_majorVersion << '.' << m_minorVersion << "\r\n";

	//Write fields
	for (const auto& [fieldKey, fieldValue] : m_fields)
		out << fieldKey << ": " << fieldValue << "\r\n";

	//Use an extra \r\n to separate the header from the body
	out << "\r\n";

	//Add the body
	out << m_body;

	return out.str();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::HTTP::Request::HasField(const std::string& field) const
{
	return m_fields.find(Utils::String::ToLower(field)) != m_fields.end();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::HTTP::Response::Response()
	: m_status(Status::ConnectionFailed),
	  m_majorVersion(0),
	  m_minorVersion(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Network::HTTP::Response::GetField(const std::string& field) const
{
	const FieldTable::const_iterator it = m_fields.find(Utils::String::ToLower(field));
	if (it != m_fields.end())
		return it->second;

	return "";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::HTTP::Response::Status TRAP::Network::HTTP::Response::GetStatus() const
{
	return m_status;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Network::HTTP::Response::GetMajorHTTPVersion() const
{
	return m_majorVersion;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Network::HTTP::Response::GetMinorHTTPVersion() const
{
	return m_minorVersion;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Network::HTTP::Response::GetBody() const
{
	return m_body;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::HTTP::Response::Parse(const std::string& data)
{
	std::istringstream in(data);

	//Extract the HTTP version from the first line
	std::string version;
	if(in >> version)
	{
		if((version.size() >= 8) && (version[6] == '.') &&
		   (Utils::String::ToLower(version.substr(0, 5)) == "http/") &&
			std::isdigit(version[5]) && std::isdigit(version[7]))
		{
			m_majorVersion = static_cast<uint32_t>(version[5] - '0');
			m_minorVersion = static_cast<uint32_t>(version[7] - '0');
		}
		else
		{
			//Invalid HTTP version
			m_status = Status::InvalidResponse;
			return;
		}
	}

	//Extract the status code from the first line
	int32_t status = 0;
	if (in >> status)
		m_status = static_cast<Status>(status);
	else
	{
		//Invalid status code
		m_status = Status::InvalidResponse;
		return;
	}

	//Ignore the end of the first line
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	//Parse the other lines, which contain fields, one by one
	ParseFields(in);

	m_body.clear();

	//Determine whether the transfer is chunked
	if(Utils::String::ToLower(GetField("transfer-encoding")) != "chunked")
	{
		//Not chunked - just read everything at once
		std::copy(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(), std::back_inserter(m_body));
	}
	else
	{
		//Chunked - have to read chunk by chunk
		std::size_t length = 0;

		//Read all chunks, identified by a chunk-size not being 0
		while(in >> std::hex >> length)
		{
			//Drop the rest of the line (chunk-extension)
			in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			//Copy the actual content data
			std::istreambuf_iterator<char> it(in);
			std::istreambuf_iterator<char> itEnd;
			for (std::size_t i = 0; ((i < length) && (it != itEnd)); i++)
				m_body.push_back(*it++);
		}

		//Drop the rest of the line (chunk-extension)
		in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		//Read all trailers (if present)
		ParseFields(in);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::HTTP::Response::ParseFields(std::istream& in)
{
	std::string line;
	while(std::getline(in, line) && (line.size() > 2))
	{
		const std::string::size_type pos = line.find(": ");
		if(pos != std::string::npos)
		{
			//Extract the field name and its value
			const std::string field = line.substr(0, pos);
			std::string value = line.substr(pos + 2);

			//Remove any trailing \r
			if (!value.empty() && (*value.rbegin() == '\r'))
				value.erase(value.size() - 1);

			//Add the field
			m_fields[Utils::String::ToLower(field)] = value;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::HTTP::HTTP()
	: m_port(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::HTTP::HTTP(const std::string host, const uint16_t port)
	: m_port(0)
{
	SetHost(std::move(host), port);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::HTTP::SetHost(const std::string& host, const uint16_t port)
{
	//Check the protocol
	if(Utils::String::ToLower(host.substr(0, 7)) == "http://")
	{
		//HTTP protocol
		m_hostName = host.substr(7);
		m_port = (port != 0 ? port : 80);
	}
	else if(Utils::String::ToLower(host.substr(0, 8)) == "https://")
	{
		//HTTPS protocol
		//UNSUPPORTED (requires encryption and certificates and stuff...)
		TP_ERROR(Log::NetworkHTTPPrefix, "HTTPS protocol is not supported by TRAP::Network::HTTP");
		m_hostName = "";
		m_port = 0;
	}
	else
	{
		//Undefined protocol - use HTTP
		m_hostName = host;
		m_port = (port != 0 ? port : 80);
	}

	//Remove any trailing '/' from the host name
	if (!m_hostName.empty() && (*m_hostName.rbegin() == '/'))
		m_hostName.erase(m_hostName.size() - 1);

	m_hostIPv6 = IPv6Address(m_hostName);
	m_host = IPv4Address(m_hostName);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::HTTP::Response TRAP::Network::HTTP::SendRequest(const Request& request, Utils::TimeStep timeout)
{
	//First make sure that the request is valid
	//Add missing mandatory fields
	Request toSend(request);
	if (!toSend.HasField("From"))
		toSend.SetField("From", "user@trappedgames.de");
	if (!toSend.HasField("User-Agent"))
		toSend.SetField("User-Agent", "libtrap-network/1.x");
	if (!toSend.HasField("Host"))
		toSend.SetField("Host", m_hostName);
	if(!toSend.HasField("Content-Length"))
	{
		std::ostringstream out;
		out << toSend.m_body.size();
		toSend.SetField("Content-Length", out.str());
	}
	if ((toSend.m_method == Request::Method::POST) && !toSend.HasField("Content-Type"))
		toSend.SetField("Content-Type", "application/x-www-form-urlencoded");
	if ((toSend.m_majorVersion * 10 + toSend.m_minorVersion >= 11) && !toSend.HasField("Connection"))
		toSend.SetField("Connection", "close");

	//Prepare the response
	Response received;

	//Connect the socket to the host
	if(m_connectionIPv6.Connect(m_hostIPv6, m_port, timeout) == Socket::Status::Done)
	{
		//Convert the request to string and send it through the connected socket
		std::string requestStr = toSend.Prepare();

		if (!requestStr.empty())
		{
			//Send it through the socket
			if (m_connectionIPv6.Send(requestStr.c_str(), requestStr.size()) == Socket::Status::Done)
			{
				//Wait for the server's response
				std::string receivedStr;
				std::size_t size = 0;
				std::array<char, 1024> buffer{};
				while (m_connectionIPv6.Receive(buffer.data(), buffer.size(), size) == Socket::Status::Done)
					receivedStr.append(buffer.data(), buffer.data() + size);

				//Build the Response object from the received data
				received.Parse(receivedStr);
			}
		}

		//Close the connection
		m_connectionIPv6.Disconnect();
	}
	else if(m_connection.Connect(m_host, m_port, timeout) == Socket::Status::Done)
	{
		//Convert the request to string and send it through the connected socket
		std::string requestStr = toSend.Prepare();

		if(!requestStr.empty())
		{
			//Send it through the socket
			if(m_connection.Send(requestStr.c_str(), requestStr.size()) == Socket::Status::Done)
			{
				//Wait for the server's response
				std::string receivedStr;
				std::size_t size = 0;
				std::array<char, 1024> buffer{};
				while (m_connection.Receive(buffer.data(), buffer.size(), size) == Socket::Status::Done)
					receivedStr.append(buffer.data(), buffer.data() + size);

				//Build the Response object from the received data
				received.Parse(receivedStr);
			}
		}

		//Close the connection
		m_connection.Disconnect();
	}

	return received;
}