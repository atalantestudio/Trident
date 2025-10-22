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
			.property("tokens", &CompilationResult::tokens)
			.property("semanticTokens", &CompilationResult::semanticTokens)
			.property("diagnostics", &CompilationResult::diagnostics);

		emscripten::class_<Diagnostic>("Diagnostic")
			.property("start", &Diagnostic::start)
			.property("end", &Diagnostic::end)
			.property("message", &Diagnostic::message)
			.property("references", &Diagnostic::references)
			.function("getSeverityIndex", &Diagnostic::getSeverityIndex)
			.function("getCodeIndex", &Diagnostic::getCodeIndex);

		emscripten::class_<DiagnosticReference>("DiagnosticReference")
			.property("start", &DiagnosticReference::start)
			.property("end", &DiagnosticReference::end)
			.property("message", &DiagnosticReference::message);

		emscripten::class_<DocumentPosition>("DocumentPosition")
			.property("line", &DocumentPosition::lineIndex)
			.property("character", &DocumentPosition::characterOffset);

		emscripten::class_<SemanticToken>("SemanticToken")
			.property("start", &SemanticToken::start)
			.property("length", &SemanticToken::characterCount)
			.property("modifiers", &SemanticToken::modifiers)
			.function("getTypeIndex", &SemanticToken::getTypeIndex);

		emscripten::class_<Token>("Token")
			.property("start", &Token::start)
			.property("length", &Token::characterCount)
			.property("value", &Token::value)
			.function("getTypeIndex", &Token::getTypeIndex);

		emscripten::function("compile", &compile);

		emscripten::register_vector<Diagnostic>("DiagnosticVector");
		emscripten::register_vector<DiagnosticReference>("DiagnosticReferenceVector");
		emscripten::register_vector<SemanticToken>("SemanticTokenVector");
		emscripten::register_vector<Token>("TokenVector");
	}
}