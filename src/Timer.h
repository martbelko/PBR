#pragma once

#include <chrono>

class Timer
{
public:
	Timer()
	{
		reset();
	}

	void reset()
	{
		mStart = std::chrono::high_resolution_clock::now();
	}

	float elapsedSeconds()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - mStart).count();
	}

	float elapsedMs()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - mStart).count();
	}

	float elapsedNs()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - mStart).count();
	}
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
};
