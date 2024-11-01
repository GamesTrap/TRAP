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

//Modified by: Jan "GamesTrap" Schuerkamp

#ifndef TRAP_NETWORK_HTTP_H
#define TRAP_NETWORK_HTTP_H

#include <map>

#include "Network/IP/IPv4Address.h"
#include "Network/IP/IPv6Address.h"
#include "Network/Sockets/TCPSocket.h"
#include "Network/Sockets/TCPSocketIPv6.h"

namespace TRAP::Network
{
	/// @brief Enum describing which IP version to use.
	///        Defaults to both IPv6 and IPv4 (prioritizes IPv6).
	enum class IPVersion : u8
	{
		IPv6 = BIT(0u), //Proritize IPv6 over IPv4
		IPv4 = BIT(1u),

		Combined = IPv6 | IPv4
	};
	MAKE_ENUM_FLAG(IPVersion);

	/// @brief A HTTP client.
	/// Prioritizes IPv6 over IPv4.
	class HTTP
	{
	public:
		/// @brief Define a HTTP request.
		class Request
		{
		public:
			/// @brief Enumerate the available HTTP methods for a request.
			enum class Method : u8
			{
				GET,   //Request in get mode, standard method to retrieve a page
				POST,  //Request in post mode, usually to send data to a page
				HEAD,  //Request a page's header only
				PUT,   //Request in put mode, useful for a REST API
				DELETE //Request in delete mode, useful for a REST API
			};

			/// @brief Constructor.
			///
			/// This constructor creates a GET request, with the root
			/// URI ("/") and an empty body.
			/// @param uri Target URI.
			/// @param method Method to use for the request.
			/// @param body Content of the request's body.
			explicit Request(std::string uri = "/", Method method = Method::GET, std::string body = "");

			/// @brief Copy constructor.
			Request(const Request&) = default;
			/// @brief Copy assignment operator.
			Request& operator=(const Request&) = default;
			/// @brief Move constructor.
			Request(Request&&) noexcept = default;
			/// @brief Move assignment operator.
			Request& operator=(Request&&) noexcept = default;

			/// @brief Destructor.
			~Request() = default;

			/// @brief Set the value of a field.
			///
			/// The field is created if it doesn't exist.
			/// The name of the field is case-insensitive.
			/// By default, a request doesn't contain any field (but the
			/// mandatory fields are added later by the HTTP client when
			/// sending the request).
			/// @param field Name of the field to set.
			/// @param value Value of the field.
			void SetField(const std::string& field, std::string value);

			/// @brief Set the request method.
			///
			/// See the Method enumeration for a complete list of all
			/// the available methods.
			/// The method is HTTP::Request::GET by default.
			/// @param method Method to use for the request.
			constexpr void SetMethod(Method method) noexcept;

			/// @brief Set the requested URI.
			///
			/// The URI is the resource (usually a web page or a file)
			/// that you want to get or post.
			/// The URI is "/" (the root page) by default.
			/// @param uri URI to request, relative to the host.
			constexpr void SetURI(std::string uri);

			/// @brief Set the HTTP version for the request.
			///
			/// The HTTP version is 1.0 by default.
			/// @param major Major HTTP version number.
			/// @param minor Minor HTTP version number.
			constexpr void SetHTTPVersion(u32 major, u32 minor) noexcept;

			/// @brief Set the body of the request.
			///
			/// The body of a request is optional and only makes sense
			/// for POST requests.
			/// It is ignored for all other methods.
			/// The body is empty by default.
			/// @param body Content of the body.
			constexpr void SetBody(std::string body);

		private:
			friend class HTTP;

			/// @brief Prepare the final request to send to the server.
			///
			/// This is used internally by HTTP before sending the
			/// request to the web server.
			/// @return String containing the request, ready to be sent.
			[[nodiscard]] std::string Prepare() const;

			/// @brief Check if the request defines a field.
			///
			/// This function uses case-insensitive comparisons.
			/// @param field Name of the field to test.
			/// @return True if the field exists, false otherwise.
			[[nodiscard]] bool HasField(const std::string& field) const;

			using FieldTable = std::map<std::string, std::string, std::less<>>;

			FieldTable m_fields{};         //Fields of the header associated to their value
			Method m_method = Method::GET; //Method to use for the request
			std::string m_uri{};           //Target URI of the request
			u32 m_majorVersion = 1u;       //Major HTTP version
			u32 m_minorVersion = 0u;       //Minor HTTP version
			std::string m_body{};          //Body of the request
		};

		/// @brief Define a HTTP response.
		class Response
		{
		public:
			/// @brief Enumerate all the valid status codes for a response.
			enum class Status : u16
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

			/// @brief Constructor.
			Response() noexcept;

			/// @brief Copy constructor.
			Response(const Response&) = default;
			/// @brief Copy assignment operator.
			Response& operator=(const Response&) = default;
			/// @brief Move constructor.
			Response(Response&&) noexcept = default;
			/// @brief Move assignment operator.
			Response& operator=(Response&&) noexcept = default;

