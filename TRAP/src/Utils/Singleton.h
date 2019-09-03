#ifndef _TRAP_SINGLETON_H_
#define _TRAP_SINGLETON_H_

class Singleton
{
public:
	Singleton(Singleton& other) = delete;
	Singleton(Singleton&& other) = delete;


	Singleton& operator =(Singleton& other) = delete;
	Singleton& operator =(Singleton&& other) = delete;

protected:
	Singleton() = default;
	virtual ~Singleton() = default;
};

#endif /*_TRAP_SINGLETON_H_*/