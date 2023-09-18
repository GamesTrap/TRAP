#ifndef GAMESTRAP_FILESYSTEMTESTS_H
#define GAMESTRAP_FILESYSTEMTESTS_H

#include <TRAP.h>

class FileSystemTests final : public TRAP::Layer
{
public:
    constexpr FileSystemTests();

    void OnAttach() override;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr FileSystemTests::FileSystemTests()
    : Layer("HeadlessFileSystemTests")
{
}

#endif /*GAMESTRAP_FILESYSTEMTESTS_H*/