			/// @brief Destructor.
			~Response() = default;

			/// @brief Get the value of a field.
			///
			/// If the field field is not found in the response header,
			/// the empty string is returned.
			/// This function uses case-insensitive comparisons.
			/// @param field Name of the field to get.
			/// @return Value of the field, or empty string if not found.
			[[nodiscard]] std::string GetField(const std::string& field) const;

			/// @brief Get the response status code.
			///
			/// The status code should be the first thing to be checked
			/// after receiving a response, it defines whether it is a
			/// success, a failure or anything else (see the Status
			/// enumeration).
			/// @return Status code of the response.
			[[nodiscard]] constexpr Status GetStatus() const noexcept;

			/// @brief Get the major HTTP version number of the response.
			/// @return Major HTTP version number.
			[[nodiscard]] constexpr u32 GetMajorHTTPVersion() const noexcept;

			/// @brief Get the minor HTTP version number of the response.
			/// @return Minor HTTP version number.
			[[nodiscard]] constexpr u32 GetMinorHTTPVersion() const noexcept;

			/// @brief Get the body of the response.
			///
			/// The body of a response may contain:
			/// - the requested page (for GET requests)
			/// - a response from the server (for POST requests)
			/// - nothing (for HEAD requests)
			/// - an error message (in case of an error)
			/// @return The response body.
			[[nodiscard]] constexpr std::string GetBody() const noexcept;

		private:
			friend class HTTP;

			/// @brief Construct the header from a response string.
			///
			/// This function is used by HTTP to build the response
			/// of a request.
			/// @param data Content of the response to parse.
			void Parse(const std::string& data);

			/// @brief Read values passed in the answer header.
			///
			/// This function is used by HTTP to extract values passed
			/// in the response.
			/// @param in String stream containing the header values.
			void ParseFields(std::istream& in);

			using FieldTable = std::map<std::string, std::string, std::equal_to<>>;

			FieldTable m_fields{};                      //Fields of the header
			Status m_status = Status::ConnectionFailed; //Status code
			u32 m_majorVersion = 0u;                    //Major HTTP version
			u32 m_minorVersion = 0u;                    //Minor HTTP version
			std::string m_body{};                       //Body of the response
		};

		/// @brief Constructor.
		HTTP() noexcept;

		/// @brief Construct the HTTP client with the target host.
		///
		/// This is equivalent to calling SetHost(host, port).
		/// The port has a default value of 0, which means that the
		/// HTTP client will use the right port according to the
		/// protocol used (80 for HTTP).
		/// You should leave it like this unless you rally need a part other than the
		/// standard one, or use an unknown protocol.
		/// @param host Web server to connect to.
		/// @param port Port to use for connection.
		explicit HTTP(const std::string& host, u16 port = 0);

