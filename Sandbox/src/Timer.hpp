#pragma once

#include <chrono>

#include "ProjectA.hpp"

template<typename Callback>
class Timer {
	private:
		const char* name;
		std::chrono::time_point<std::chrono::steady_clock> startTimepoint;
		bool paused = false;
		Callback callback;

	public:
		Timer(const char* name, Callback&& callback) :
			name(name),
			startTimepoint(std::chrono::high_resolution_clock::now()),
			callback(callback)
		{}

		~Timer() {
			if (!paused) {
				pause();
			}
		}

		void pause() {
			std::chrono::time_point<std::chrono::steady_clock> endTimepoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
			float duration = (end - start) * 0.001f;

			paused = true;

			callback({name, duration});
		}
};