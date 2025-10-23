#pragma once

#include "Engine/Filesystem/Filesystem.hpp"

#include "TSC/ASTDebugger.hpp"
#include "TSC/CompilationOutput.hpp"
#include "TSC/GLSLCodeGenerator.hpp"
#include "TSC/Lexer.hpp"
#include "TSC/Parser.hpp"
#include "TSC/Preprocessor.hpp"

namespace ProjectA {
	static sint32 tsc(const char8* path) {
		const sequence<uint8> buffer = Filesystem::openBuffer("Test/test0.tsl");
		// const sequence<uint8> buffer = Filesystem::openBuffer("Test/include.tsl");
		const std::string code(buffer.begin(), buffer.end());

		CompilationOutput output{};
		output.text = code;

		// Preprocessor preprocessor(&output);
		// preprocessor.link(code);

		Lexer lexer;
		lexer.tokenize(&output);

		Parser parser;
		parser.parse(&output);

		bool hasError = false;

		for (const Diagnostic& diagnostic : output.diagnostics) {
			switch (diagnostic.severity) {
				case DiagnosticSeverity::ERROR: {
					ENGINE_ERROR(diagnostic.message);

					hasError = true;

					break;
				}
				case DiagnosticSeverity::WARNING: {
					ENGINE_WARN(diagnostic.message);

					break;
				}
				case DiagnosticSeverity::INFO:
				case DiagnosticSeverity::HINT_OR_WIP: {
					ENGINE_INFO(diagnostic.message);

					break;
				}
			}
		}

		if (hasError) {
			return 1;
		}

		std::ofstream targetStream("Test/test0.glsl");

		DEBUG_ASSERT(targetStream.is_open());

		GLSLCodeGenerator generator;
		generator.write(targetStream, output);

		targetStream.close();

		#if 1
			std::ofstream stream("Test/test0.ast.json");

			DEBUG_ASSERT(stream.is_open());

			ASTDebugger astDebugger;
			astDebugger.write(stream, output);

			stream.close();
		#endif

		return 0;
	}
}