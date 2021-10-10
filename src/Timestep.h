#pragma once

class Timestep
{
public:
	Timestep(float timeMs = 0.0f)
		: mTimeMs(timeMs)
	{
	}

	operator float() const { return mTimeMs; }

	float getSeconds() const { return mTimeMs / 1000.0f; }
	float getMilliseconds() const { return mTimeMs; }
private:
	float mTimeMs;
};
