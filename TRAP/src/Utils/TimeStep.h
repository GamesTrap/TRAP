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

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Utils::TimeStep::operator float() const
{
	return m_time;
}

//-------------------------------------------------------------------------------------------------------------------//

inline float TRAP::Utils::TimeStep::GetSeconds() const
{
	return m_time;
}

//-------------------------------------------------------------------------------------------------------------------//

inline float TRAP::Utils::TimeStep::GetMilliseconds() const
{
	return m_time * 1000.0f;
}

#endif /*_TRAP_TIMESTEP_H_*/