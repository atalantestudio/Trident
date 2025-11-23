#include "pch.hpp"

#include "Engine/Filesystem/FileWriter/FileWriter.hpp"

namespace ProjectA {
	template<>
	class FileWriter<FileFormat::JSON> : public BaseFileWriter {
		public:
			explicit FileWriter(view<char8> path, uint32 tabSize) :
				BaseFileWriter(path),
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
			void property(view<char8> name, Argument&& valueArgument) {
				propertyStart();

				writeLine(Logger::format("\"[]\": \"[]\"", name, std::forward<Argument>(valueArgument)));
			}

			template<typename Argument>
			void propertyNonString(view<char8> name, Argument&& valueArgument) {
				propertyStart();

				writeLine(Logger::format("\"[]\": []", name, std::forward<Argument>(valueArgument)));
			}

			template<typename... Argument>
			void property(view<char8> name, view<char8> valuePattern, Argument&&... valueArguments) {
				propertyStart();

				writeLine(Logger::format("\"[]\": []", name, Logger::format(valuePattern, std::forward<Argument>(arguments)...)));
			}

			void propertyArray(view<char8> name) {
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

			void propertyObject(view<char8> name) {
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

			void writeLine(view<char8> value) {
				stream << std::setw(currentScopeIndex * tabSize + value.count()) << &value[0];
			}

			bool isDigitOnly(view<char8> value) {
				for (const char8 character : value) {
					if (character < '0' || character > '9') {
						return false;
					}
				}

				return true;
			}

		private:
			uint32 tabSize;
			uint32 currentScopeIndex = 0;
			std::vector<uint32> itemsPerScope = {0};
	};
}