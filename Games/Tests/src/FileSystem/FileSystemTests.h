#ifndef GAMESTRAP_FILESYSTEMTESTS_H
#define GAMESTRAP_FILESYSTEMTESTS_H

#include <TRAP.h>

class FileSystemTests final : public TRAP::Layer
{
public:
	FileSystemTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	void OnEvent(TRAP::Events::Event& event) override;

	static bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
};

#endif /*GAMESTRAP_FILESYSTEMTESTS_H*/