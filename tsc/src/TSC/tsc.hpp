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
			logger.writeEscapeCodes(ConsoleEscapeCode::DIMMED);
		}

		logger.getOutputStream() << std::setw(maxLineIndex) << lineIndex + 1;
		logger.write(logger.format("\t[]\n", output.document.getLine(lineIndex)));

		if (dimmed) {
			logger.writeEscapeCodes(ConsoleEscapeCode::RESET_BRIGHT);
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
					logger.writeEscapeCodes(ConsoleEscapeCode::BACKGROUND_COLOR_LIGHT_BLUE, ConsoleEscapeCode::FOREGROUND_COLOR_WHITE);
					logger.write(" INFO ");
					logger.writeEscapeCodes(ConsoleEscapeCode::RESET_BACKGROUND_COLOR, ConsoleEscapeCode::RESET_FOREGROUND_COLOR);
					logger.write(logger.format(" []:[]: []\n", absolutePath, diagnostic.range.start, diagnostic.message));

					break;
				}
				case DiagnosticSeverity::WARNING: {
					// TODO: INFO and ERROR look good with BRIGHT/BOLD but for some reason it WARNING.
					logger.writeEscapeCodes(ConsoleEscapeCode::BACKGROUND_COLOR_LIGHT_YELLOW, ConsoleEscapeCode::FOREGROUND_COLOR_BLACK);
					logger.write(" WARNING ");
					logger.writeEscapeCodes(ConsoleEscapeCode::RESET_BACKGROUND_COLOR, ConsoleEscapeCode::RESET_FOREGROUND_COLOR);
					logger.write(logger.format(" []:[]: []\n", absolutePath, diagnostic.range.start, diagnostic.message));

					break;
				}
				case DiagnosticSeverity::ERROR: {
					hasError = true;

					logger.writeEscapeCodes(ConsoleEscapeCode::BACKGROUND_COLOR_LIGHT_RED, ConsoleEscapeCode::FOREGROUND_COLOR_WHITE);
					logger.write(" ERROR ");
					logger.writeEscapeCodes(ConsoleEscapeCode::RESET_BACKGROUND_COLOR, ConsoleEscapeCode::RESET_FOREGROUND_COLOR);
					logger.write(logger.format(" []:[]: []\n", absolutePath, diagnostic.range.start, diagnostic.message));

					break;
				}
			}

			if (diagnostic.severity == DiagnosticSeverity::ERROR && diagnostic.range.start.lineIndex == diagnostic.range.end.lineIndex) {
				logger.write("\n");

				uint64 maxLineIndex = diagnostic.range.start.lineIndex;

				if (diagnostic.range.start.lineIndex < output.document.getLineCount() - 1) {
					maxLineIndex += 1;
				}

				const uint64 maxLineIndexSize = Logger::format(maxLineIndex).count();

				if (diagnostic.range.start.lineIndex > 0) {
					// Write preceding line.
					writeLine(output, logger, diagnostic.range.start.lineIndex - 1, maxLineIndexSize, true);
				}

				writeLine(output, logger, diagnostic.range.start.lineIndex, maxLineIndexSize, false);

				logger.writeEscapeCodes(ConsoleEscapeCode::FOREGROUND_COLOR_LIGHT_RED);
				logger.getOutputStream() << std::setw(8 + diagnostic.range.end.characterOffset) << std::string(max(static_cast<uint32>(1), diagnostic.range.end.characterOffset - diagnostic.range.start.characterOffset), '^') << '\n';
				logger.writeEscapeCodes(ConsoleEscapeCode::RESET_FOREGROUND_COLOR);

				if (diagnostic.range.start.lineIndex < output.document.getLineCount() - 1) {
					// Write following line.
					writeLine(output, logger, diagnostic.range.start.lineIndex + 1, maxLineIndexSize, true);
				}
			}

			logger.write("\n");
		}

		// TODO: Set color based on the most important diagnostic found. Use green if there are no diagnostics.
		// TODO: Show compilation time.
		// logger.writeEscapeCodes(ConsoleEscapeCode::FOREGROUND_COLOR_LIGHT_GREEN);
		// logger.write("Done in []ms.");
		// logger.writeEscapeCodes(ConsoleEscapeCode::RESET_FOREGROUND_COLOR);

		logger.write("\n");

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