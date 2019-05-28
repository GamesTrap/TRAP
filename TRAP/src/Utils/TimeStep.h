#pragma once

namespace TRAP::Utils
{
	struct TimeStep
	{
		explicit TimeStep(float initialTime);

		void Update(float currentTime);

		float GetMilliseconds() const;
		float GetSeconds() const;

	private:
		float m_timeStep;
		float m_lastTime;
	};
}