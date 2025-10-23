#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "Diagnostic/Error.hpp"
#include "Preprocessor.hpp"
#include "Statement.hpp"

namespace ProjectA {
	Preprocessor::Preprocessor(CompilationOutput* _output) {
		output = _output;
	}

	void Preprocessor::link(const std::string& code) {
		characterIndex = 0;
		position.lineIndex = 0;
		position.characterOffset = 0;

		this->code = &code;

		std::vector<IncludeStatement> includeStatements;

		while (read() != '\0') {
			if (read() == '/' && peek(1) == '/') {
				jump(2);

				while (read() != '\n' && read() != '\0') {
					jump(1);
				}
			}

			if (testLowerCaseLetter(read())) {
				const DocumentPosition includeKeywordStart = position;

				jump(1);

				while (testLowerCaseLetter(read())) {
					jump(1);
				}

				const DocumentPosition includeKeywordEnd = position;
				const std::string identifier(&code[includeKeywordStart.characterOffset], includeKeywordEnd.characterOffset - includeKeywordStart.characterOffset);

				if (identifier != "include") {
					continue;
				}

				if (!testBlank(read())) {
					generateDiagnostic<DiagnosticCode::UNEXPECTED_CHARACTER>(*output, read(), position);

					continue;
				}

				jump(1);

				while (testBlank(read())) {
					jump(1);
				}

				const DocumentPosition includePathStart = position;

				while (read() != '\r' && read() != '\n' && read() != '\0') {
					jump(1);
				}

				const uint32 characterCount = position.characterOffset - includePathStart.characterOffset;

				if (characterCount == 0) {
					generateDiagnostic<DiagnosticCode::MISSING_INCLUDE_PATH>(*output, includeKeywordStart, includeKeywordEnd);

					continue;
				}

				IncludeStatement statement{};
				statement.start = includePathStart;
				statement.end = position;
				statement.path = std::string(&code[includePathStart.characterOffset], position.characterOffset - includePathStart.characterOffset);

				includeStatements.emplace_back(statement);
			}

			jump(1);
		}

		for (uint32 includeStatementIndex = 0; includeStatementIndex < includeStatements.size(); includeStatementIndex += 1) {
			const IncludeStatement& includeStatement = includeStatements[includeStatementIndex];

			// Try opening the included file.
			std::ifstream stream(includeStatement.path);

			if (!stream.is_open()) {
				generateDiagnostic<DiagnosticCode::INCLUDE_FILE_NOT_FOUND>(*output, includeStatement);

				continue;
			}

			BREAKPOINT();

			// Find the includes referenced in the file.
		}

		BREAKPOINT();

		// 2. Validate includes.

		// 3. Merge included codes and remove include statements.

		// 4. Return linked code.
	}

	void Preprocessor::jump(uint32 characterCount) {
		characterIndex += characterCount;
		position.characterOffset += characterCount;
	}

	char8 Preprocessor::read() const {
		return peek(0);
	}

	char8 Preprocessor::peek(uint32 index) const {
		if (characterIndex + index >= code->size()) {
			return '\0';
		}

		return (*code)[characterIndex + index];
	}

	bool Preprocessor::testBlank(char8 character) const {
		return character == ' ' || character == '\t';
	}

	bool Preprocessor::testLowerCaseLetter(char8 character) const {
		return character >= 'a' && character <= 'z';
	}
}