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

#ifndef TRAP_FTP_H
#define TRAP_FTP_H

#include <filesystem>
#include <string>

#include <fmt/ostream.h>

#include "Core/Backports.h"
#include "Network/Sockets/TCPSocket.h"
#include "TRAP_Assert.h"

namespace TRAP::Network
{
	class IPv4Address;

	/// @brief A FTP client.
	/// @note Only supports IPv4.
	class FTP
	{
	public:
		/// @brief Enumeration of transfer modes
		enum class TransferMode
		{
			Binary, //Binary mode (file is transferred as a sequence of bytes)
			ASCII,  //Text mode using ASCII encoding
			EBCDIC  //Text mode using EBCDIC encoding
		};

		class Response
		{
		public:
			/// @brief Status codes possibly returned by a FTP response.
			enum class Status
			{
				//1XX: The requested action is being initialized,
				//     expect another reply before proceeding with a new command
				RestartMarkerReply          = 110, //Restart marker reply
				ServiceReadySoon            = 120, //Service ready in N minutes
				DataConnectionAlreadyOpened = 125, //Data connection already opened, transfer starting
				OpeningDataConnection       = 150, //File status ok, about to open data connection

				//2XX: The requested action has been successfully completed
				OK                    = 200, //Command OK
				PointlessCommand      = 202, //Command not implemented
				SystemStatus          = 211, //System status, or system help reply
				DirectoryStatus       = 212, //Directory status
				FileStatus            = 213, //File status
				HelpMessage           = 214, //Help message
				SystemType            = 215, //NAME system type, where NAME is an official system name from the list in the Assigned Numbers doc
				ServiceReady          = 220, //Service ready for new user
				ClosingConnection     = 221, //Service closing control connection
				DataConnectionOpened  = 225, //Data connection open, no transfer in progress
				ClosingDataConnection = 226, //Closing data connection, requested file action successful
				EnteringPassiveMode   = 227, //Entering passive mode
				LoggedIn              = 230, //User logged in, proceed. Logged out if appropriate
				FileActionOK          = 250, //Requested file action OK
				DirectoryOK           = 257, //PATHNAME created

				//3XX: The command has been accepted, but the requested action
				//     is dormant, pending receipt of further information
				NeedPassword       = 331, //User name OK, need password
				NeedAccountToLogIn = 332, //Need account for login
				NeedInformation    = 350, //Requested file action pending further information

				//4XX: The command was not accepted and the requested action did not take place,
				//     but the error condition is temporary and the action may be requested again
				ServiceUnavailable        = 421, //Service not available, closing control connection
				DataConnectionUnavailable = 425, //Can't open data connection
				TransferAborted           = 426, //Connection closed, transfer aborted
				FileActionAborted         = 450, //Requested file action not taken
				LocalError                = 451, //Requested action aborted, local error in processing
				InsufficientStorageSpace  = 452, //Requested action not taken; insufficient storage space in system, file unavailable

				//5XX: The command was not accepted and
				//     the requested action did not take place
				CommandUnknown          = 500, //Syntax error, command unrecognized
				ParametersUnknown       = 501, //Syntax error in parameters or arguments
				CommandNotImplemented   = 502, //Command not implemented
				BadCommandSequence      = 503, //Bad sequence of commands
				ParameterNotImplemented = 504, //Command not implemented for that parameter
				NotLoggedIn             = 530, //Not logged in
				NeedAccountToStore      = 532, //Need account for storing files
				FileUnavailable         = 550, //Requested action not taken, file unavailable
				PageTypeUnknown         = 551, //Requested action aborted, page type unknown
				NotEnoughMemory         = 552, //Requested file action aborted, exceeded storage allocation
				FilenameNotAllowed      = 553, //Requested action not taken, file name not allowed

				//10XX: Custom codes
				InvalidResponse  = 1000, //Not part of the FTP standard, generated by TRAP when a received response cannot be parsed
				ConnectionFailed = 1001, //Not part of the FTP standard, generated by TRAP when the low-level socket connection with the server fails
				ConnectionClosed = 1002, //Not part of the FTP standard, generated by TRAP when the low-level socket connection is unexpectedly closed
				InvalidFile      = 1003  //Not part of the FTP standard, generated by TRAP when a local file cannot be read or written
			};

