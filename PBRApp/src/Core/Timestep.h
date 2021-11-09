#pragma once

class Timestep
{
public:
	Timestep(float timeMs = 0.0f)
		: mTimeMs(timeMs)
	{
	}

	operator float() const { return mTimeMs / 1000.0f; }

	float GetSeconds() const { return mTimeMs / 1000.0f; }
	float GetMilliseconds() const { return mTimeMs; }
private:
	float mTimeMs;
};
