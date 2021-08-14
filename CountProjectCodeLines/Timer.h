#pragma once

#include <ostream>
#include <chrono>

class Timer
{
public:
	Timer();

	void Start();
	void Stop();
	void Reset();

private:
	friend std::ostream& operator<<(std::ostream& os, const Timer& m_timer);

	std::chrono::high_resolution_clock::time_point m_start_time_point;
	std::chrono::high_resolution_clock::time_point m_end_time_point;
};