			/// @brief This constructor is used by the FTP client to build the response.
			/// @param code Response status code.
			/// @param message Response message.
			constexpr explicit Response(Status code = Status::InvalidResponse, std::string message = "") noexcept;

			/// @brief Copy constructor.
			constexpr Response(const Response&) = default;
			/// @brief Copy assignment operator.
			constexpr Response& operator=(const Response&) = default;
			/// @brief Move constructor.
			constexpr Response(Response&&) noexcept = default;
			/// @brief Move assignment operator.
			constexpr Response& operator=(Response&&) noexcept = default;

			/// @brief Destructor.
			constexpr ~Response() = default;

			/// @brief Check if the status code means a success.
			///
			/// This function is defined for convenience, it is
			/// equivalent to testing if the status code is < 400.
			/// @return True if the status is a success, false if it is a failure.
			[[nodiscard]] constexpr bool IsOK() const noexcept;

			/// @brief Get the status code of the response.
			/// @return Status code.
			[[nodiscard]] constexpr Status GetStatus() const noexcept;

			/// @brief Get the full message contained in the response.
			/// @return The response message.
			[[nodiscard]] constexpr std::string GetMessage() const noexcept;

		private:
			Status m_status; //Status code returned from the server
			std::string m_message; //Last message received from the server
		};

		/// @brief Specialization of FTP response returning a directory.
		class DirectoryResponse : public Response
		{
		public:
			/// @brief Constructor.
			/// @param response Source response.
			explicit DirectoryResponse(const Response& response);

			/// @brief Copy constructor.
			DirectoryResponse(const DirectoryResponse&) = default;
			/// @brief Copy assignment operator.
			DirectoryResponse& operator=(const DirectoryResponse&) = default;
			/// @brief Move constructor.
			DirectoryResponse(DirectoryResponse&&) noexcept = default;
			/// @brief Move assignment operator.
			DirectoryResponse& operator=(DirectoryResponse&&) noexcept = default;

			/// @brief Destructor.
			~DirectoryResponse() = default;

			/// @brief Get the directory returned in the response.
			/// @return Directory name.
			[[nodiscard]] const std::filesystem::path& GetDirectory() const noexcept;

		private:
			std::filesystem::path m_directory; //Directory extracted from the response message
		};

		/// @brief Specialization of FTP response returning a filename listing.
		class ListingResponse : public Response
		{
		public:
			/// @brief Constructor.
			/// @param response Source response.
			/// @param data Data containing the raw listing.
			constexpr ListingResponse(const Response& response, std::string_view data);

			/// @brief Copy constructor.
			ListingResponse(const ListingResponse&) = default;
			/// @brief Copy assignment operator.
			ListingResponse& operator=(const ListingResponse&) = default;
			/// @brief Move constructor.
			ListingResponse(ListingResponse&&) noexcept = default;
			/// @brief Move assignment operator.
			ListingResponse& operator=(ListingResponse&&) noexcept = default;

			/// @brief Destructor.
			~ListingResponse() = default;

			/// @brief Return the array of directory/file names.
			/// @return Array containing the requested listing.
			[[nodiscard]] constexpr const std::vector<std::filesystem::path>& GetListing() const noexcept;

		private:
			std::vector<std::filesystem::path> m_listing; //Directory/file names extracted from the data
		};

		/// @brief Constructor.
		FTP() noexcept = default;

		/// @brief Automatically closes the connection with the server if it is still opened.
		~FTP();

		/// @brief Copy constructor.
		consteval FTP(const FTP&) noexcept = delete;

		/// @brief Copy assignment operator.
		consteval FTP& operator=(const FTP&) noexcept = delete;

		/// @brief Move constructor.
		constexpr FTP(FTP&&) noexcept = default;

		/// @brief Move assignment operator.
		FTP& operator=(FTP&&) noexcept = default;

		/// @brief Connect to the specified FTP server.
		///
		/// The port has a default value of 21, which is the standard
		/// port used by the FTP protocol.
		/// You shouldn't use a different value, unless you really know what you do.
		/// This function tries to connect to the server so it may take
		/// a while to complete, especially if the server is not
		/// reachable.
		/// To avoid blocking your application for too long,
		/// you can use a timeout.
		/// The default value, Time::Zero, means that the
		/// system timeout will be used (which is usually pretty long).
		/// @param server Name or address of the FTP server to connect to.
		/// @param port Port used for the connection.
		/// @param timeout Maximum time to wait.
		/// @return Server response to the request.
		[[nodiscard]] Response Connect(const IPv4Address& server, u16 port = 21,
		                               Utils::TimeStep timeout = Utils::TimeStep(0.0f));

