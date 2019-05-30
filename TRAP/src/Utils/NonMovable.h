#ifndef _TRAP_NONMOVABLE_H_
#define _TRAP_NONMOVABLE_H_

class NonMovable
{
public:
	NonMovable(NonMovable&&) = delete;
	NonMovable& operator=(NonMovable&&) = delete;

protected:
	NonMovable() = default;
};

#endif /*_TRAP_NONMOVABLE_H_*/