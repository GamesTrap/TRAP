#ifndef _TRAP_NONCOPYABLE_H_
#define _TRAP_NONCOPYABLE_H_

class NonCopyable
{
public:
	NonCopyable() = default;

	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};

#endif /*_TRAP_NONCOPYABLE_H_*/