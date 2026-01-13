#pragma once

#include "Engine/Process/Process.hpp"

#include "tsl/AST/ASTDebugger.hpp"
#include "tsl/compile.hpp"
#include "tsl/Generator/GLSLGenerator.hpp"

namespace ProjectA {
	class BuildShaderModuleProcess : public Process {
		private:
			sint32 processFile(const std::filesystem::directory_entry& directoryEntry, const std::filesystem::path& assetOutputPath) {
				const sequence<char8> path = directoryEntry.path().string();
				const sequence<char8> inputData = Filesystem::openTextBuffer(path);

				CompilationInput input{};
				input.path = path;
				input.text = inputData;

				CompilationContext context{};
				context.document.text = input.text;
				context.document.updateLines();

				tsl::compile(context);

				const bool hasError = logDiagnostics(path, context);

				if (hasError) {
					return 1;
				}

				const std::string astPathString = std::filesystem::path(directoryEntry.path()).replace_extension("ast.json").string();
				const view<char8> astPath = view<char8>(&astPathString[0], astPathString.size());

				tsl::ASTDebugger(astPath, context);

				tsl::ShaderModuleAssetDescriptor glslDescriptor{};

				tsl::GLSLGenerator(context, glslDescriptor);

				{
					const std::string glslPathString = std::filesystem::path(directoryEntry.path()).replace_extension("glsl").string();

					std::ofstream stream(glslPathString);

					stream << glslDescriptor.glslCode;

					stream.close();
				}

				ShaderModuleAssetDescriptor shaderModuleAssetDescriptor{};
				shaderModuleAssetDescriptor.identifier = AssetLibrary::createAssetIdentifier();
				shaderModuleAssetDescriptor.version = {1, 0, 0};
				shaderModuleAssetDescriptor.glslCode = glslDescriptor.glslCode;
				shaderModuleAssetDescriptor.glslEntryPointActivationOffsets = glslDescriptor.glslEntryPointActivationOffsets;

				uint64 outputSize = sizeof(AssetSignature) + sizeof(VersionNumber) + 2 * sizeof(uint32);

				for (const auto& glslEntryPointActivationOffsetEntry : shaderModuleAssetDescriptor.glslEntryPointActivationOffsets) {
					outputSize += glslEntryPointActivationOffsetEntry.first.count() + 2 * sizeof(uint32);
				}

				outputSize += sizeof(uint32) + shaderModuleAssetDescriptor.glslCode.count();

				const uint32 entryPointCount = static_cast<uint32>(shaderModuleAssetDescriptor.glslEntryPointActivationOffsets.size());

				ByteStream<StreamOperation::WRITE> outputByteStream(outputSize, Endianness::LITTLE_ENDIAN);
				outputByteStream.write<AssetSignature>(AssetSignature::TSM);
				outputByteStream.write<uint32>(shaderModuleAssetDescriptor.identifier);
				outputByteStream.write<VersionNumber>({0, 0, 0});
				outputByteStream.write<uint32>(static_cast<uint32>(shaderModuleAssetDescriptor.glslCode.count()));

				for (const char8 character : shaderModuleAssetDescriptor.glslCode) {
					outputByteStream.write<char8>(character);
				}

				outputByteStream.write<uint32>(entryPointCount);

				for (const auto& glslEntryPointActivationOffsetEntry : shaderModuleAssetDescriptor.glslEntryPointActivationOffsets) {
					outputByteStream.write<uint32>(static_cast<uint32>(glslEntryPointActivationOffsetEntry.first.count()));

					for (const char8 character : glslEntryPointActivationOffsetEntry.first) {
						outputByteStream.write<char8>(character);
					}
				}

				for (const auto& glslEntryPointActivationOffsetEntry : shaderModuleAssetDescriptor.glslEntryPointActivationOffsets) {
					outputByteStream.write<uint32>(glslEntryPointActivationOffsetEntry.second);
				}

				const std::string stem = directoryEntry.path().stem().string();
				const std::string assetOutputPathString = assetOutputPath.string();

				// Count '\0'.
				sequence<char8> outputPath(assetOutputPathString.size() + stem.size() + 12);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy_n("Shader/", 7, &outputPath[assetOutputPathString.size()]);
				std::copy(stem.begin(), stem.end(), &outputPath[assetOutputPathString.size() + 7]);
				std::copy_n(".tsm", 4, &outputPath[assetOutputPathString.size() + stem.size() + 7]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(outputByteStream.data, outputByteStream.size);

				ENGINE_INFO("Generated shader module asset \"[]\" (ID: []).", outputPath, shaderModuleAssetDescriptor.identifier);

				return 0;
			}

			bool logDiagnostics(view<char8> uri, const CompilationContext& context) {
				bool hasError = false;

				for (const Diagnostic& diagnostic : context.diagnostics) {
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
					logger << ' ' << diagnostic.message << ' ';
					logger << ConsoleEscapeCode::FOREGROUND_COLOR_LIGHT_GRAY << uri << ':' << diagnostic.range.start.lineIndex + 1 << ':' << diagnostic.range.start.characterOffset + 1 << ConsoleEscapeCode::RESET_FOREGROUND_COLOR << '\n';

					if (diagnostic.severity == DiagnosticSeverity::ERROR && diagnostic.range.start.lineIndex == diagnostic.range.end.lineIndex) {
						logger << '\n';

						const uint64 maxLineIndex = diagnostic.range.end.lineIndex + 1;
						const uint64 maxLineIndexSize = toString(maxLineIndex).size();

						if (diagnostic.range.start.lineIndex > 0) {
							writeLine(context, diagnostic.range.start.lineIndex - 1, maxLineIndexSize, true);
						}

						writeLine(context, diagnostic.range.start.lineIndex, maxLineIndexSize, false);

						const uint32 characterOffset = diagnostic.range.start.characterOffset;
						const uint32 characterCount = static_cast<uint32>(max(static_cast<sint32>(1), static_cast<sint32>(diagnostic.range.end.characterOffset) - static_cast<sint32>(characterOffset)));

						const view<char8> line = context.document.getLine(diagnostic.range.start.lineIndex);

						uint64 characterIndex = 0;

						logger << '\t';

						while (characterIndex < diagnostic.range.start.characterOffset) {
							const char8 character = line[characterIndex];

							characterIndex += 1;

							if (character == '\t') {
								logger << '\t';

								continue;
							}

							logger << ' ';
						}

						logger << ConsoleEscapeCode::FOREGROUND_COLOR_LIGHT_RED;

						characterIndex = 0;

						while (characterIndex < characterCount) {
							logger << '^';

							characterIndex += 1;
						}

						logger << ConsoleEscapeCode::RESET_FOREGROUND_COLOR << '\n';

						const uint32 nextLineIndex = diagnostic.range.start.lineIndex + 1;

						if (nextLineIndex < context.document.getLineCount()) {
							writeLine(context, nextLineIndex, maxLineIndexSize, true);
						}
					}

					logger << '\n';
				}

				return hasError;
			}

			void writeLine(const CompilationContext& context, uint32 lineIndex, uint64 maxLineIndex, bool dimmed) {
				if (dimmed) {
					logger << ConsoleEscapeCode::DIMMED;
				}

				logger.padLeft(maxLineIndex) << lineIndex + 1 << '\t' << context.document.getLine(lineIndex);

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
				const std::filesystem::path rootInputPath = pointer_cast<const char8*>(arguments[PROJECT_INPUT_PATH_ARGUMENT_INDEX]);
				const std::filesystem::path rootOutputPath = pointer_cast<const char8*>(arguments[PROJECT_OUTPUT_PATH_ARGUMENT_INDEX]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/";
				const std::filesystem::recursive_directory_iterator shaderDirectoryIterator(rootInputPath / "Asset/Shader");

				logger << '\n';

				sint32 code = 0;

				const uint64 start = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();

				for (const std::filesystem::directory_entry& directoryEntry : shaderDirectoryIterator) {
					if (directoryEntry.is_directory()) {
						continue;
					}

					// TODO: Extract asset information from a pre-built manifest file?
					if (directoryEntry.path().extension() != ".tsl") {
						continue;
					}

					// TODO: (debug-only)
					/* #if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
						const std::string command = (outputPath / "../tslc/tslc.exe").string();

						ShellExecuteA(0, "open", &command[0], &path[0], NULL, SW_SHOW);
					#endif */

					code = processFile(directoryEntry, assetOutputPath);
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