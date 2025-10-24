#pragma once

#include "CompilationOutput.hpp"

namespace ProjectA {
	class Preprocessor {
		public:
			explicit Preprocessor(CompilationOutput* output);

			void link(const std::string& code);

		private:
			void jump(uint32 characterCount);

			char8 read() const;
			char8 peek(uint32 index) const;

			bool testBlank(char8 character) const;
			bool testLowerCaseLetter(char8 character) const;

		private:
			CompilationOutput* output;
			const std::string* code;
			uint32 characterIndex;
			DocumentPosition position;
	};
}