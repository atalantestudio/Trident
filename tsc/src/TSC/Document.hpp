#pragma once

namespace ProjectA {
	class Document {
		public:
			Document() = default;

			explicit Document(const str8& text) :
				text(text)
			{
				ASSERT(text.count() > 0);

				storeLines();
			}

			uint64 getCharacterCount() const {
				return text.count();
			}

			uint64 getLineCount() const {
				return lineStarts.size() - 1;
			}

			const str8& getText() const {
				return text;
			}

			str8 getLine(uint64 lineIndex) const {
				ASSERT(lineIndex < lineStarts.size());

				const char8* lineStart = lineStarts[lineIndex];
				const char8* lineEnd = lineEnds[lineIndex];

				str8 line(lineEnd - lineStart);

				std::copy(lineStart, lineEnd, &line[0]);

				return line;
			}

		private:
			void storeLines() {
				uint64 offset = 0;

				while (true) {
					lineStarts.emplace_back(&text[offset]);

					offset = text.find('\n', offset);

					if (offset >= text.count()) {
						break;
					}

					lineEnds.emplace_back(&text[offset]);

					offset += 1;
				}

				// Push EOF.
				lineEnds.emplace_back(text.end());
			}

		private:
			str8 text;

			// NOTE: Also includes END_OF_FILE.
			std::vector<const char8*> lineStarts;
			std::vector<const char8*> lineEnds;
	};
}