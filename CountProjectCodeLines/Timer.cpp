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

std::ostream& operator<<(std::ostream& os, const Timer& m_timer)
{
	if (m_timer.m_start_time_point.time_since_epoch().count() == 0)
	{
		os << "Not started!";
		return os;
	}

	if (m_timer.m_end_time_point.time_since_epoch().count() == 0)
	{
		const_cast<Timer&>(m_timer).Stop();
	}

	os << std::chrono::duration_cast<std::chrono::milliseconds> (m_timer.m_end_time_point - m_timer.m_start_time_point).count();
	return os;
}
