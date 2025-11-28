#pragma once

#include "Engine/Process/Process.hpp"

namespace ProjectA {
	class BuildShaderModuleProcess : public Process {
		private:
			sint32 processFile(view<char8> path) {
				const sequence<char8> text = Filesystem::openTextBuffer(path);

				CompilationInput input{};
				input.path = path;
				input.text = text;

				CompilationOutput output(input);
				output.path = path;

				// TODO
				// Preprocessor preprocessor(&output);
				// preprocessor.link(text);

				Lexer lexer;
				lexer.tokenize(&output);

				if (output.tokens.empty()) {
					ENGINE_ERROR("No tokens generated.");

					return 1;
				}

				Parser parser;
				parser.parse(&output);

				SemanticAnalyzer semanticAnalyzer(output);

				const bool hasError = logDiagnostics(output);

				if (hasError) {
					return 1;
				}

				// Visualize AST.
				// ASTDebugger("Test/test.ast.json", output);

				/* std::ofstream targetStream("Test/test.glsl");

				DEBUG_ASSERT(targetStream.is_open());

				// Generate GLSL.
				GLSLGenerator generator;
				generator.write(targetStream, output);

				targetStream.close(); */

				return 0;
			}

			bool logDiagnostics(const CompilationOutput& output) {
				bool hasError = false;

				for (const Diagnostic& diagnostic : output.diagnostics) {
					switch (diagnostic.severity) {
						case DiagnosticSeverity::INFO:
						case DiagnosticSeverity::HINT: {
							logger << ConsoleEscapeCode::BACKGROUND_COLOR_LIGHT_BLUE << ConsoleEscapeCode::FOREGROUND_COLOR_WHITE << " INFO ";

							break;
						}
						case DiagnosticSeverity::WARNING: {
							logger << ConsoleEscapeCode::BACKGROUND_COLOR_LIGHT_YELLOW << ConsoleEscapeCode::FOREGROUND_COLOR_BLACK << " WARNING ";

							break;
						}
						case DiagnosticSeverity::ERROR: {
							logger << ConsoleEscapeCode::BACKGROUND_COLOR_LIGHT_RED << ConsoleEscapeCode::FOREGROUND_COLOR_WHITE << " ERROR ";

							hasError = true;

							break;
						}
					}

					logger << ConsoleEscapeCode::RESET_BACKGROUND_COLOR << ConsoleEscapeCode::RESET_FOREGROUND_COLOR;
					logger << ' ' << diagnostic.message << ' ' << ConsoleEscapeCode::FOREGROUND_COLOR_LIGHT_GRAY << Logger::format("[]:[]", output.path, diagnostic.range.start);
					logger << ConsoleEscapeCode::RESET_FOREGROUND_COLOR << '\n';

					if (diagnostic.severity == DiagnosticSeverity::ERROR && diagnostic.range.start.lineIndex == diagnostic.range.end.lineIndex) {
						logger << '\n';

						const uint64 maxLineIndex = diagnostic.range.end.lineIndex + 1;
						const uint64 maxLineIndexSize = toString(maxLineIndex).size();

						if (diagnostic.range.start.lineIndex > 0) {
							writeLine(output, diagnostic.range.start.lineIndex - 1, maxLineIndexSize, true);
						}

						writeLine(output, diagnostic.range.start.lineIndex, maxLineIndexSize, false);

						const uint32 characterOffset = diagnostic.range.start.characterOffset;
						const uint32 characterCount = static_cast<uint32>(max(static_cast<sint32>(1), static_cast<sint32>(diagnostic.range.end.characterOffset) - static_cast<sint32>(characterOffset)));

						logger << ConsoleEscapeCode::FOREGROUND_COLOR_LIGHT_RED;
						logger.padLeft(8 + characterOffset + characterCount) << std::string(characterCount, '^') << '\n';
						logger << ConsoleEscapeCode::RESET_FOREGROUND_COLOR;

						const uint32 nextLineIndex = diagnostic.range.start.lineIndex + 1;

						if (nextLineIndex < output.document.getLineCount()) {
							writeLine(output, nextLineIndex, maxLineIndexSize, true);
						}
					}

					logger << '\n';
				}

				return hasError;
			}

			void writeLine(const CompilationOutput& output, uint32 lineIndex, uint64 maxLineIndex, bool dimmed) {
				if (dimmed) {
					logger << ConsoleEscapeCode::DIMMED;
				}

				logger.padLeft(maxLineIndex) << lineIndex + 1 << '\t' << output.document.getLine(lineIndex);

				if (dimmed) {
					logger << ConsoleEscapeCode::RESET_BRIGHT;
				}

				logger << '\n';
			}

		private:
			static constexpr uint32 PROJECT_INPUT_PATH_ARGUMENT_INDEX = 1;
			static constexpr uint32 PROJECT_OUTPUT_PATH_ARGUMENT_INDEX = 2;

		public:
			explicit BuildShaderModuleProcess() :
				logger(std::cout, LogLevel::ALL, "")
			{}

			sint32 main(const sequence<void*>& arguments) override {
				const std::filesystem::path inputPath = pointer_cast<const char8*>(arguments[PROJECT_INPUT_PATH_ARGUMENT_INDEX]);
				const std::filesystem::path outputPath = pointer_cast<const char8*>(arguments[PROJECT_OUTPUT_PATH_ARGUMENT_INDEX]);
				const std::filesystem::recursive_directory_iterator shaderDirectoryIterator(inputPath / "Asset/Shader");
				const uint64 start = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();

				logger << '\n';

				sint32 code = 0;

				for (const std::filesystem::directory_entry& directoryEntry : shaderDirectoryIterator) {
					if (directoryEntry.is_directory()) {
						continue;
					}

					// TODO: Extract asset information from a pre-built manifest file.
					if (directoryEntry.path().extension() != ".tsl") {
						continue;
					}

					const std::string path = directoryEntry.path().string();

					code = processFile(view<char8>(&path[0], path.size()));
				}

				const uint64 end = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();

				if (code == 0) {
					logger << ConsoleEscapeCode::FOREGROUND_COLOR_LIGHT_GREEN;
				}
				else {
					logger << ConsoleEscapeCode::FOREGROUND_COLOR_LIGHT_RED;
				}

				logger << Logger::format("Done in []ms.", end - start) << ConsoleEscapeCode::RESET_FOREGROUND_COLOR << "\n\n";

				return code;
			}

		private:
			ConsoleLogger logger;
	};
}