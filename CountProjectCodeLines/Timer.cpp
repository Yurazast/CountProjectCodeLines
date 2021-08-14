#include "Timer.h"

Timer::Timer()
		: m_start_time_point{}
		, m_end_time_point{}
{}

void Timer::Start()
{
	m_start_time_point = std::chrono::high_resolution_clock::now();
}

void Timer::Stop()
{
	m_end_time_point = std::chrono::high_resolution_clock::now();
}

void Timer::Reset()
{
	m_start_time_point = m_end_time_point = {};
}

std::ostream& operator<<(std::ostream& os, Timer& timer)
{
	if (timer.m_start_time_point.time_since_epoch().count() == 0)
	{
		os << "Timer was not started" << std::endl;
		return os;
	}

	if (timer.m_end_time_point.time_since_epoch().count() == 0)
	{
		timer.Stop();
	}

	os.imbue(std::locale(""));
	os << "Execution time: " << std::chrono::duration_cast<std::chrono::microseconds>
		(timer.m_end_time_point - timer.m_start_time_point).count() << " microseconds" << std::endl;
	return os;
}
