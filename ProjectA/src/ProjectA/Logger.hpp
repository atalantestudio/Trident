#pragma once

#include "core.hpp"

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

namespace ProjectA {
	class API Logger {
		private:
			static std::shared_ptr<spdlog::logger> coreLogger;
			static std::shared_ptr<spdlog::logger> clientLogger;

		public:
			static void initialize();

			static inline std::shared_ptr<spdlog::logger>& getCoreLogger() {
				return coreLogger;
			}

			static inline std::shared_ptr<spdlog::logger>& getClientLogger() {
				return clientLogger;
			}
	};
}

#define LOG_CORE_ERROR(...) ::ProjectA::Logger::getCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_FATAL(...) ::ProjectA::Logger::getCoreLogger()->critical(__VA_ARGS__)
#define LOG_CORE_INFO(...) ::ProjectA::Logger::getCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_TRACE(...) ::ProjectA::Logger::getCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_WARN(...) ::ProjectA::Logger::getCoreLogger()->warn(__VA_ARGS__)

#define LOG_CLIENT_ERROR(...) ::ProjectA::Logger::getClientLogger()->error(__VA_ARGS__)
#define LOG_CLIENT_FATAL(...) ::ProjectA::Logger::getClientLogger()->critical(__VA_ARGS__)
#define LOG_CLIENT_INFO(...) ::ProjectA::Logger::getClientLogger()->info(__VA_ARGS__)
#define LOG_CLIENT_TRACE(...) ::ProjectA::Logger::getClientLogger()->trace(__VA_ARGS__)
#define LOG_CLIENT_WARN(...) ::ProjectA::Logger::getClientLogger()->warn(__VA_ARGS__)