#include "pch.hpp"

#include "Engine/Profile/Profiler.hpp"
#include "Engine/Profile/ProfileTimer.hpp"

namespace ProjectA {
	ProfileTimer::ProfileTimer(const std::string& name) :
		event({
			name,
			std::this_thread::get_id(),
			std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count(),
			// NOTE: The event's end is set on the timer's destruction.
			0,
		})
	{}

	ProfileTimer::~ProfileTimer() {
		std::chrono::time_point<std::chrono::system_clock> endTimePoint = std::chrono::system_clock::now();

		event.end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

		Profiler::writeEvent(event);
	}
}