#ifndef GAMESTRAP_FILESYSTEMTESTS_H
#define GAMESTRAP_FILESYSTEMTESTS_H

#include <TRAP.h>

class FileSystemTests final : public TRAP::Layer
{
public:
    FileSystemTests();

    void OnAttach() override;
};

#endif /*GAMESTRAP_FILESYSTEMTESTS_H*/