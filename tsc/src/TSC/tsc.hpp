#pragma once

#include "Engine/Filesystem/Filesystem.hpp"

#include "TSC/ASTDebugger.hpp"
#include "TSC/CompilationOutput.hpp"
#include "TSC/GLSLCodeGenerator.hpp"
#include "TSC/Lexer.hpp"
#include "TSC/Parser.hpp"
#include "TSC/Preprocessor.hpp"
#include "TSC/SemanticAnalyzer.hpp"

namespace ProjectA {
	static void writeLine(const CompilationOutput& output, ConsoleLogger& logger, uint32 lineIndex, uint64 maxLineIndex, bool dimmed) {
		if (dimmed) {
			logger << ConsoleEscapeCode::DIMMED;
		}

		logger.padLeft(maxLineIndex) << lineIndex + 1 << logger.format("\t[]\n", output.document.getLine(lineIndex));

		if (dimmed) {
			logger << ConsoleEscapeCode::RESET_BRIGHT;
		}
	}

	static bool logTslErrors(const CompilationOutput& output) {
		const std::string absolutePath = std::filesystem::absolute("Test/test3.tsl").string();

		ConsoleLogger logger(std::cout, LogLevel::ALL, "TSLC");
		bool hasError = false;

		for (const Diagnostic& diagnostic : output.diagnostics) {
			switch (diagnostic.severity) {
				case DiagnosticSeverity::INFO:
				case DiagnosticSeverity::HINT: {
					logger << ConsoleEscapeCode::BACKGROUND_COLOR_LIGHT_BLUE << ConsoleEscapeCode::FOREGROUND_COLOR_WHITE;
					logger << " INFO ";
					logger << ConsoleEscapeCode::RESET_BACKGROUND_COLOR << ConsoleEscapeCode::RESET_FOREGROUND_COLOR;
					logger << logger.format(" []:[]: []\n", absolutePath, diagnostic.range.start, diagnostic.message);

					break;
				}
				case DiagnosticSeverity::WARNING: {
					// TODO: INFO and ERROR look good with BRIGHT/BOLD but for some reason it WARNING.
					logger << ConsoleEscapeCode::BACKGROUND_COLOR_LIGHT_YELLOW << ConsoleEscapeCode::FOREGROUND_COLOR_BLACK;
					logger << " WARNING ";
					logger << ConsoleEscapeCode::RESET_BACKGROUND_COLOR << ConsoleEscapeCode::RESET_FOREGROUND_COLOR;
					logger << logger.format(" []:[]: []\n", absolutePath, diagnostic.range.start, diagnostic.message);

					break;
				}
				case DiagnosticSeverity::ERROR: {
					hasError = true;

					logger << ConsoleEscapeCode::BACKGROUND_COLOR_LIGHT_RED << ConsoleEscapeCode::FOREGROUND_COLOR_WHITE;
					logger << " ERROR ";
					logger << ConsoleEscapeCode::RESET_BACKGROUND_COLOR << ConsoleEscapeCode::RESET_FOREGROUND_COLOR;
					logger << logger.format(" []:[]: []\n", absolutePath, diagnostic.range.start, diagnostic.message);

					break;
				}
			}

			if (diagnostic.severity == DiagnosticSeverity::ERROR && diagnostic.range.start.lineIndex == diagnostic.range.end.lineIndex) {
				logger << '\n';

				const uint64 maxLineIndex = diagnostic.range.end.lineIndex + 1;
				const uint64 maxLineIndexSize = Logger::format(maxLineIndex).count();

				if (diagnostic.range.start.lineIndex > 0) {
					// Write preceding line.
					writeLine(output, logger, diagnostic.range.start.lineIndex - 1, maxLineIndexSize, true);
				}

				writeLine(output, logger, diagnostic.range.start.lineIndex, maxLineIndexSize, false);

				const uint32 characterOffset = diagnostic.range.start.characterOffset;
				const uint32 characterCount = max(static_cast<uint32>(1), diagnostic.range.end.characterOffset - characterOffset);

				logger << ConsoleEscapeCode::FOREGROUND_COLOR_LIGHT_RED;
				logger.padLeft(8 + characterOffset + characterCount) << std::string(characterCount, '^') << '\n';
				logger << ConsoleEscapeCode::RESET_FOREGROUND_COLOR;

				if (diagnostic.range.start.lineIndex < output.document.getLineCount() - 1) {
					// Write following line.
					writeLine(output, logger, diagnostic.range.start.lineIndex + 1, maxLineIndexSize, true);
				}
			}

			logger << '\n';
		}

		// TODO: Set color based on the most important diagnostic found. Use green if there are no diagnostics.
		// TODO: Show compilation time.
		// logger << ConsoleEscapeCode::FOREGROUND_COLOR_LIGHT_GREEN;
		// logger << "Done in []ms.";
		// logger << ConsoleEscapeCode::RESET_FOREGROUND_COLOR;
		// logger << '\n';

		return hasError;
	}

	static sint32 tsc(const char8* path) {
		const sequence<uint8> buffer = Filesystem::openBuffer("Test/test3.tsl");
		const sequence<char8> code = *pointer_cast<const sequence<char8>*>(&buffer);

		CompilationOutput output(code);

		// Preprocessor preprocessor(&output);
		// preprocessor.link(code);

		Lexer lexer;
		lexer.tokenize(&output);

		if (output.tokens.empty()) {
			ENGINE_ERROR("No tokens generated.");

			return 1;
		}

		Parser parser;
		parser.parse(&output);

		SemanticAnalyzer semanticAnalyzer(output);

		const bool hasError = logTslErrors(output);

		if (hasError) {
			return 1;
		}

		// Visualize AST.
		ASTDebugger("Test/test0.ast.json", output);

		// Generate GLSL.
		{
			std::ofstream targetStream("Test/test0.glsl");

			DEBUG_ASSERT(targetStream.is_open());

			GLSLCodeGenerator generator;
			generator.write(targetStream, output);

			targetStream.close();
		}

		return 0;
	}
}