		/// @brief Close the connection with the server.
		/// @return Server response to the request.
		[[nodiscard]] Response Disconnect();

		/// @brief Log in using an anonymous account.
		///
		/// Logging in is mandatory after connecting to the server.
		/// Users that are not logged in cannot perform any operation.
		/// @return Server response to the request.
		[[nodiscard]] Response Login();

		/// @brief Log in using a username and a password.
		///
		/// Logging in is mandatory after connecting to the server.
		/// Users that are not logged in cannot perform any operation.
		/// @param name User name.
		/// @param password Password.
		/// @return Server response to the request.
		[[nodiscard]] Response Login(const std::string& name, const std::string& password);

		/// @brief Send a null command to keep the connection alive.
		///
		/// This command is useful because the server may close the
		/// connection automatically if no command is sent.
		/// @return Server response to the request.
		[[nodiscard]] Response KeepAlive();

		/// @brief Get the current working directory.
		///
		/// The working directory is the root path for subsequent
		/// operations involving directories and/or filenames.
		/// @return Server response to the request.
		[[nodiscard]] [[nodiscard]] DirectoryResponse GetWorkingDirectory();

		/// @brief Get the contents of the given directory.
		///
		/// This function retrieves the sub-directories and files
		/// contained in the given directory.
		/// It is not recursive.
		/// The directory parameter is relative to the current working directory.
		/// @param directory Directory to list.
		/// @return Server response to the request.
		[[nodiscard]] [[nodiscard]] ListingResponse GetDirectoryListing(const std::filesystem::path& directory = "");

		/// @brief Change the current working directory.
		///
		/// The new directory must be relative to the current one.
		/// @param directory New working directory.
		/// @return Server response to the request.
		[[nodiscard]] Response ChangeDirectory(const std::filesystem::path& directory);

		/// @brief Go to the parent directory of the current one.
		/// @return Server response to the request.
		[[nodiscard]] Response ParentDirectory();

		/// @brief Create a new directory.
		///
		/// The new directory is created as a child of the current working directory.
		/// @param name Name of the directory to create.
		/// @return Server response to the request.
		[[nodiscard]] Response CreateDirectory(const std::filesystem::path& name);

		/// @brief Remove an existing directory.
		///
		/// The directory to remove must be relative to the
		/// current working directory.
		/// Use this function with caution, the directory will
		/// be removed permanently!
		/// @param name Name of the directory to remove.
		/// @return Server response to the request.
		[[nodiscard]] Response DeleteDirectory(const std::filesystem::path& name);

		/// @brief Rename an existing file.
		///
		/// The filenames must be relative to the current working directory.
		/// @param file File to rename.
		/// @param newName New name of the file.
		/// @return Server response to the request.
		[[nodiscard]] Response RenameFile(const std::filesystem::path& file, const std::filesystem::path& newName);

		/// @brief Remove an existing file.
		///
		/// The file name must be relative to the current working directory.
		/// Use this function with caution, the file will be
		/// removed permanently!
		/// @param name File to remove.
		/// @return Server response to the request.
		[[nodiscard]] Response DeleteFile(const std::filesystem::path& name);

		/// @brief Download a file from the server.
		///
		/// The filename of the distant file is relative to the
		/// current working directory of the server, and the local
		/// destination path is relative to the current directory
		/// of you application.
		/// If a file with the same filename as the distant file
		/// already exists in the local destination path, it will
		/// be overwritten.
		/// @param remoteFile Filename of the distant file to download.
		/// @param path The directory in which to put the file on the local computer.
		/// @param mode Transfer mode.
		/// @return Server response to the request.
		[[nodiscard]] Response Download(const std::filesystem::path& remoteFile,
										const std::filesystem::path& path,
		                                TransferMode mode = TransferMode::Binary);

