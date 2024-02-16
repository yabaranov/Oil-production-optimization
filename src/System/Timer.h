#pragma once

#include <chrono> 

class Timer
{
private:

	using clock_t = std::chrono::high_resolution_clock;
	
	std::chrono::time_point<clock_t> m_beg;

public:

	Timer() : m_beg(clock_t::now())
	{
	}

	void reset()
	{
		m_beg = clock_t::now();
	}

	std::chrono::hh_mm_ss<std::chrono::milliseconds> elapsed() const
	{
		return std::chrono::hh_mm_ss<std::chrono::milliseconds>{std::chrono::duration_cast<std::chrono::milliseconds>(clock_t::now() - m_beg)};
	}
};