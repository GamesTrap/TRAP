#ifndef _TRAP_TIMESTEP_H_
#define _TRAP_TIMESTEP_H_

namespace TRAP::Utils
{
	struct TimeStep
	{
		explicit TimeStep(float time);

		operator float() const;
		
		float GetSeconds() const;
		float GetMilliseconds() const;

	private:
		float m_time;
	};
}

#endif /*_TRAP_TIMESTEP_H_*/