#ifndef _TRAP_SINGLETON_H_
#define _TRAP_SINGLETON_H_

#include "NonMovable.h"
#include "NonCopyable.h"

class Singleton : public NonMovable, public NonCopyable
{	
};

#endif /*_TRAP_SINGLETON_H_*/