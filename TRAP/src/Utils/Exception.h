#ifndef _TRAP_EXCEPTION_H_
#define _TRAP_EXCEPTION_H_

namespace TRAP
{
	class Exception : public std::exception
	{
	public:
		Exception(int line, const char* file) noexcept;
		Exception(const Exception&) = delete;
		Exception& operator=(const Exception&) = delete;
		Exception(Exception&&) = delete;
		Exception& operator=(Exception&&) = delete;
		virtual ~Exception() = default;

		char const* what() const override;

		virtual const char* GetType() const noexcept;
		int GetLine() const noexcept;
		std::string_view GetFile() const noexcept;
		std::string GetOriginString() const;

	protected:
		mutable std::string p_whatBuffer;

	private:
		int m_line;
		std::string m_file;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline const char* TRAP::Exception::GetType() const noexcept
{
	return "TRAP Exception";
}

//-------------------------------------------------------------------------------------------------------------------//

inline int TRAP::Exception::GetLine() const noexcept
{
	return m_line;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string_view TRAP::Exception::GetFile() const noexcept
{
	return m_file;
}

#endif /*_TRAP_EXCEPTION_H_*/