		/// @brief Upload a file to the server.
		///
		/// The name of the local file is relative to the current
		/// working directory of your application, and the
		/// remote path is relative to the current directory of the
		/// FTP server.
		///
		/// The append parameter controls whether the remote file is
		/// appended to or overwritten if it already exists.
		/// @param localFile Path of the local file to upload.
		/// @param remotePath The directory in which to put the file on the server.
		/// @param mode Transfer mode.
		/// @param append Pass true to append to or false to overwrite the remote file if it already exists.
		/// @return Server response to the request.
		[[nodiscard]] Response Upload(const std::filesystem::path& localFile,
		                			  const std::filesystem::path& remotePath,
		                			  TransferMode mode = TransferMode::Binary, bool append = false);

		/// @brief Send a command to the FTP server.
		///
		/// While the most often used commands are provided as member
		/// functions in the TRAP::Network::FTP class, this method can be used
		/// to send any FTP command to the server.
		/// If the command requires one or more parameters, they can be specified
		/// in parameter.
		/// If the server returns information, you
		/// can extract it from the response using TRAP::Network::FTP::Response::GetMessage().
		/// @param command Command to send.
		/// @param parameter Command parameter.
		/// @return Server response to the request.
		[[nodiscard]] Response SendCommand(const std::string& command, const std::string& parameter = "");

	private:
		/// @brief Receive a response from the server.
		///
		/// This function must be called after each call to
		/// SendCommand that expects a response.
		/// @return Server response to the request.
		[[nodiscard]] Response GetResponse();

		/// @brief Utility class for exchanging data with the server on the data channel.
		class DataChannel;

		friend class DataChannel;

