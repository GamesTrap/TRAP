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
#include "FTP.h"

#include "Network/IP/IPv4Address.h"
#include "Utils/Time/TimeStep.h"
#include "VFS/VFS.h"

namespace TRAP::Network
{
	class FTP::DataChannel
	{
	public:
		DataChannel(const DataChannel&) = delete;
		DataChannel operator=(const DataChannel&) = delete;
		DataChannel(DataChannel&&) = delete;
		DataChannel operator=(DataChannel&&) = delete;

		explicit DataChannel(FTP& owner);

		Response Open(TransferMode mode);

		void Send(std::istream& stream);

		void Receive(std::ostream& stream);

	private:
		FTP& m_ftp; //Reference to the owner FTP instance
		TCPSocket m_dataSocket{}; //Socket used for data transfers
	};
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response::Response(const Status code, std::string message)
	: m_status(code), m_message(std::move(message))
{
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Network::FTP::Response::IsOK() const
{
	return static_cast<uint32_t>(m_status) < 400;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response::Status TRAP::Network::FTP::Response::GetStatus() const
{
	return m_status;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Network::FTP::Response::GetMessage() const
{
	return m_message;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::DirectoryResponse::DirectoryResponse(const Response& response)
	: Response(response)
{
	if(!IsOK())
		return;

	//Extract the directory from the server response
	const std::string::size_type begin = GetMessage().find('"', 0);
	const std::string::size_type end = GetMessage().find('"', begin + 1);
	m_directory = GetMessage().substr(begin + 1, end - begin - 1);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Network::FTP::DirectoryResponse::GetDirectory() const
{
	return m_directory;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::ListingResponse::ListingResponse(const Response& response, const std::string& data)
	: Response(response)
{
	if(!IsOK())
		return;

	//Fill the array of strings
	std::string::size_type lastPos = 0;
	for(std::string::size_type pos = data.find("\r\n"); pos != std::string::npos; pos = data.find("\r\n", lastPos))
	{
		m_listing.emplace_back(data.substr(lastPos, pos - lastPos));
		lastPos = pos + 2;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::string>& TRAP::Network::FTP::ListingResponse::GetListing() const
{
	return m_listing;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::~FTP()
{
	Disconnect();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::Connect(const IPv4Address& server, const uint16_t port,
                                                         const Utils::TimeStep timeout)
{
	//Connect to the server
	if (m_commandSocket.Connect(server, port, timeout) != Socket::Status::Done)
		return Response(Response::Status::ConnectionFailed);

	//Get the response to the connection
	return GetResponse();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::Login()
{
	return Login("anonymous", "user@trappedgames.de");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::Login(const std::string& name, const std::string& password)
{
	Response response = SendCommand("USER", name);
	if (response.IsOK())
		response = SendCommand("PASS", password);

	return response;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::Disconnect()
{
	//Send the exit command
	Response response = SendCommand("QUIT");
	if (response.IsOK())
		m_commandSocket.Disconnect();

	return response;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::KeepAlive()
{
	return SendCommand("NOOP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::DirectoryResponse TRAP::Network::FTP::GetWorkingDirectory()
{
	return DirectoryResponse(SendCommand("PWD"));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::ListingResponse TRAP::Network::FTP::GetDirectoryListing(const std::string& directory)
{
	//Open a data channel on default port (20) using ASCII transfer mode
	std::ostringstream directoryData;
	DataChannel data(*this);
	Response response = data.Open(TransferMode::ASCII);
	if(response.IsOK())
	{
		//Tell the server to send us the listing
		response = SendCommand("NLST", directory);
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

TRAP::Network::FTP::Response TRAP::Network::FTP::ChangeDirectory(const std::string& directory)
{
	return SendCommand("CWD", directory);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::ParentDirectory()
{
	return SendCommand("CDUP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::CreateDirectory(const std::string& name)
{
	return SendCommand("MKD", name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::DeleteDirectory(const std::string& name)
{
	return SendCommand("RMD", name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::RenameFile(const std::string& file, const std::string& newName)
{
	Response response = SendCommand("RNFR", file);
	if (response.IsOK())
		response = SendCommand("RNTO", newName);

	return response;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::DeleteFile(const std::string& name)
{
	return SendCommand("DELE", name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::Download(const std::string& remoteFile,
                                                          const std::string& localVirtualOrPhysicalPath,
														  const TransferMode mode)
{
	//Open a data channel using the given transfer mode
	DataChannel data(*this);
	Response response = data.Open(mode);
	if(response.IsOK())
	{
		//Tell the server to start the transfer
		response = SendCommand("RETR", remoteFile);
		if(response.IsOK())
		{
			//Extract the filename from the file path
			std::string filename = remoteFile;
			const std::string::size_type pos = filename.find_last_of("/\\");
			if (pos != std::string::npos)
				filename = filename.substr(pos + 1);

			//Make sure the destination path ends with a slash
			std::string path = localVirtualOrPhysicalPath;
			if (!path.empty() && (path[path.size() - 1] != '\\') && (path[path.size() - 1] != '/'))
				path += '/';

			std::filesystem::path physicalPath;
			if (!VFS::ResolveReadPhysicalPath(path, physicalPath, true))
			{
				TP_ERROR(Log::NetworkFTPPrefix, "Couldn't resolve folder path: ", path, "!");
				return Response(Response::Status::InvalidFile);
			}

			//Create the file and truncate it if necessary
			std::ofstream file((physicalPath.string() + filename).c_str(), std::ios::binary | std::ios::trunc);
			if (!file)
				return Response(Response::Status::InvalidFile);

			//Receive the file data
			data.Receive(file);

			//Close the file
			file.close();

			//Get the response from the server
			response = GetResponse();

			//IF the download was unsuccessful, delete the partial file
			if (!response.IsOK())
				std::remove((path + filename).c_str());
		}
	}

	return response;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::Upload(const std::string& localVirtualOrPhysicalFile,
                                                        const std::string& remotePath,
														TransferMode mode, bool append)
{
	std::filesystem::path physicalPath;
	if (!VFS::ResolveReadPhysicalPath(localVirtualOrPhysicalFile, physicalPath, true))
	{
		TP_ERROR(Log::NetworkFTPPrefix, "Couldn't resolve file path: ", localVirtualOrPhysicalFile, "!");
		return Response(Response::Status::InvalidFile);
	}

	if (VFS::FileOrFolderExists(physicalPath, true))
	{
		//Get the contents of the file to send
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TP_ERROR(Log::NetworkFTPPrefix, "Couldn't open file path: ", localVirtualOrPhysicalFile, "!");
			return Response(Response::Status::InvalidFile);
		}

		//Extract the filename from the file path
		std::string filename = localVirtualOrPhysicalFile;
		const std::string::size_type pos = filename.find_last_of("/\\");
		if (pos != std::string::npos)
			filename = filename.substr(pos + 1);

		//Make sure the destination path ends with a slash
		std::string path = remotePath;
		if (!path.empty() && (path[path.size() - 1] != '\\') && (path[path.size() - 1] != '/'))
			path += '/';

		//Open a data channel using the given transfer mode
		DataChannel data(*this);
		Response response = data.Open(mode);
		if (response.IsOK())
		{
			//Tell the server to start the transfer
			response = SendCommand(append ? "APPE" : "STOR", path + filename);
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

	return Response(Response::Status::InvalidFile);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::SendCommand(const std::string& command,
                                                             const std::string& parameter)
{
	//Build the command string
	std::string commandStr;
	if (!parameter.empty())
		commandStr = command + " " + parameter + "\r\n";
	else
		commandStr = command + "\r\n";

	//Send it to the server
	if (m_commandSocket.Send(commandStr.c_str(), commandStr.length()) != Socket::Status::Done)
		return Response(Response::Status::ConnectionClosed);

	//Get the response
	return GetResponse();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::GetResponse()
{
	//We'll use a variable to keep track of the last valid code.
	//It is useful in case of multi-lines responses, because the end of such a response
	//will start by the same code
	uint32_t lastCode = 0;
	bool isInsideMultiline = false;
	std::string message;

	while(true)
	{
		//Receive the response from the server
		std::array<char, 1024> buffer{};
		std::size_t length;

		if(m_receiveBuffer.empty())
		{
			if (m_commandSocket.Receive(buffer.data(), buffer.size(), length) != Socket::Status::Done)
				return Response(Response::Status::ConnectionClosed);
		}
		else
		{
			std::copy(m_receiveBuffer.begin(), m_receiveBuffer.end(), buffer.begin());
			length = m_receiveBuffer.size();
			m_receiveBuffer.clear();
		}

		//There can be several lines inside the received buffer, extract them all
		std::istringstream in(std::string(buffer.data(), length), std::ios::binary);
		while(in)
		{
			//Try to extract the code
			uint32_t code;
			if(in >> code)
			{
				//Extract the separator
				char separator;
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
					message = separator + message + '\n';
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
						{
							std::ostringstream out;
							out << code << separator << line;
							message += out.str();
						}
						else
							message = separator + line;

						//Save the remaining data for the next time GetResponse() is called
						m_receiveBuffer.assign(buffer.data() + static_cast<std::size_t>(in.tellg()),
						                       length - static_cast<std::size_t>(in.tellg()));

						//Return the response code and message
						return Response(static_cast<Response::Status>(code), message);
					}
					else
					{
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
							std::ostringstream out;
							out << code << separator << line << '\n';
							message += out.str();
						}
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

TRAP::Network::FTP::DataChannel::DataChannel(FTP& owner)
	: m_ftp(owner)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Network::FTP::Response TRAP::Network::FTP::DataChannel::Open(const TransferMode mode)
{
	//Open a data connection in active mode (we connect to the server)
	Response response = m_ftp.SendCommand("PASV");
	if(response.IsOK())
	{
		//Extract the connection address and port from the response
		const std::string::size_type begin = response.GetMessage().find_first_of("0123456789");
		if(begin != std::string::npos)
		{
			std::array<uint8_t, 6> data{};
			const std::string str = response.GetMessage().substr(begin);
			std::size_t index = 0;
			for (uint8_t& i : data)
			{
				//Extract the current number
				while(std::isdigit(str[index]))
				{
					i = i * 10 + (str[index] - '0');
					index++;
				}

				//Skip separator
				index++;
			}

			//Reconstruct connection port and address
			const uint16_t port = data[4] * 256 + data[5];
			const IPv4Address address(data[0], data[1], data[2], data[3]);

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
	//Receive data
	std::array<char, 1024> buffer{};
	std::size_t received;
	while(m_dataSocket.Receive(buffer.data(), buffer.size(), received) == Socket::Status::Done)
	{
		stream.write(buffer.data(), static_cast<std::streamsize>(received));

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

		const std::size_t count = static_cast<std::size_t>(stream.gcount());

		if(count > 0)
		{
			//We could read mode data from the stream: send them
			if (m_dataSocket.Send(buffer.data(), count) != Socket::Status::Done)
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