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
#include "FTP.h"

#include "Network/IP/IPv4Address.h"
#include "Utils/String/String.h"
#include "Utils/Time/TimeStep.h"
#include "FileSystem/FileSystem.h"

namespace TRAP::Network
{
	class FTP::DataChannel
	{
	public:
		constexpr DataChannel(const DataChannel&) = delete;
		constexpr DataChannel& operator=(const DataChannel&) = delete;
		constexpr DataChannel(DataChannel&&) noexcept = default;
		constexpr DataChannel& operator=(DataChannel&&) noexcept = delete;
		~DataChannel() = default;

		explicit DataChannel(FTP& owner) noexcept;

		Response Open(TransferMode mode);

		void Send(std::istream& stream);

		void Receive(std::ostream& stream);

	private:
		FTP& m_ftp; //Reference to the owner FTP instance
		TCPSocket m_dataSocket{}; //Socket used for data transfers
	};
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::DirectoryResponse::DirectoryResponse(const Response& response)
	: Response(response)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	if(!IsOK())
		return;

	//Extract the directory from the server response
	const std::string::size_type begin = GetMessage().find('"', 0);
	const std::string::size_type end = GetMessage().find('"', begin + 1);
	m_directory = GetMessage().substr(begin + 1, end - begin - 1);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::filesystem::path& TRAP::Network::FTP::DirectoryResponse::GetDirectory() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_directory;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::~FTP()
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	const auto response = Disconnect();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::Connect(const IPv4Address& server, const u16 port,
                                                         const Utils::TimeStep timeout)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	//Connect to the server
	if (m_commandSocket.Connect(server, port, timeout) != Socket::Status::Done)
		return Response(Response::Status::ConnectionFailed);

	//Get the response to the connection
	return GetResponse();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::Login()
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	return Login("anonymous", "user@trappedgames.de");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::Login(const std::string& name, const std::string& password)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	Response response = SendCommand("USER", name);
	if (response.IsOK())
		response = SendCommand("PASS", password);

	return response;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::Disconnect()
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	//Send the exit command
	Response response = SendCommand("QUIT");
	if (response.IsOK())
		m_commandSocket.Disconnect();

	return response;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::KeepAlive()
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	return SendCommand("NOOP");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Network::FTP::DirectoryResponse TRAP::Network::FTP::GetWorkingDirectory()
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	return DirectoryResponse(SendCommand("PWD"));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Network::FTP::ListingResponse TRAP::Network::FTP::GetDirectoryListing(const std::filesystem::path& directory)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	//Open a data channel on default port (20) using ASCII transfer mode
	std::ostringstream directoryData;
	DataChannel data(*this);
	Response response = data.Open(TransferMode::ASCII);
	if(response.IsOK())
	{
		//Tell the server to send us the listing
		response = SendCommand("NLST", directory.string());
		if(response.IsOK())
		{
			//Receive the listing
			data.Receive(directoryData);

			//Get the response from the server
			response = GetResponse();
		}
	}

