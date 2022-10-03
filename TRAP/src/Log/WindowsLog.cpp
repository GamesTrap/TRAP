#include "TRAPPCH.h"
#include "Log.h"

#include "Utils/String/String.h"

HANDLE TRAP::Log::m_handleConsole = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO TRAP::Log::m_csbi;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::GetInfo()
{
	ZoneScoped;

	GetConsoleScreenBufferInfo(m_handleConsole, &m_csbi);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::SetColor(const WORD wRGBI)
{
	ZoneScoped;

	GetInfo();
	m_csbi.wAttributes &= 0;
	m_csbi.wAttributes |= wRGBI;
	SetConsoleTextAttribute(m_handleConsole, m_csbi.wAttributes);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::ResetColor()
{
	ZoneScoped;

	SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}