		TCPSocket m_commandSocket; //Socket holding the control connection with the server
		std::string m_receiveBuffer; //Received command data that is yet to be processed
	};
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Network::FTP::Response::Status>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Network::FTP::Response::Status status,
	                                            fmt::format_context& ctx)
    {
		std::string enumStr{};

		switch(status)
		{
			case TRAP::Network::FTP::Response::Status::RestartMarkerReply:
				enumStr = "RestartMarkerReply";
				break;
			case TRAP::Network::FTP::Response::Status::ServiceReadySoon:
				enumStr = "ServiceReadySoon";
				break;
			case TRAP::Network::FTP::Response::Status::DataConnectionAlreadyOpened:
				enumStr = "DataConnectionAlreadyOpened";
				break;
			case TRAP::Network::FTP::Response::Status::OpeningDataConnection:
				enumStr = "OpeningDataConnection";
				break;

			case TRAP::Network::FTP::Response::Status::OK:
				enumStr = "OK";
				break;
			case TRAP::Network::FTP::Response::Status::PointlessCommand:
				enumStr = "PointlessCommand";
				break;
			case TRAP::Network::FTP::Response::Status::SystemStatus:
				enumStr = "SystemStatus";
				break;
			case TRAP::Network::FTP::Response::Status::DirectoryStatus:
				enumStr = "DirectoryStatus";
				break;
			case TRAP::Network::FTP::Response::Status::FileStatus:
				enumStr = "FileStatus";
				break;
			case TRAP::Network::FTP::Response::Status::HelpMessage:
				enumStr = "HelpMessage";
				break;
			case TRAP::Network::FTP::Response::Status::SystemType:
				enumStr = "SystemType";
				break;
			case TRAP::Network::FTP::Response::Status::ServiceReady:
				enumStr = "ServiceReady";
				break;
			case TRAP::Network::FTP::Response::Status::ClosingConnection:
				enumStr = "ClosingConnection";
				break;
			case TRAP::Network::FTP::Response::Status::DataConnectionOpened:
				enumStr = "DataConnectionOpened";
				break;
			case TRAP::Network::FTP::Response::Status::ClosingDataConnection:
				enumStr = "ClosingDataConnection";
				break;
			case TRAP::Network::FTP::Response::Status::EnteringPassiveMode:
				enumStr = "EnteringPassiveMode";
				break;
			case TRAP::Network::FTP::Response::Status::LoggedIn:
				enumStr = "LoggedIn";
				break;
			case TRAP::Network::FTP::Response::Status::FileActionOK:
				enumStr = "FileActionOK";
				break;
			case TRAP::Network::FTP::Response::Status::DirectoryOK:
				enumStr = "DirectoryOK";
				break;

			case TRAP::Network::FTP::Response::Status::NeedPassword:
				enumStr = "NeedPassword";
				break;
			case TRAP::Network::FTP::Response::Status::NeedAccountToLogIn:
				enumStr = "NeedAccountToLogIn";
				break;
			case TRAP::Network::FTP::Response::Status::NeedInformation:
				enumStr = "NeedInformation";
				break;

			case TRAP::Network::FTP::Response::Status::ServiceUnavailable:
				enumStr = "ServiceUnavailable";
				break;
			case TRAP::Network::FTP::Response::Status::DataConnectionUnavailable:
				enumStr = "DataConnectionUnavailable";
				break;
			case TRAP::Network::FTP::Response::Status::TransferAborted:
				enumStr = "TransferAborted";
				break;
			case TRAP::Network::FTP::Response::Status::FileActionAborted:
				enumStr = "FileActionAborted";
				break;
			case TRAP::Network::FTP::Response::Status::LocalError:
				enumStr = "LocalError";
				break;
			case TRAP::Network::FTP::Response::Status::InsufficientStorageSpace:
				enumStr = "InsufficientStorageSpace";
				break;

			case TRAP::Network::FTP::Response::Status::CommandUnknown:
				enumStr = "CommandUnknown";
				break;
			case TRAP::Network::FTP::Response::Status::ParametersUnknown:
				enumStr = "ParametersUnknown";
				break;
			case TRAP::Network::FTP::Response::Status::CommandNotImplemented:
				enumStr = "CommandNotImplemented";
				break;
			case TRAP::Network::FTP::Response::Status::BadCommandSequence:
				enumStr = "BadCommandSequence";
				break;
			case TRAP::Network::FTP::Response::Status::ParameterNotImplemented:
				enumStr = "ParameterNotImplemented";
				break;
			case TRAP::Network::FTP::Response::Status::NotLoggedIn:
				enumStr = "NotLoggedIn";
				break;
			case TRAP::Network::FTP::Response::Status::NeedAccountToStore:
				enumStr = "NeedAccountToStore";
				break;
			case TRAP::Network::FTP::Response::Status::FileUnavailable:
				enumStr = "FileUnavailable";
				break;
			case TRAP::Network::FTP::Response::Status::PageTypeUnknown:
				enumStr = "PageTypeUnknown";
				break;
			case TRAP::Network::FTP::Response::Status::NotEnoughMemory:
				enumStr = "NotEnoughMemory";
				break;
			case TRAP::Network::FTP::Response::Status::FilenameNotAllowed:
				enumStr = "FilenameNotAllowed";
				break;

			case TRAP::Network::FTP::Response::Status::InvalidResponse:
				enumStr = "InvalidResponse";
				break;
			case TRAP::Network::FTP::Response::Status::ConnectionFailed:
				enumStr = "ConnectionFailed";
				break;
			case TRAP::Network::FTP::Response::Status::ConnectionClosed:
				enumStr = "ConnectionClosed";
				break;
			case TRAP::Network::FTP::Response::Status::InvalidFile:
				enumStr = "InvalidFile";
				break;

		default:
            TRAP_ASSERT(false, "fmt::formatter<TRAP::Network::FTP::Response::Status>: Missing enum value!");
            enumStr = "<MISSING ENUM VALUE>";
            break;
		}

		return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of_v<TRAP::Network::FTP::Response, T>, char>>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Network::FTP::Response& response, fmt::format_context& ctx)
    {
		return fmt::format_to(ctx.out(), "{}({}): {}", response.GetStatus(),
		                      std::to_underlying(response.GetStatus()), response.GetMessage());
    }
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::FTP::Response::Response(const Status code, std::string message) noexcept
	: m_status(code), m_message(std::move(message))
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Network::FTP::Response::IsOK() const noexcept
{
	return std::to_underlying(m_status) < 400;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Network::FTP::Response::Status TRAP::Network::FTP::Response::GetStatus() const noexcept
{
	return m_status;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Network::FTP::Response::GetMessage() const noexcept
{
	return m_message;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Network::FTP::ListingResponse::ListingResponse(const Response& response, const std::string_view data)
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

[[nodiscard]] constexpr const std::vector<std::filesystem::path>& TRAP::Network::FTP::ListingResponse::GetListing() const noexcept
{
	return m_listing;
}

#endif /*TRAP_FTP_H*/