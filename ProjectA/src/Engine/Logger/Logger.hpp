#pragma once

#include "Engine/API/Window/Window.hpp"
#include "Engine/Logger/LogLevel.hpp"
#include "Engine/Logger/LogStructure.hpp"
#include "Engine/Platform.hpp"

namespace ProjectA {
	class Logger {
		public:
			template<typename CurrentArgument>
			static std::string format(CurrentArgument&& argument) {
				formatStream << argument;

				const std::string log = formatStream.str();

				clearFormatStream();

				return log;
			}

			template<typename CurrentArgument, typename... Argument>
			static std::string format(std::string_view pattern, CurrentArgument&& argument, Argument&&... arguments) {
				const std::size_t offset = pattern.find(argumentInjectionPattern);

				formatStream << pattern.substr(0, offset);

				if (offset == std::string::npos) {
					const std::string log = formatStream.str();

					clearFormatStream();

					return log;
				}

				formatStream << argument;

				return format(pattern.substr(offset + argumentInjectionPattern.size()), std::forward<Argument>(arguments)...);
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
				stream << "\033[m\n";

				*LOG_LEVEL_OUTPUT_STREAMS[logLevelIndex] << stream.str();

				clear();
			}

		public:
			Logger(std::string_view name, LogLevel minimumLogLevel = LogLevel::ALL);

			template<typename Argument>
			void trace(Argument&& argument) {
				log(LogLevel::TRACE, argumentInjectionPattern, std::forward<Argument>(argument));
			}

			template<typename... Argument>
			void trace(std::string_view pattern, Argument&&... arguments) {
				log(LogLevel::TRACE, pattern, std::forward<Argument>(arguments)...);
			}

			template<typename Argument>
			void debug(Argument&& argument) {
				log(LogLevel::DEBUG, argumentInjectionPattern, std::forward<Argument>(argument));
			}

			template<typename... Argument>
			void debug(std::string_view pattern, Argument&&... arguments) {
				log(LogLevel::DEBUG, pattern, std::forward<Argument>(arguments)...);
			}

			template<typename Argument>
			void info(Argument&& argument) {
				log(LogLevel::INFO, argumentInjectionPattern, std::forward<Argument>(argument));
			}

			template<typename... Argument>
			void info(std::string_view pattern, Argument&&... arguments) {
				log(LogLevel::INFO, pattern, std::forward<Argument>(arguments)...);
			}

			template<typename Argument>
			void warn(Argument&& argument) {
				log(LogLevel::WARN, argumentInjectionPattern, std::forward<Argument>(argument));
			}

			template<typename... Argument>
			void warn(std::string_view pattern, Argument&&... arguments) {
				log(LogLevel::WARN, pattern, std::forward<Argument>(arguments)...);
			}

			template<typename Argument>
			void error(Argument&& argument) {
				log(LogLevel::ERROR, argumentInjectionPattern, std::forward<Argument>(argument));
			}

			template<typename... Argument>
			void error(std::string_view pattern, Argument&&... arguments) {
				log(LogLevel::ERROR, pattern, std::forward<Argument>(arguments)...);
			}

			template<typename... Argument>
			void fatal(uint32 code, std::string_view pattern, Argument&&... arguments) {
				const std::string message = format(pattern, std::forward<Argument>(arguments)...);

				log(LogLevel::FATAL, message);
				Platform::fatalExit(code, message);
			}

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
	};
}