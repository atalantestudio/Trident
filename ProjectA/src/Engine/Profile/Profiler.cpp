#include "pch.hpp"

#include "Engine/Profile/Profiler.hpp"

namespace ProjectA {
	std::ofstream Profiler::stream;
	bool Profiler::firstEvent = true;

	void Profiler::writeEvent(const ProfileEvent& event) {
		// TODO(review): Reproduce and fix crash that happens when closing the window instead of the console.
		ENGINE_ASSERT(stream.is_open(), "Could not write to the profiler output stream.");

		if (!firstEvent) {
			stream << ",";
		}

		std::string name = event.name;

		std::replace(name.begin(), name.end(), '"', '\'');

		// Write event name.
		stream << "{\"name\":\"" << name << "\",";

		// Write event category.
		stream << "\"cat\":\"Scope\",";

		// Write event type.
		stream << "\"ph\":\"X\",";

		// Write event time stamp.
		stream << "\"ts\":" << event.start << ',';

		// Write event duration.
		stream << "\"dur\":" << (event.end - event.start) << ',';

		// Write process ID.
		stream << "\"pid\":0,";

		// Write thread ID.
		stream << "\"tid\":" << event.threadId << '}';

		firstEvent = false;
	}

	Profiler::Profiler(std::string_view path) {
		stream.open(path.data());

		ENGINE_ASSERT(stream.is_open(), "Could not open the profiler output stream.");

		// Write header.
		stream << "{\"traceEvents\":[";
	}

	Profiler::~Profiler() {
		// Write footer.
		stream << "]}";

		stream.close();
	}
}