	return ListingResponse(response, directoryData.str());
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::ChangeDirectory(const std::filesystem::path& directory)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	return SendCommand("CWD", directory.string());
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::ParentDirectory()
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	return SendCommand("CDUP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::CreateDirectory(const std::filesystem::path& name)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	return SendCommand("MKD", name.string());
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::DeleteDirectory(const std::filesystem::path& name)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	return SendCommand("RMD", name.string());
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::RenameFile(const std::filesystem::path& file,
   															const std::filesystem::path& newName)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	Response response = SendCommand("RNFR", file.string());
	if (response.IsOK())
		response = SendCommand("RNTO", newName.string());

	return response;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::DeleteFile(const std::filesystem::path& name)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	return SendCommand("DELE", name.string());
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::Download(const std::filesystem::path& remoteFile,
                                                          const std::filesystem::path& path,
														  const TransferMode mode)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	//Open a data channel using the given transfer mode
	DataChannel data(*this);
	Response response = data.Open(mode);
	if(response.IsOK())
	{
		//Tell the server to start the transfer
		response = SendCommand("RETR", remoteFile.string());
		if(response.IsOK())
		{
			//Extract the filename from the file path
			const auto filename = TRAP::FileSystem::GetFileNameWithEnding(remoteFile);
			if(!filename)
			{
				TP_ERROR(Log::NetworkFTPPrefix, "Couldn't get file name from file path: ", remoteFile, "!");
				return Response(Response::Status::InvalidFile);
			}

			//Create missing directories if any
			if(!TRAP::FileSystem::Exists(path) && !TRAP::FileSystem::CreateFolder(path))
				return Response(Response::Status::InvalidFile);

			//Create the file and truncate it if necessary
			const std::filesystem::path filePath = path / *filename;
			std::ofstream file(filePath, std::ios::binary | std::ios::trunc);
			if (!file.is_open() || !file.good())
			{
				TP_ERROR(Log::NetworkFTPPrefix, "Couldn't open file path: ", filePath, "!");
				return Response(Response::Status::InvalidFile);
			}

			//Receive the file data
			data.Receive(file);

			//Close the file
			file.close();

			//Get the response from the server
			response = GetResponse();

			//If the download was unsuccessful, delete the partial file
			if (!response.IsOK())
				TRAP::FileSystem::Delete(filePath);
		}
	}

	return response;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::Upload(const std::filesystem::path& localFile,
                                                        const std::filesystem::path& remotePath,
														const TransferMode mode, const bool append)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	if(!FileSystem::Exists(localFile))
		return Response(Response::Status::InvalidFile);

	//Get the contents of the file to send
	std::ifstream file(localFile, std::ios::binary);
	if (!file.is_open() || !file.good())
	{
		TP_ERROR(Log::NetworkFTPPrefix, "Couldn't open file path: ", localFile, "!");
		return Response(Response::Status::InvalidFile);
	}

	//Extract the filename from the file path
	const auto filename = TRAP::FileSystem::GetFileNameWithEnding(localFile);
	if(!filename)
	{
		TP_ERROR(Log::NetworkFTPPrefix, "Couldn't get file name from file path: ", localFile, "!");
		return Response(Response::Status::InvalidFile);
	}

	//Open a data channel using the given transfer mode
	DataChannel data(*this);
	Response response = data.Open(mode);
	if (response.IsOK())
	{
		//Tell the server to start the transfer
		response = SendCommand(append ? "APPE" : "STOR", (remotePath / *filename).string());
		if (response.IsOK())
		{
			//Send the file data
			data.Send(file);

			//Get the response from the server
			response = GetResponse();
		}
	}

	return response;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::SendCommand(const std::string& command,
                                                             const std::string& parameter)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	//Build the command string
	std::string commandStr;
	if (!parameter.empty())
		commandStr = fmt::format("{} {}\r\n", command, parameter);
	else
		commandStr = fmt::format("{}\r\n", command);

	//Send it to the server
	if (m_commandSocket.Send(commandStr.c_str(), commandStr.length()) != Socket::Status::Done)
		return Response(Response::Status::ConnectionClosed);

	//Get the response
	return GetResponse();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Network::FTP::Response TRAP::Network::FTP::GetResponse()
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	//We'll use a variable to keep track of the last valid code.
	//It is useful in case of multi-lines responses, because the end of such a response
	//will start by the same code
	u32 lastCode = 0;
	bool isInsideMultiline = false;
	std::string message;

	while(true)
	{
		//Receive the response from the server
		std::array<char, 1024> buffer{};
		usize length = 0;

		if(m_receiveBuffer.empty())
		{
			if (m_commandSocket.Receive(buffer.data(), buffer.size(), length) != Socket::Status::Done)
				return Response(Response::Status::ConnectionClosed);
		}
		else
		{
			std::ranges::copy(m_receiveBuffer, buffer.begin());
			length = m_receiveBuffer.size();
			m_receiveBuffer.clear();
		}

		//There can be several lines inside the received buffer, extract them all
		std::istringstream in(std::string(buffer.data(), length), std::ios::binary);
		while(in)
		{
			//Try to extract the code
			u32 code = 0;
			if(in >> code)
			{
				//Extract the separator
				char separator = ' ';
				in.get(separator);

				//The '-' character means a multiline response
				if((separator == '-') && !isInsideMultiline)
				{
					//Set the multiline flag
					isInsideMultiline = true;

					//Keep track of the code
					if (lastCode == 0)
						lastCode = code;

					//Extract the line
					std::getline(in, message);

					//Remove the ending '\r' (all lines are terminated by "\r\n")
					message.erase(message.length() - 1);
					message = fmt::format("{}{}\n", separator, message);
				}
				else
				{
					//We must make sure that the code is the same, otherwise it means
					//we haven't reached the end of the multiline response
					if((separator != '-') && ((code == lastCode) || (lastCode == 0)))
					{
						//Extract the line
						std::string line;
						std::getline(in, line);

						//Remove the ending '\r' (all lines are terminated by "\r\n")
						line.erase(line.length() - 1);

						//Append it to the message
						if (code == lastCode)
							message += fmt::format("{}{}{}", code, separator, line);
						else
							message = separator + line;

						//Save the remaining data for the next time GetResponse() is called
						m_receiveBuffer.assign(buffer.data() + static_cast<usize>(in.tellg()),
						                       length - static_cast<usize>(in.tellg()));

						//Return the response code and message
						return Response(static_cast<Response::Status>(code), message);
					}

					//The line we just read was actually not a response,
					//only a new part of the current multiline response

					//Extract the line
					std::string line;
					std::getline(in, line);

					if(!line.empty())
					{
						//Remove the ending '\r' (all lines are terminated by "\r\n")
						line.erase(line.length() - 1);

						//Append it to the current message
						message += fmt::format("{}{}{}\n", code, separator, line);
					}
				}
			}
			else if(lastCode != 0)
			{
				//It seems we are in the middle of a multiline response

				//Clear the error bits of the stream
				in.clear();

				//Extract the line
				std::string line;
				std::getline(in, line);

				if(!line.empty())
				{
					//remove the ending '\r' (all lines are terminated by "\r\n")
					line.erase(line.length() - 1);

					//Append it to the current message
					message += line + '\n';
				}
			}
			else
			{
				//Error: cannot extract the code, and we are not in a multiline response
				return Response(Response::Status::InvalidResponse);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::DataChannel::DataChannel(FTP& owner) noexcept
	: m_ftp(owner)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::DataChannel::Open(const TransferMode mode)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	//Open a data connection in active mode (we connect to the server)
	Response response = m_ftp.SendCommand("PASV");
	if(response.IsOK())
	{
		//Extract the connection address and port from the response
		const std::string::size_type begin = response.GetMessage().find_first_of("0123456789");
		if(begin != std::string::npos)
		{
			std::array<u8, 6> data{};
			const std::string str = response.GetMessage().substr(begin);
			usize index = 0;
			for (u8& i : data)
			{
				//Extract the current number
				while(Utils::String::IsDigit(str[index]))
				{
					i = NumericCast<u8>(NumericCast<u8>(i * 10u) + NumericCast<u8>(str[index] - '0'));
					index++;
				}

				//Skip separator
				index++;
			}

			//Reconstruct connection port and address
			const u16 port = NumericCast<u16>(std::get<4>(data) * 256) + std::get<5>(data);
			const IPv4Address address(std::get<0>(data), std::get<1>(data), std::get<2>(data), std::get<3>(data));

			//Connect the data channel to the server
			if(m_dataSocket.Connect(address, port) == Socket::Status::Done)
			{
				//Translate the transfer mode to the corresponding FTP parameter
				std::string modeStr;
				switch(mode)
				{
				case TransferMode::Binary:
					modeStr = 'I';
					break;

				case TransferMode::ASCII:
					modeStr = 'A';
					break;

				case TransferMode::EBCDIC:
					modeStr = 'E';
					break;

				default:
					modeStr = "";
					break;
				}

				//Set the transfer mode
				response = m_ftp.SendCommand("TYPE", modeStr);
			}
			else
			{
				//Failed to connect to the server
				response = Response(Response::Status::ConnectionFailed);
			}
		}
	}

	return response;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::FTP::DataChannel::Receive(std::ostream& stream)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	//Receive data
	std::array<char, 1024> buffer{};
	usize received = 0;
	while(m_dataSocket.Receive(buffer.data(), buffer.size(), received) == Socket::Status::Done)
	{
		stream.write(buffer.data(), NumericCast<std::streamsize>(received));

		if(!stream.good())
		{
			TP_ERROR(Log::NetworkFTPPrefix, "Writing to the file has failed!");
			break;
		}
	}

	//Close the data socket
	m_dataSocket.Disconnect();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Network::FTP::DataChannel::Send(std::istream& stream)
{
	ZoneNamedC(__tracy, tracy::Color::Azure, (GetTRAPProfileSystems() & ProfileSystems::Network) != ProfileSystems::None);

	//Send data
	std::array<char, 1024> buffer{};

	while(true)
	{
		//Read some data from the stream
		stream.read(buffer.data(), buffer.size());

		if(!stream.good() && !stream.eof())
		{
			TP_ERROR(Log::NetworkFTPPrefix, "Reading from the file has failed!");
			break;
		}

		const i64 count = stream.gcount();

		if(count > 0)
		{
			//We could read mode data from the stream: send them
			if (m_dataSocket.Send(buffer.data(), NumericCast<usize>(count)) != Socket::Status::Done)
				break;
		}
		else
		{
			//No more data: exit the loop
			break;
		}
	}

	//Close the data socket
	m_dataSocket.Disconnect();
}