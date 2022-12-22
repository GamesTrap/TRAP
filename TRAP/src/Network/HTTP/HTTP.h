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

#ifndef TRAP_NETWORK_HTTP_H
#define TRAP_NETWORK_HTTP_H

#include <map>

#include "Network/IP/IPv4Address.h"
#include "Network/IP/IPv6Address.h"
#include "Network/Sockets/TCPSocket.h"
#include "Network/Sockets/TCPSocketIPv6.h"

namespace TRAP::Network
{
	/// <summary>
	/// A HTTP client.
	/// Prioritizes IPv6 over IPv4.
	/// </summary>
	class HTTP
	{
	public:
		/// <summary>
		/// Define a HTTP request.
		/// </summary>
		class Request
		{
		public:
			/// <summary>
			/// Enumerate the available HTTP methods for a request.
			/// </summary>
			enum class Method
			{
				GET,   //Request in get mode, standard method to retrieve a page
				POST,  //Request in post mode, usually to send data to a page
				HEAD,  //Request a page's header only
				PUT,   //Request in put mode, useful for a REST API
				DELETE //Request in delete mode, useful for a REST API
			};

			/// <summary>
			/// Constructor.
			///
			/// This constructor creates a GET request, with the root
			/// URI ("/") and an empty body.
			/// </summary>
			/// <param name="uri">Target URI.</param>
			/// <param name="method">Method to use for the request.</param>
			/// <param name="body">Content of the request's body.</param>
			explicit Request(std::string uri = "/", Method method = Method::GET, std::string body = "");

			/// <summary>
			/// Set the value of a field.
			///
			/// The field is created if it doesn't exist.
			/// The name of the field is case-insensitive.
			/// By default, a request doesn't contain any field (but the
			/// mandatory fields are added later by the HTTP client when
			/// sending the request).
			/// </summary>
			/// <param name="field">Name of the field to set.</param>
			/// <param name="value">Value of the field.</param>
			void SetField(const std::string& field, const std::string& value);

			/// <summary>
			/// Set the request method.
			///
			/// See the Method enumeration for a complete list of all
			/// the available methods.
			/// The method is HTTP::Request::GET by default.
			/// </summary>
			/// <param name="method">Method to use for the request.</param>
			void SetMethod(Method method) noexcept;

			/// <summary>
			/// Set the requested URI.
			///
			/// The URI is the resource (usually a web page or a file)
			/// that you want to get or post.
			/// The URI is "/" (the root page) by default.
			/// </summary>
			/// <param name="uri">URI to request, relative to the host.</param>
			void SetURI(std::string uri);

			/// <summary>
			/// Set the HTTP version for the request.
			///
			/// The HTTP version is 1.0 by default.
			/// </summary>
			/// <param name="major">Major HTTP version number.</param>
			/// <param name="minor">Minor HTTP version number.</param>
			void SetHTTPVersion(uint32_t major, uint32_t minor) noexcept;

			/// <summary>
			/// Set the body of the request.
			///
			/// The body of a request is optional and only makes sense
			/// for POST requests.
			/// It is ignored for all other methods.
			/// The body is empty by default.
			/// </summary>
			/// <param name="body">Content of the body.</param>
			void SetBody(std::string body);

		private:
			friend class HTTP;

			/// <summary>
			/// Prepare the final request to send to the server.
			///
			/// This is used internally by HTTP before sending the
			/// request to the web server.
			/// </summary>
			/// <returns>String containing the request, ready to be sent.</returns>
			[[nodiscard]] std::string Prepare() const;

			/// <summary>
			/// Check if the request defines a field.
			///
			/// This function uses case-insensitive comparisons.
			/// </summary>
			/// <param name="field">Name of the field to test.</param>
			/// <returns>True if the field exists, false otherwise.</returns>
			[[nodiscard]] bool HasField(const std::string& field) const;

			using FieldTable = std::map<std::string, std::string>;

			FieldTable m_fields;     //Fields of the header associated to their value
			Method m_method;         //Method to use for the request
			std::string m_uri;       //Target URI of the request
			uint32_t m_majorVersion; //Major HTTP version
			uint32_t m_minorVersion; //Minor HTTP version
			std::string m_body;      //Body of the request
		};

		/// <summary>
		/// Define a HTTP response.
		/// </summary>
		class Response
		{
		public:
			/// <summary>
			/// Enumerate all the valid status codes for a response.
			/// </summary>
			enum class Status
			{
				//2XX: Success
				OK             = 200, //Most common code returned when operation was successful
				Created        = 201, //The resource has successfully been created
				Accepted       = 202, //The request has been accepted, but will be processed later by the server
				NoContent      = 204, //The server didn't send any data in return
				ResetContent   = 205, //The server informs the client that it should clear the view (form) that caused the request to be sent
				PartialContent = 206, //The server has sent a part of the resource, as a response to a partial GET request

				//3XX: Redirection
				MultipleChoices  = 300, //The requested page can be accessed from several locations
				MovedPermanently = 301, //The requested page has permanently moved to a new location
				MovedTemporarily = 302, //The requested page has temporarily moved to a new location
				NotModified      = 304, //For conditional requests, means the requested page hasn't changed and doesn't need to be refreshed

				//4XX: Client error
				BadRequest          = 400, //The server couldn't understand the request (syntax error)
				Unauthorized        = 401, //The requested page needs an authentication to be accessed
				Forbidden           = 403, //The requested page cannot be accessed at all, event with authentication
				NotFound            = 404, //The requested page doesn't exist
				RangeNotSatisfiable = 407, //The server can't satisfy the partial GET request (with a "Range" header field)

