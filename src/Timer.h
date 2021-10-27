#pragma once

#include <chrono>

class Timer
{
public:
	Timer()
	{
		Reset();
	}

	void Reset()
	{
		mStart = std::chrono::high_resolution_clock::now();
	}

	float ElapsedSeconds()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - mStart).count();
	}

	float ElapsedMs()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - mStart).count();
	}

	float ElapsedNs()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - mStart).count();
	}
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
};
