#pragma once

#include "Engine/API/Window/Window.hpp"
#include "Engine/Logger/LogLevel.hpp"
#include "Engine/Logger/LogStructure.hpp"
#include "Engine/Platform.hpp"

namespace ProjectA {
	class Logger {
		private:
			static constexpr std::size_t TIME_PADDING = 14;
			static constexpr std::size_t LEVEL_NAME_PADDING = 5;
			static constexpr uint8 LOG_LEVEL_BACKGROUND_COLORS[] = {0, 0, 0, 0, 101, 107};
			static constexpr uint8 LOG_LEVEL_FOREGROUND_COLORS[] = {97, 37, 94, 93, 97, 30};
			static constexpr const char* LOG_LEVEL_NAMES[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
			static std::ostream* LOG_LEVEL_OUTPUT_STREAMS[];

			static std::size_t namePadding;

			static std::ostringstream formatStream;

		public:
			static Logger engineLogger;
			static Logger clientLogger;

			static constexpr std::string_view argumentInjectionPattern = "[]";

		private:
			std::ostringstream stream;

		public:
			std::string name;
			LogLevel minimumLogLevel;
			LogStructure logStructure = LogStructure::TIME | LogStructure::LOGGER_NAME | LogStructure::LOG_LEVEL_NAME;
			std::size_t spacing = 3;

		public:
			template<typename T>
			static std::string format(T&& argument) {
				formatStream << argument;

				const std::string log = formatStream.str();

				clearFormatStream();

				return log;
			}

			template<typename T, typename... U>
			static std::string format(std::string_view pattern, T&& argument, U&&... arguments) {
				const std::size_t offset = pattern.find(argumentInjectionPattern);

				formatStream << pattern.substr(0, offset);

				if (offset == std::string::npos) {
					const std::string log = formatStream.str();

					clearFormatStream();

					return log;
				}

				formatStream << argument;

				return format(pattern.substr(offset + argumentInjectionPattern.size()), std::forward<U>(arguments)...);
			}

		private:
			static void clearFormatStream();

		private:
			void clear();

			void writeTime();

			template<typename... T>
			void log(LogLevel logLevel, std::string_view pattern, T&&... arguments) {
				if (logLevel < minimumLogLevel) {
					return;
				}

				std::string log = format(pattern, std::forward<T>(arguments)...);
				const sint32 logLevelIndex = static_cast<sint32>(logLevel) - 1;
				const std::string ansiSet = "\033[" + std::to_string(LOG_LEVEL_BACKGROUND_COLORS[logLevelIndex]) + ';' + std::to_string(LOG_LEVEL_FOREGROUND_COLORS[logLevelIndex]) + 'm';
				std::size_t padding = 0;
				std::size_t indentSize = 0;

				stream << ansiSet;

				if (logStructure & LogStructure::TIME) {
					writeTime();

					padding = spacing;
					indentSize += TIME_PADDING + spacing;
				}

				if (logStructure & LogStructure::LOGGER_NAME) {
					stream << std::setw(padding + namePadding);
					stream << name;

					padding = spacing;
					indentSize += namePadding + spacing;
				}

				if (logStructure & LogStructure::LOG_LEVEL_NAME) {
					stream << std::setw(padding + LEVEL_NAME_PADDING);
					stream << LOG_LEVEL_NAMES[logLevelIndex];

					padding = spacing;
					indentSize += LEVEL_NAME_PADDING + spacing;
				}

				const std::string indent = "\033[m\n" + ansiSet + std::string(indentSize, ' ');
				std::size_t offset = log.find('\n');

				while (offset != std::string::npos) {
					log.replace(offset, 1, indent);

					offset = log.find('\n', offset + indent.size());
				}

				stream << std::setw(padding + log.size());
				stream << log;
				stream << "\033[m";
				stream << std::endl;

				*LOG_LEVEL_OUTPUT_STREAMS[logLevelIndex] << stream.str();

				clear();
			}

		public:
			Logger(std::string_view name, LogLevel minimumLogLevel = LogLevel::ALL);

			template<typename T>
			void trace(T&& argument) {
				log(LogLevel::TRACE, argumentInjectionPattern, std::forward<T>(argument));
			}

			template<typename... T>
			void trace(std::string_view pattern, T&&... arguments) {
				log(LogLevel::TRACE, pattern, std::forward<T>(arguments)...);
			}

			template<typename T>
			void debug(T&& argument) {
				log(LogLevel::DEBUG, argumentInjectionPattern, std::forward<T>(argument));
			}

			template<typename... T>
			void debug(std::string_view pattern, T&&... arguments) {
				log(LogLevel::DEBUG, pattern, std::forward<T>(arguments)...);
			}

			template<typename T>
			void info(T&& argument) {
				log(LogLevel::INFO, argumentInjectionPattern, std::forward<T>(argument));
			}

			template<typename... T>
			void info(std::string_view pattern, T&&... arguments) {
				log(LogLevel::INFO, pattern, std::forward<T>(arguments)...);
			}

			template<typename T>
			void warn(T&& argument) {
				log(LogLevel::WARN, argumentInjectionPattern, std::forward<T>(argument));
			}

			template<typename... T>
			void warn(std::string_view pattern, T&&... arguments) {
				log(LogLevel::WARN, pattern, std::forward<T>(arguments)...);
			}

			template<typename T>
			void error(T&& argument) {
				log(LogLevel::ERROR, argumentInjectionPattern, std::forward<T>(argument));
			}

			template<typename... T>
			void error(std::string_view pattern, T&&... arguments) {
				log(LogLevel::ERROR, pattern, std::forward<T>(arguments)...);
			}

			template<typename... T>
			void fatal(uint32 code, std::string_view pattern, T&&... arguments) {
				const std::string message = format(pattern, std::forward<T>(arguments)...);

				log(LogLevel::FATAL, message);
				Platform::fatalExit(code, message);
			}
	};
}