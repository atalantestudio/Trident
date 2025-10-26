#include "Base/Base.hpp"

#include "CompilationOutput.hpp"
#include "Definition.hpp"
#include "Diagnostic/Diagnostic.hpp"
#include "Diagnostic/DiagnosticReference.hpp"
#include "DocumentPosition.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "SemanticToken.hpp"
#include "Token.hpp"

namespace ProjectA {
	CompilationOutput compile(const std::string& text) {
		CompilationOutput output{};
		output.text = text;

		Lexer lexer;
		lexer.tokenize(&output);

		Parser parser;
		parser.parse(&output);

		return output;
	}

	EMSCRIPTEN_BINDINGS(Module) {
		emscripten::class_<CompilationOutput>("CompilationOutput")
			// .property("completionItems", &CompilationOutput::completionItems)
			.property("definitions", &CompilationOutput::definitions)
			.property("diagnostics", &CompilationOutput::diagnostics)
			.property("scopes", &CompilationOutput::scopes)
			.property("semanticTokens", &CompilationOutput::semanticTokens)
			.property("tokens", &CompilationOutput::tokens);

		emscripten::class_<CompletionItem>("CompletionItem")
			.property("label", &CompletionItem::label)
			.property("end", &CompletionItem::end)
			.function("getKindIndex", &CompletionItem::getKindIndex);

		emscripten::class_<Definition>("Definition")
			.property("end", &Definition::end)
			.property("semanticTokenModifiers", &Definition::semanticTokenModifierFlag)
			.property("identifier", &Definition::identifier)
			.function("getSemanticTokenTypeIndex", &Definition::getSemanticTokenTypeIndex);

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

		emscripten::class_<Scope>("Scope")
			.property("start", &Scope::start)
			.property("end", &Scope::end)
			.property("parentScopeIndex", &Scope::parentScopeIndex)
			.property("definitionIndexOffset", &Scope::definitionIndexOffset)
			.property("definitionCount", &Scope::definitionCount);

		emscripten::class_<SemanticToken>("SemanticToken")
			.property("start", &SemanticToken::start)
			.property("length", &SemanticToken::characterCount)
			.property("modifiers", &SemanticToken::modifiers)
			.function("getTypeIndex", &SemanticToken::getTypeIndex);

		emscripten::class_<Token>("Token")
			.property("start", &Token::start)
			.property("length", &Token::characterCount)
			.property("type", &Token::type);

		emscripten::function("compile", &compile);

		emscripten::register_vector<CompletionItem>("CompletionItemVector");
		emscripten::register_vector<Definition>("DefinitionVector");
		emscripten::register_vector<Diagnostic>("DiagnosticVector");
		emscripten::register_vector<DiagnosticReference>("DiagnosticReferenceVector");
		emscripten::register_vector<Scope>("ScopeVector");
		emscripten::register_vector<SemanticToken>("SemanticTokenVector");
		emscripten::register_vector<Token>("TokenVector");
	}
}