#include "pch.hpp"

#include "Engine/Logger/FileLogger/FileLogger.hpp"

namespace ProjectA {
	template<>
	class FileLogger<FileFormat::JSON> : public BaseFileLogger {
		public:
			explicit FileLogger(std::string_view path, uint32 tabSize) :
				BaseFileLogger(path),
				tabSize(tabSize)
			{
				DEBUG_ASSERT(tabSize > 0);
			}

			void openArray() {
				DEBUG_ASSERT(itemsPerScope[0] <= 1);

				if (itemsPerScope[currentScopeIndex] > 0) {
					stream << ',';
				}

				if (currentScopeIndex > 0) {
					stream << '\n';
				}

				itemsPerScope[currentScopeIndex] += 1;

				writeLine("[");
				indent();
			}

			void closeArray() {
				stream << '\n';

				unindent();
				writeLine("]");
			}

			void openObject() {
				DEBUG_ASSERT(itemsPerScope[0] <= 1);

				if (itemsPerScope[currentScopeIndex] > 0) {
					stream << ',';
				}

				if (currentScopeIndex > 0) {
					stream << '\n';
				}

				itemsPerScope[currentScopeIndex] += 1;

				writeLine("{");
				indent();
			}

			void closeObject() {
				stream << '\n';

				unindent();
				writeLine("}");
			}

			template<typename Argument>
			void property(std::string_view name, Argument&& valueArgument) {
				propertyStart();

				// TODO: Remove " if the value is not meant to be a string.
				writeLine(Logger::format("\"[]\": \"[]\"", name, std::forward<Argument>(valueArgument)));
			}

			template<typename... Argument>
			void property(std::string_view name, std::string_view valuePattern, Argument&&... valueArguments) {
				propertyStart();

				writeLine(Logger::format("\"[]\": []", name, Logger::format(pattern, std::forward<Argument>(arguments)...)));
			}

			void propertyArray(std::string_view name) {
				DEBUG_ASSERT(itemsPerScope[0] <= 1);

				if (itemsPerScope[currentScopeIndex] > 0) {
					stream << ',';
				}

				itemsPerScope[currentScopeIndex] += 1;

				stream << '\n';
				writeLine(Logger::format("\"[]\": ", name));
				stream << "[";

				indent();
			}

			void propertyObject(std::string_view name) {
				DEBUG_ASSERT(itemsPerScope[0] <= 1);

				if (itemsPerScope[currentScopeIndex] > 0) {
					stream << ',';
				}

				itemsPerScope[currentScopeIndex] += 1;

				stream << '\n';
				writeLine(Logger::format("\"[]\": ", name));
				stream << '{';

				indent();
			}

		private:
			void indent() {
				itemsPerScope.emplace_back(0);
				currentScopeIndex += 1;
			}

			void unindent() {
				DEBUG_ASSERT(currentScopeIndex > 0);

				itemsPerScope.pop_back();
				currentScopeIndex -= 1;
			}

			void propertyStart() {
				DEBUG_ASSERT(itemsPerScope[0] <= 1);

				if (itemsPerScope[currentScopeIndex] > 0) {
					stream << ',';
				}

				itemsPerScope[currentScopeIndex] += 1;

				stream << '\n';
			}

			void writeLine(const std::string& value) {
				stream << std::setw(currentScopeIndex * tabSize + value.size()) << value;
			}

		private:
			uint32 tabSize;
			uint32 currentScopeIndex = 0;
			std::vector<uint32> itemsPerScope = {0};
	};
}