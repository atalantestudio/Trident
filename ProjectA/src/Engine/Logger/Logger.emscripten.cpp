#include "../../../../tsc/src/TSC/Base/Base.hpp"

#include "Logger.hpp"

namespace ProjectA {
	std::ostream* Logger::LOG_LEVEL_OUTPUT_STREAMS[] = {&std::cerr, &std::cerr, &std::cout, &std::cerr, &std::cerr, &std::cerr};

	std::size_t Logger::namePadding = 0;

	std::ostringstream Logger::formatStream;

	Logger Logger::engineLogger("ENGINE", LogLevel::ALL);
	Logger Logger::clientLogger("CLIENT", LogLevel::ALL);

	void Logger::clearFormatStream() {
		formatStream.str(std::string());
	}

	void Logger::clear() {
		stream.str(std::string());
	}

	void Logger::writeTime() {}

	Logger::Logger(std::string_view name, LogLevel minimumLogLevel) : name(name), minimumLogLevel(minimumLogLevel) {
		if (name.size() > namePadding) {
			namePadding = name.size();
		}
	}
}