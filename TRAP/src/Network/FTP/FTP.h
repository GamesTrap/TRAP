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

namespace TRAP::Network
{
	class IPv4Address;

	/// <summary>
	/// A FTP client.
	/// Note: Only supports IPv4.
	/// </summary>
	class FTP
	{
	public:
		/// <summary>
		/// Enumeration of transfer modes
		/// </summary>
		enum class TransferMode
		{
			Binary, //Binary mode (file is transferred as a sequence of bytes)
			ASCII,  //Text mode using ASCII encoding
			EBCDIC  //Text mode using EBCDIC encoding
		};

		class Response
		{
		public:
			/// <summary>
			/// Status codes possibly returned by a FTP response.
			/// </summary>
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

			/// <summary>
			/// This constructor is used by the FTP client to build the response.
			/// </summary>
			/// <param name="code">Response status code.</param>
			/// <param name="message">Response message.</param>
			constexpr explicit Response(Status code = Status::InvalidResponse, std::string message = "") noexcept;

			/// <summary>
			/// Check if the status code means a success.
			///
			/// This function is defined for convenience, it is
			/// equivalent to testing if the status code is < 400.
			/// </summary>
			/// <returns>True if the status is a success, false if it is a failure.</returns>
			[[nodiscard]] constexpr bool IsOK() const noexcept;

			/// <summary>
			/// Get the status code of the response.
			/// </summary>
			/// <returns>Status code.</returns>
			[[nodiscard]] constexpr Status GetStatus() const noexcept;

			/// <summary>
			/// Get the full message contained in the response.
			/// </summary>
			/// <returns>The response message.</returns>
			[[nodiscard]] constexpr std::string GetMessage() const noexcept;

		private:
			Status m_status; //Status code returned from the server
			std::string m_message; //Last message received from the server
		};

		friend std::ostream& operator<<(std::ostream& stream, const Response& response)
		{
			return stream << std::to_string(std::to_underlying(response.GetStatus())) << response.GetMessage();
		}

		/// <summary>
		/// Specialization of FTP response returning a directory.
		/// </summary>
		class DirectoryResponse : public Response
		{
		public:
			/// <summary>
			/// Constructor.
			/// </summary>
			/// <param name="response">Source response.</param>
			explicit DirectoryResponse(const Response& response);

			/// <summary>
			/// Get the directory returned in the response.
			/// </summary>
			/// <returns>Directory name.</returns>
			[[nodiscard]] const std::filesystem::path& GetDirectory() const noexcept;

		private:
			std::filesystem::path m_directory; //Directory extracted from the response message
		};

		/// <summary>
		/// Specialization of FTP response returning a filename listing.
		/// </summary>
		class ListingResponse : public Response
		{
		public:
			/// <summary>
			/// Constructor.
			/// </summary>
			/// <param name="response">Source response.</param>
			/// <param name="data">Data containing the raw listing.</param>
			constexpr ListingResponse(const Response& response, std::string_view data);

			/// <summary>
			/// Return the array of directory/file names.
			/// </summary>
			/// <returns>Array containing the requested listing.</returns>
			[[nodiscard]] constexpr const std::vector<std::filesystem::path>& GetListing() const noexcept;

		private:
			std::vector<std::filesystem::path> m_listing; //Directory/file names extracted from the data
		};

		/// <summary>
		/// Constructor.
		/// </summary>
		FTP() noexcept = default;

		/// <summary>
		/// Automatically closes the connection with the server if it is still opened.
		/// </summary>
		~FTP();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr FTP(const FTP&) = delete;

		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr FTP& operator=(const FTP&) = delete;

		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr FTP(FTP&&) noexcept = default;

		/// <summary>
		/// Move assignment operator.
		/// </summary>
		FTP& operator=(FTP&&) noexcept = default;

		/// <summary>
		/// Connect to the specified FTP server.
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
		/// </summary>
		/// <param name="server">Name or address of the FTP server to connect to.</param>
		/// <param name="port">Port used for the connection.</param>
		/// <param name="timeout">Maximum time to wait.</param>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] Response Connect(const IPv4Address& server, u16 port = 21,
		                               Utils::TimeStep timeout = Utils::TimeStep(0.0f));

		/// <summary>
		/// Close the connection with the server.
		/// </summary>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] Response Disconnect();

		/// <summary>
		/// Log in using an anonymous account.
		///
		/// Logging in is mandatory after connecting to the server.
		/// Users that are not logged in cannot perform any operation.
		/// </summary>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] Response Login();

		/// <summary>
		/// Log in using a username and a password.
		///
		/// Logging in is mandatory after connecting to the server.
		/// Users that are not logged in cannot perform any operation.
		/// </summary>
		/// <param name="name">User name.</param>
		/// <param name="password">Password.</param>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] Response Login(const std::string& name, const std::string& password);

		/// <summary>
		/// Send a null command to keep the connection alive.
		///
		/// This command is useful because the server may close the
		/// connection automatically if no command is sent.
		/// </summary>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] Response KeepAlive();

