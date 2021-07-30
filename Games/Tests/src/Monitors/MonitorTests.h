#ifndef _GAMESTRAP_MONITORTESTS_H_
#define _GAMESTRAP_MONITORTESTS_H_

#include <TRAP.h>

class MonitorTests final : public TRAP::Layer
{
public:
	MonitorTests();

	void OnAttach() override;
	void OnImGuiRender() override;

private:
	static int32_t Euclid(int32_t a, int32_t b);
	static std::string FormatMode(const TRAP::Monitor::VideoMode& mode);
	static void ListModes(const TRAP::Monitor& monitor);

	struct MonitorInfo
	{
		std::string Name{};
		uint32_t ID{};
		TRAP::Monitor::VideoMode CurrentVideoMode;
		std::vector<TRAP::Monitor::VideoMode> VideoModes{};
		TRAP::Math::Vec2i Position{};
		TRAP::Math::Vec2 Scale{};
		TRAP::Math::Vec4i WorkArea{};
	};
	static std::vector<MonitorInfo> s_monitorInfos;
};

#endif /*_GAMESTRAP_MONITORTESTS_H_*/