#include "pch.hpp"
#include "ProjectA/Profile/Profiler.hpp"

namespace ProjectA {
	std::ofstream Profiler::stream;
	std::size_t Profiler::eventCount = 0;

	void Profiler::writeEvent(const ProfileEvent& event) {
		if (!stream.is_open()) {
			throw std::runtime_error("Couldn't write to the profiler output stream.");
		}

		if (eventCount > 0) {
			stream << ",";
		}

		std::string name = event.name;
		std::replace(name.begin(), name.end(), '"', '\'');

		// Write event name
		stream << "{\"name\":\"" << name << "\",";

		// Write event category
		stream << "\"cat\":\"Scope\",";

		// Write event type
		stream << "\"ph\":\"X\",";

		// Write event time stamp
		stream << "\"ts\":" << event.start << ',';

		// Write event duration
		stream << "\"dur\":" << (event.end - event.start) << ',';

		// Write process ID
		stream << "\"pid\":0,";

		// Write thread ID
		stream << "\"tid\":" << event.threadId << '}';

		eventCount++;
	}

	Profiler::Profiler(const std::string& path) {
		stream.open(path);

		if (!stream.is_open()) {
			throw std::runtime_error("Couldn't open the profiler output stream.");
		}

		// Write header
		stream << "{\"traceEvents\":[";
	}

	Profiler::~Profiler() {
		// Write footer
		stream << "]}";

		stream.close();

		eventCount = 0;
	}
}