		/// @brief Move constructor.
		constexpr HTTP(HTTP&&) noexcept = default;
		/// @brief Move assignment operator.
		HTTP& operator=(HTTP&&) noexcept = default;
		/// @brief Copy constructor.
		consteval HTTP(const HTTP&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval HTTP& operator=(const HTTP&) noexcept = delete;
		/// @brief Destructor.
		~HTTP() = default;

		/// @brief Set target host.
		///
		/// This function just stores the host address and port, it
		/// doesn't actually connect to it until you send a request.
		/// The port has a default value of 0, which means that the
		/// HTTP client will use the right port according to the
		/// protocol used (80 for HTTP).
		/// You should leave it like this unless you really need a port other than the
		/// standard one, or use an unknown protocol.
		/// @param host Web server to connect to.
		/// @param port Port to use for connection.
		/// @param ipVersion Which version of the Internet Protocol to use.
		void SetHost(std::string host, u16 port = 0u, IPVersion ipVersion = IPVersion::Combined);

		/// @brief Send a HTTP request and return the server's response.
		///
		/// You must have a valid host before sending a request (see SetHost).
		/// Any missing mandatory header field in the request will be added
		/// with an appropriate value.
		///
		/// A value of Utils::TimeStep(0.0f) means that the client will use the system default timeout
		/// (which is usually pretty long).
		/// @param request Request to send.
		/// @param timeout Maximum time to wait. Default: 2 minutes.
		/// @return Server response.
		/// @warning This function waits for the server's response and may not return instantly;
		///          use a thread if you don't want to block your application, or use a
		///          timeout to limit the time to wait.
		[[nodiscard]] Response SendRequest(const Request& request, Utils::TimeStep timeout = Utils::TimeStep(120.0f));

	private:
		TCPSocket m_connection;         //Connection to the host
		TCPSocketIPv6 m_connectionIPv6; //Connection to the host
		IPv4Address m_host{};           //Web host address
		IPv6Address m_hostIPv6{};       //Web host address
		std::string m_hostName{};       //Web host name
		u16 m_port = 0;                 //Port used for connection with host
	};
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Network::HTTP::Request::Method>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Network::HTTP::Request::Method method,
	                                            fmt::format_context& ctx)
    {
		std::string enumStr{};

		switch(method)
		{
			case TRAP::Network::HTTP::Request::Method::GET:
				enumStr = "GET";
				break;
			case TRAP::Network::HTTP::Request::Method::POST:
				enumStr = "POST";
				break;
			case TRAP::Network::HTTP::Request::Method::HEAD:
				enumStr = "HEAD";
				break;
			case TRAP::Network::HTTP::Request::Method::PUT:
				enumStr = "PUT";
				break;
			case TRAP::Network::HTTP::Request::Method::DELETE:
				enumStr = "DELETE";
				break;
		}

		if(enumStr.empty())
		{
            TRAP_ASSERT(false, "fmt::formatter<TRAP::Network::HTTP::Request::Method>: Missing enum value!");
            enumStr = "<MISSING ENUM VALUE>";
		}

		return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Network::HTTP::Response::Status>
{
    static constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Network::HTTP::Response::Status method,
	                                            fmt::format_context& ctx)
    {
		std::string enumStr{};

		switch(method)
		{
			case TRAP::Network::HTTP::Response::Status::OK:
				enumStr = "OK";
				break;
			case TRAP::Network::HTTP::Response::Status::Created:
				enumStr = "Created";
				break;
			case TRAP::Network::HTTP::Response::Status::Accepted:
				enumStr = "Accepted";
				break;
			case TRAP::Network::HTTP::Response::Status::NoContent:
				enumStr = "NoContent";
				break;
			case TRAP::Network::HTTP::Response::Status::ResetContent:
				enumStr = "ResetContent";
				break;
			case TRAP::Network::HTTP::Response::Status::PartialContent:
				enumStr = "PartialContent";
				break;

			case TRAP::Network::HTTP::Response::Status::MultipleChoices:
				enumStr = "MultipleChoices";
				break;
			case TRAP::Network::HTTP::Response::Status::MovedPermanently:
				enumStr = "MovedPermanently";
				break;
			case TRAP::Network::HTTP::Response::Status::MovedTemporarily:
				enumStr = "MovedTemporarily";
				break;
			case TRAP::Network::HTTP::Response::Status::NotModified:
				enumStr = "NotModified";
				break;

			case TRAP::Network::HTTP::Response::Status::BadRequest:
				enumStr = "BadRequest";
				break;
			case TRAP::Network::HTTP::Response::Status::Unauthorized:
				enumStr = "Unauthorized";
				break;
			case TRAP::Network::HTTP::Response::Status::Forbidden:
				enumStr = "Forbidden";
				break;
			case TRAP::Network::HTTP::Response::Status::NotFound:
				enumStr = "NotFound";
				break;
			case TRAP::Network::HTTP::Response::Status::RangeNotSatisfiable:
				enumStr = "RangeNotSatisfiable";
				break;

			case TRAP::Network::HTTP::Response::Status::InternalServerError:
				enumStr = "InternalServerError";
				break;
			case TRAP::Network::HTTP::Response::Status::NotImplemented:
				enumStr = "NotImplemented";
				break;
			case TRAP::Network::HTTP::Response::Status::BadGateway:
				enumStr = "BadGateway";
				break;
			case TRAP::Network::HTTP::Response::Status::ServiceNotAvailable:
				enumStr = "ServiceNotAvailable";
				break;
			case TRAP::Network::HTTP::Response::Status::GatewayTimeout:
				enumStr = "GatewayTimeout";
				break;
			case TRAP::Network::HTTP::Response::Status::VersionNotSupported:
				enumStr = "VersionNotSupported";
				break;

			case TRAP::Network::HTTP::Response::Status::InvalidResponse:
				enumStr = "InvalidResponse";
				break;
			case TRAP::Network::HTTP::Response::Status::ConnectionFailed:
				enumStr = "ConnectionFailed";
				break;
		}

		if(enumStr.empty())
		{
            TRAP_ASSERT(false, "fmt::formatter<TRAP::Network::HTTP::Response::Status>: Missing enum value!");
            enumStr = "<MISSING ENUM VALUE>";
		}

		return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Network::HTTP::Request::SetMethod(const Method method) noexcept
{
	m_method = method;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Network::HTTP::Request::SetURI(std::string uri)
{
	m_uri = std::move(uri);

	//Make sure it starts with a '/'
	if (m_uri.empty() || (m_uri[0] != '/'))
		m_uri.insert(0, "/");
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Network::HTTP::Request::SetHTTPVersion(const u32 major, const u32 minor) noexcept
{
	m_majorVersion = major;
	m_minorVersion = minor;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Network::HTTP::Request::SetBody(std::string body)
{
	m_body = std::move(body);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Network::HTTP::Response::Status TRAP::Network::HTTP::Response::GetStatus() const noexcept
{
	return m_status;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Network::HTTP::Response::GetMajorHTTPVersion() const noexcept
{
	return m_majorVersion;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Network::HTTP::Response::GetMinorHTTPVersion() const noexcept
{
	return m_minorVersion;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Network::HTTP::Response::GetBody() const noexcept
{
	return m_body;
}

#endif /*TRAP_NETWORK_HTTP_H*/
