#ifndef _TRAP_TIMESTEP_H_
#define _TRAP_TIMESTEP_H_

namespace TRAP::Utils
{
	struct TimeStep
	{
		explicit TimeStep(float initialTime);

		void Update(float currentTime);

		float GetMilliseconds() const;
		float GetSeconds() const;

		operator float() const;

	private:
		float m_timeStep;
		float m_lastTime;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline float TRAP::Utils::TimeStep::GetMilliseconds() const
{
	return m_timeStep;
}

//-------------------------------------------------------------------------------------------------------------------//

inline float TRAP::Utils::TimeStep::GetSeconds() const
{
	return m_timeStep * 0.001f;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Utils::TimeStep::operator float() const
{
	return m_timeStep;
}

#endif /*_TRAP_TIMESTEP_H_*/