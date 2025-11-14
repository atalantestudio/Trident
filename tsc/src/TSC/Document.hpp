#pragma once

namespace ProjectA {
	class Document {
		public:
			Document() = default;

			explicit Document(const sequence<char8>& text) :
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

			view<char8> getText() const {
				return text;
			}

			view<char8> getLine(uint64 lineIndex) const {
				ASSERT(lineIndex < lineStarts.size());

				const char8* lineStart = lineStarts[lineIndex];
				const char8* lineEnd = lineEnds[lineIndex];

				return view<char8>(lineStart, lineEnd - lineStart);
			}

		private:
			void storeLines() {
				const view<char8> textView(text);

				uint64 offset = 0;

				while (true) {
					lineStarts.emplace_back(&textView[offset]);

					offset = textView.find('\n', offset);

					if (offset >= textView.count()) {
						break;
					}

					lineEnds.emplace_back(&textView[offset]);

					offset += 1;
				}

				// Push EOF.
				lineEnds.emplace_back(textView.end());
			}

		private:
			sequence<char8> text;

			// NOTE: Also includes END_OF_FILE.
			std::vector<const char8*> lineStarts;
			std::vector<const char8*> lineEnds;
	};
}