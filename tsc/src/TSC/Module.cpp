#include "Base/Base.hpp"

#include "AST.hpp"
#include "CompilationResult.hpp"
#include "Diagnostic.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "SemanticAnalyzer.hpp"
#include "SemanticToken.hpp"
#include "Token.hpp"

namespace ProjectA {
	CompilationResult compile(const std::string& text) {
		CompilationResult compilationResult{};

		Lexer lexer;
		lexer.tokenize(text, compilationResult);

		Parser parser;
		const AST ast = parser.parse(compilationResult);

		// SemanticAnalyzer semanticAnalyzer;
		// semanticAnalyzer.analyze(compilationResult, ast);

		return compilationResult;
	}

	EMSCRIPTEN_BINDINGS(Module) {
		emscripten::class_<CompilationResult>("CompilationResult")
			.function("getTokens", &CompilationResult::getTokens)
			.function("getSemanticTokens", &CompilationResult::getSemanticTokens)
			.function("getDiagnostics", &CompilationResult::getDiagnostics);
		emscripten::class_<Diagnostic>("Diagnostic")
			.function("getSeverityIndex", &Diagnostic::getSeverityIndex)
			.function("getMessage", &Diagnostic::getMessage)
			.function("getStartLineIndex", &Diagnostic::getStartLineIndex)
			.function("getStartCharacterOffset", &Diagnostic::getStartCharacterOffset)
			.function("getEndLineIndex", &Diagnostic::getEndLineIndex)
			.function("getEndCharacterOffset", &Diagnostic::getEndCharacterOffset);
		emscripten::class_<SemanticToken>("SemanticToken")
			.function("getTypeIndex", &SemanticToken::getTypeIndex)
			.function("getModifiers", &SemanticToken::getModifiers)
			.function("getLine", &SemanticToken::getLineIndex)
			.function("getCharacter", &SemanticToken::getCharacterOffset)
			.function("getLength", &SemanticToken::getCharacterCount);
		emscripten::class_<Token>("Token")
			.function("getTypeIndex", &Token::getTypeIndex)
			.function("getLine", &Token::getLineIndex)
			.function("getCharacter", &Token::getCharacterOffset)
			.function("getLength", &Token::getCharacterCount)
			.function("getValue", &Token::getValue);

		emscripten::function("compile", &compile);

		emscripten::register_vector<Diagnostic>("DiagnosticVector");
		emscripten::register_vector<SemanticToken>("SemanticTokenVector");
		emscripten::register_vector<Token>("TokenVector");
	}
}