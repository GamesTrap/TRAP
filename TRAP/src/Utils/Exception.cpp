#include "TRAPPCH.h"
#include "Exception.h"

namespace TRAP
{
	Exception::Exception(const int line, const char* file) noexcept
		: m_line(line), m_file(file)
	{		
	}

	const char* Exception::what() const
	{
		std::ostringstream oss;
		oss << GetType() << std::endl << GetOriginString();
		p_whatBuffer = oss.str();

		return p_whatBuffer.c_str();
	}

	const char* Exception::GetType() const noexcept
	{
		return "TRAP Exception";
	}

	int Exception::GetLine() const noexcept
	{
		return m_line;
	}

	std::string_view Exception::GetFile() const noexcept
	{
		return m_file;
	}

	std::string Exception::GetOriginString() const
	{
		std::ostringstream oss;
		oss << "[File] " << m_file << std::endl << "[Line] " << m_line;

		return oss.str();
	}
}
