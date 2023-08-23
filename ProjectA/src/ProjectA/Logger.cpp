#include "Logger.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace ProjectA {
	std::shared_ptr<spdlog::logger> Logger::coreLogger;
	std::shared_ptr<spdlog::logger> Logger::clientLogger;

	void Logger::initialize() {
		spdlog::set_pattern("%^[%T] %n: %v%$");

		coreLogger = spdlog::stdout_color_mt("CORE");
		coreLogger->set_level(spdlog::level::trace);

		clientLogger = spdlog::stdout_color_mt("CLIENT");
		clientLogger->set_level(spdlog::level::trace);
	}

	std::shared_ptr<spdlog::logger>& Logger::getCoreLogger() {
		return coreLogger;
	}

	std::shared_ptr<spdlog::logger>& Logger::getClientLogger() {
		return clientLogger;
	}
}