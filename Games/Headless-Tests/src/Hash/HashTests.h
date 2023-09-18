#ifndef GAMESTRAP_HASHTESTS_H
#define GAMESTRAP_HASHTESTS_H

#include <TRAP.h>

class HashTests final : public TRAP::Layer
{
public:
	constexpr HashTests();

	void OnAttach() override;

private:
    void TestAdler32();
    void TestCRC32();
    void TestSHA2_256();
    void TestSHA2_512();
    void TestSHA3_256();
    void TestSHA3_512();
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr HashTests::HashTests()
	: Layer("Hash")
{
}

#endif /*GAMESTRAP_HASHTESTS_H*/