		/// <summary>
		/// Get the current working directory.
		///
		/// The working directory is the root path for subsequent
		/// operations involving directories and/or filenames.
		/// </summary>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] [[nodiscard]] DirectoryResponse GetWorkingDirectory();

		/// <summary>
		/// Get the contents of the given directory.
		///
		/// This function retrieves the sub-directories and files
		/// contained in the given directory.
		/// It is not recursive.
		/// The directory parameter is relative to the current working directory.
		/// </summary>
		/// <param name="directory">Directory to list.</param>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] [[nodiscard]] ListingResponse GetDirectoryListing(const std::filesystem::path& directory = "");

		/// <summary>
		/// Change the current working directory.
		///
		/// The new directory must be relative to the current one.
		/// </summary>
		/// <param name="directory">New working directory.</param>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] Response ChangeDirectory(const std::filesystem::path& directory);

		/// <summary>
		/// Go to the parent directory of the current one.
		/// </summary>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] Response ParentDirectory();

		/// <summary>
		/// Create a new directory.
		///
		/// The new directory is created as a child of the current working directory.
		/// </summary>
		/// <param name="name">Name of the directory to create.</param>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] Response CreateDirectory(const std::filesystem::path& name);

		/// <summary>
		/// Remove an existing directory.
		///
		/// The directory to remove must be relative to the
		/// current working directory.
		/// Use this function with caution, the directory will
		/// be removed permanently!
		/// </summary>
		/// <param name="name">Name of the directory to remove.</param>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] Response DeleteDirectory(const std::filesystem::path& name);

		/// <summary>
		/// Rename an existing file.
		///
		/// The filenames must be relative to the current working directory.
		/// </summary>
		/// <param name="file">File to rename.</param>
		/// <param name="newName">New name of the file.</param>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] Response RenameFile(const std::filesystem::path& file, const std::filesystem::path& newName);

		/// <summary>
		/// Remove an existing file.
		///
		/// The file name must be relative to the current working directory.
		/// Use this function with caution, the file will be
		/// removed permanently!
		/// </summary>
		/// <param name="name">File to remove.</param>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] Response DeleteFile(const std::filesystem::path& name);

		/// <summary>
		/// Download a file from the server.
		///
		/// The filename of the distant file is relative to the
		/// current working directory of the server, and the local
		/// destination path is relative to the current directory
		/// of you application.
		/// If a file with the same filename as the distant file
		/// already exists in the local destination path, it will
		/// be overwritten.
		/// </summary>
		/// <param name="remoteFile">Filename of the distant file to download.</param>
		/// <param name="path">
		/// The directory in which to put the file on the local computer.
		/// </param>
		/// <param name="mode">Transfer mode.</param>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] Response Download(const std::filesystem::path& remoteFile,
										const std::filesystem::path& path,
		                                TransferMode mode = TransferMode::Binary);

		/// <summary>
		/// Upload a file to the server.
		///
		/// The name of the local file is relative to the current
		/// working directory of your application, and the
		/// remote path is relative to the current directory of the
		/// FTP server.
		///
		/// The append parameter controls whether the remote file is
		/// appended to or overwritten if it already exists.
		/// </summary>
		/// <param name="localFile">Path of the local file to upload.</param>
		/// <param name="remotePath">The directory in which to put the file on the server.</param>
		/// <param name="mode">Transfer mode.</param>
		/// <param name="append">
		/// Pass true to append to or false to overwrite the remote file if it already exists.
		/// </param>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] Response Upload(const std::filesystem::path& localFile,
		                			  const std::filesystem::path& remotePath,
		                			  TransferMode mode = TransferMode::Binary, bool append = false);

		/// <summary>
		/// Send a command to the FTP server.
		///
		/// While the most often used commands are provided as member
		/// functions in the TRAP::Network::FTP class, this method can be used
		/// to send any FTP command to the server.
		/// If the command requires one or more parameters, they can be specified
		/// in parameter.
		/// If the server returns information, you
		/// can extract it from the response using TRAP::Network::FTP::Response::GetMessage().
		/// </summary>
		/// <param name="command">Command to send.</param>
		/// <param name="parameter">Command parameter.</param>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] Response SendCommand(const std::string& command, const std::string& parameter = "");

	private:
		/// <summary>
		/// Receive a response from the server.
		///
		/// This function must be called after each call to
		/// SendCommand that expects a response.
		/// </summary>
		/// <returns>Server response to the request.</returns>
		[[nodiscard]] Response GetResponse();

		/// <summary>
		/// Utility class for exchanging data with the server on the data channel.
		/// </summary>
		class DataChannel;

		friend class DataChannel;

		TCPSocket m_commandSocket; //Socket holding the control connection with the server
		std::string m_receiveBuffer; //Received command data that is yet to be processed
	};
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Network::FTP::Response> : fmt::ostream_formatter
{};

template<>
struct fmt::formatter<TRAP::Network::FTP::DirectoryResponse> : fmt::ostream_formatter
{};

template<>
struct fmt::formatter<TRAP::Network::FTP::ListingResponse> : fmt::ostream_formatter
{};

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