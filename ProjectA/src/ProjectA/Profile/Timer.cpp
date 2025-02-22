#include "pch.hpp"

#include "ProjectA/Profile/Profiler.hpp"
#include "ProjectA/Profile/Timer.hpp"

namespace ProjectA {
	Timer::Timer(const std::string& name) :
		event({
			name,
			std::this_thread::get_id(),
			std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count(),
			0, // Event end is set on the timer's destruction
		})
	{}

	Timer::~Timer() {
		std::chrono::time_point<std::chrono::system_clock> endTimePoint = std::chrono::system_clock::now();

		event.end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

		Profiler::writeEvent(event);
	}
}