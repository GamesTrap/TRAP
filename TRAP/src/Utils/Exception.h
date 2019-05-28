#pragma once

namespace TRAP
{
	class Exception : public std::exception
	{
	public:
		Exception(int line, const char* file) noexcept;

		const char* what() const override;

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
