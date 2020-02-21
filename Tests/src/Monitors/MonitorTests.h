#ifndef _GAMESTRAP_MONITORTESTS_H_
#define _GAMESTRAP_MONITORTESTS_H_

#include <TRAP.h>

class MonitorTests final : public TRAP::Layer
{
public:
	MonitorTests();

	void OnAttach() override;

private:
	static int32_t Euclid(int32_t a, int32_t b);
	static std::string FormatMode(const TRAP::Monitor::VideoMode& mode);
	static void ListModes(const TRAP::Monitor& monitor);
};

#endif /*_GAMESTRAP_MONITORTESTS_H_*/