				//5XX: Server error
				InternalServerError = 500, //The server encountered an unexpected error
				NotImplemented      = 501, //The server doesn't implement a requested feature
				BadGateway          = 502, //The gateway server has received an error from the source server
				ServiceNotAvailable = 503, //The server is temporarily unavailable (overloaded, in maintenance, ...)
				GatewayTimeout      = 504, //The gateway server couldn't receive a response from the source server
				VersionNotSupported = 505, //The server doesn't support the requested HTTP version

				//10XX: Custom codes
				InvalidResponse  = 1000, //Response is not a valid HTTP one
				ConnectionFailed = 1001  //Connection with server failed
			};

			/// <summary>
			/// Constructor.
			/// </summary>
			Response() noexcept;

			/// <summary>
			/// Get the value of a field.
			///
			/// If the field field is not found in the response header,
			/// the empty string is returned.
			/// This function uses case-insensitive comparisons.
			/// </summary>
			/// <param name="field">Name of the field to get.</param>
			/// <returns>Value of the field, or empty string if not found.</returns>
			[[nodiscard]] std::string GetField(const std::string& field) const;

			/// <summary>
			/// Get the response status code.
			///
			/// The status code should be the first thing to be checked
			/// after receiving a response, it defines whether it is a
			/// success, a failure or anything else (see the Status
			/// enumeration).
			/// </summary>
			/// <returns>Status code of the response.</returns>
			[[nodiscard]] Status GetStatus() const noexcept;

			/// <summary>
			/// Get the major HTTP version number of the response.
			/// </summary>
			/// <returns>Major HTTP version number.</returns>
			[[nodiscard]] uint32_t GetMajorHTTPVersion() const noexcept;

			/// <summary>
			/// Get the minor HTTP version number of the response.
			/// </summary>
			/// <returns>Minor HTTP version number.</returns>
			[[nodiscard]] uint32_t GetMinorHTTPVersion() const noexcept;

			/// <summary>
			/// Get the body of the response.
			///
			/// The body of a response may contain:
			/// - the requested page (for GET requests)
			/// - a response from the server (for POST requests)
			/// - nothing (for HEAD requests)
			/// - an error message (in case of an error)
			/// </summary>
			/// <returns>The response body.</returns>
			[[nodiscard]] std::string GetBody() const noexcept;

		private:
			friend class HTTP;

			/// <summary>
			/// Construct the header from a response string.
			///
			/// This function is used by HTTP to build the response
			/// of a request.
			/// </summary>
			/// <param name="data">Content of the response to parse.</param>
			void Parse(const std::string& data);

			/// <summary>
			/// Read values passed in the answer header.
			///
			/// This function is used by HTTP to extract values passed
			/// in the response.
			/// </summary>
			/// <param name="in">String stream containing the header values.</param>
			void ParseFields(std::istream& in);

			using FieldTable = std::map<std::string, std::string>;

			FieldTable m_fields;     //Fields of the header
			Status m_status;         //Status code
			uint32_t m_majorVersion; //Major HTTP version
			uint32_t m_minorVersion; //Minor HTTP version
			std::string m_body;      //Body of the response
		};

		/// <summary>
		/// Constructor.
		/// </summary>
		HTTP() noexcept;

		/// <summary>
		/// Construct the HTTP client with the target host.
		///
		/// This is equivalent to calling SetHost(host, port).
		/// The port has a default value of 0, which means that the
		/// HTTP client will use the right port according to the
		/// protocol used (80 for HTTP).
		/// You should leave it like this unless you rally need a part other than the
		/// standard one, or use an unknown protocol.
		/// </summary>
		/// <param name="host">Web server to connect to.</param>
		/// <param name="port">Port to use for connection.</param>
		explicit HTTP(std::string host, uint16_t port = 0);

		/// <summary>
		/// Move constructor.
		/// </summary>
		HTTP(HTTP&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		HTTP& operator=(HTTP&&) noexcept = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		HTTP(const HTTP&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		HTTP& operator=(const HTTP&) = delete;
		/// <summary>
		/// Destructor.
		/// </summary>
		~HTTP() = default;

		/// <summary>
		/// Set target host.
		///
		/// This function just stores the host address and port, it
		/// doesn't actually connect to it until you send a request.
		/// The port has a default value of 0, which means that the
		/// HTTP client will use the right port according to the
		/// protocol used (80 for HTTP).
		/// You should leave it like this unless you really need a port other than the
		/// standard one, or use an unknown protocol.
		/// </summary>
		/// <param name="host">Web server to connect to.</param>
		/// <param name="port">Port to use for connection.</param>
		void SetHost(const std::string& host, uint16_t port = 0);

		/// <summary>
		/// Send a HTTP request and return the server's response.
		///
		/// You must have a valid host before sending a request (see SetHost).
		/// Any missing mandatory header field in the request will be added
		/// with an appropriate value.
		/// Warning: this function waits for the server's response and may
		/// not return instantly; use a thread if you don't want to block your
		/// application, or use a timeout to limit the time to wait.
		/// A value of Utils::TimeStep(0.0f) means that the client will use the system default timeout
		/// (which is usually pretty long).
		/// </summary>
		/// <param name="request">Request to send.</param>
		/// <param name="timeout">Maximum time to wait.</param>
		/// <returns>Server response.</returns>
		[[nodiscard]] Response SendRequest(const Request& request, Utils::TimeStep timeout = Utils::TimeStep(0.0f));

	private:
		TCPSocket m_connection;         //Connection to the host
		TCPSocketIPv6 m_connectionIPv6; //Connection to the host
		IPv4Address m_host;             //Web host address
		IPv6Address m_hostIPv6;         //Web host address
		std::string m_hostName;         //Web host name
		uint16_t m_port;                //Port used for connection with host
	};
}

#endif /*TRAP_NETWORK_HTTP_H*/