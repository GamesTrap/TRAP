#include "TRAPPCH.h"
#include "Exception.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Exception::Exception(const int line, const char* file) noexcept
	: m_line(line), m_file(file)
{
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Exception::what() const
{
	std::ostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	p_whatBuffer = oss.str();

	return p_whatBuffer.c_str();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Exception::GetOriginString() const
{
	std::ostringstream oss;
	oss << "[File] " << m_file << std::endl << "[Line] " << m_line;

	return oss.str();
}