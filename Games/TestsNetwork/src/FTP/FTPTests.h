#ifndef GAMESTRAP_FTPTESTS_H
#define GAMESTRAP_FTPTESTS_H

#include <TRAP.h>

class FTPTests final : public TRAP::Layer
{
public:
	constexpr FTPTests();

	void OnAttach() override;

private:
	static void FTP();

	std::unique_ptr<std::thread> m_ftpThread = nullptr;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr FTPTests::FTPTests()
	: Layer("FTPTests")
{
}

#endif /*GAMESTRAP_FTPTESTS_H*/