// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Profile/Profiler.hpp"

namespace Trident {
	std::ofstream Profiler::stream;
	bool Profiler::firstEvent = true;

	void Profiler::writeEvent(const ProfileEvent& event) {
		// TODO(profiler): Reproduce and fix crash that happens when closing the window instead of the console.
		ENGINE_ASSERT(stream.is_open(), "Could not write to the profiler output stream.");

		if (!firstEvent) {
			stream << ",";
		}

		const sequence<char8> name = replace(event.name, "\"", "'");

		stream << "{\"name\": \"" << name << "\",";
		stream << "\"cat\": \"Scope\",";
		stream << "\"ph\": \"X\",";
		stream << "\"ts\": " << event.start << ',';
		stream << "\"dur\": " << (event.end - event.start) << ',';
		stream << "\"pid\": 0,";
		stream << "\"tid\": " << event.threadId << '}';

		firstEvent = false;
	}

	Profiler::Profiler(view<char8> path) {
		stream.open(&path[0]);

		ENGINE_ASSERT(stream.is_open(), "Failed to open the profiler output stream.");

		stream << "{\"traceEvents\":[";
	}

	Profiler::~Profiler() {
		stream << "]}";

		stream.close();

		firstEvent = true;
	}
}