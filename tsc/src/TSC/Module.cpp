#include "Base/Base.hpp"

#include "CompilationOutput.hpp"
#include "Definition.hpp"
#include "Diagnostic/Diagnostic.hpp"
#include "Diagnostic/DiagnosticReference.hpp"
#include "DocumentPosition.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "SemanticAnalyzer.hpp"
#include "SemanticToken.hpp"

namespace ProjectA {
	CompilationOutput compile(const std::string& text) {
		CompilationOutput output{};
		output.text = text;

		Lexer lexer;
		lexer.tokenize(&output);

		Parser parser;
		parser.parse(&output);

		SemanticAnalyzer semanticAnalyzer(output);

		return output;
	}

	EMSCRIPTEN_BINDINGS(Module) {
		emscripten::class_<CompilationOutput>("CompilationOutput")
			.property("definitions", &CompilationOutput::definitions)
			.property("diagnostics", &CompilationOutput::diagnostics)
			.property("references", &CompilationOutput::references)
			.property("scopes", &CompilationOutput::scopes)
			.property("semanticTokens", &CompilationOutput::semanticTokens)
			.function("getIdentifier", &CompilationOutput::_emscripten_getIdentifier)
			.function("getSize", &CompilationOutput::_emscripten_getSize)
			.function("getAlignment", &CompilationOutput::_emscripten_getAlignment)
			.function("getPackedTypeString", &CompilationOutput::_emscripten_getPackedTypeString);

		emscripten::class_<Definition>("Definition")
			.property("identifierRange", &Definition::identifierRange)
			.property("end", &Definition::end)
			.property("identifierIndex", &Definition::identifierIndex)
			.property("memberCount", &Definition::memberCount)
			.function("getTypeIndex", &Definition::getTypeIndex)
			.function("getTemplateArgumentIndex", &Definition::getTemplateArgumentIndex)
			.function("getSemanticTokenTypeIndex", &Definition::getSemanticTokenTypeIndex)
			.function("getEvaluatedInitializer", &Definition::getEvaluatedInitializer);

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

		emscripten::class_<DocumentRange>("DocumentRange")
			.property("start", &DocumentRange::start)
			.property("end", &DocumentRange::end);

		emscripten::class_<Reference>("Reference")
			.property("definitionIndex", &Reference::definitionIndex)
			.property("identifierRange", &Reference::identifierRange);

		emscripten::class_<Scope>("Scope")
			.property("start", &Scope::start)
			.property("end", &Scope::end)
			.property("parentScopeIndex", &Scope::parentScopeIndex)
			.property("definitionIndexOffset", &Scope::definitionIndexOffset)
			.property("definitionCount", &Scope::definitionCount);

		emscripten::class_<SemanticToken>("SemanticToken")
			.property("range", &SemanticToken::range)
			.function("getTypeIndex", &SemanticToken::getTypeIndex);

		emscripten::function("compile", &compile);

		emscripten::register_vector<Definition>("DefinitionVector");
		emscripten::register_vector<Diagnostic>("DiagnosticVector");
		emscripten::register_vector<DiagnosticReference>("DiagnosticReferenceVector");
		emscripten::register_vector<Reference>("ReferenceVector");
		emscripten::register_vector<Scope>("ScopeVector");
		emscripten::register_vector<SemanticToken>("SemanticTokenVector");
	}
}