#include "TRAPPCH.h"
#include "Log.h"

namespace TRAP
{
	HANDLE Log::m_handleConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO Log::m_csbi;

	void Log::GetInfo()
	{
		GetConsoleScreenBufferInfo(m_handleConsole, &m_csbi);
	}
	void Log::SetColor(const WORD wRGBI)
	{
		GetInfo();
		m_csbi.wAttributes &= 0;
		m_csbi.wAttributes |= wRGBI;
		SetConsoleTextAttribute(m_handleConsole, m_csbi.wAttributes);
	}
	void Log::ResetColor()
	{
		SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
}