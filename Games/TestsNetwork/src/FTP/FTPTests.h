#ifndef GAMESTRAP_FTPTESTS_H
#define GAMESTRAP_FTPTESTS_H

#include <TRAP.h>

class FTPTests final : public TRAP::Layer
{
public:
	FTPTests();

	void OnAttach() override;
	void OnDetach() override;

private:
	static void FTP();

	std::unique_ptr<std::thread> m_ftpThread;
};

#endif /*GAMESTRAP_FTPTESTS_H*/