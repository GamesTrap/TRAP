#ifndef GAMESTRAP_FILESYSTEMTESTS_H
#define GAMESTRAP_FILESYSTEMTESTS_H

#include <TRAP.h>

class FileSystemTests final : public TRAP::Layer
{
public:
	constexpr FileSystemTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	constexpr void OnEvent(TRAP::Events::Event& event) override;

private:
	static bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr FileSystemTests::FileSystemTests()
	: Layer("FileSystem")
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void FileSystemTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(OnKeyPress);
}

#endif /*GAMESTRAP_FILESYSTEMTESTS_H*/
