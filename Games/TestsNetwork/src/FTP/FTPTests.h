#ifndef _GAMESTRAP_FTPTESTS_H_
#define _GAMESTRAP_FTPTESTS_H_

#include <TRAP.h>

class FTPTests final : public TRAP::Layer
{
public:
	FTPTests();

	void OnAttach() override;
	void OnDetach() override;

private:
	static void FTP();
	
	TRAP::Scope<std::thread> m_ftpThread;
};

#endif /*_GAMESTRAP_FTPTESTS_H_*/