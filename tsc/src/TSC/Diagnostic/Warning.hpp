#pragma once

#include "../CompilationOutput.hpp"
#include "Diagnostic.hpp"
#include "DiagnosticCode.hpp"
#include "DiagnosticSeverity.hpp"
#include "generateDiagnostic.hpp"
#include "../Token.hpp"

namespace ProjectA {
	template<>
	struct generateDiagnostic<DiagnosticCode::EMPTY_STRUCTURE> {
		explicit generateDiagnostic(CompilationOutput& output, const Token& identifierToken) {
			Diagnostic diagnostic{};
			diagnostic.start = identifierToken.start;
			diagnostic.end = identifierToken.getEnd();
			diagnostic.message = "Structure is empty.";
			diagnostic.severity = DiagnosticSeverity::WARNING;
			diagnostic.code = DiagnosticCode::EMPTY_STRUCTURE;

			output.diagnostics.emplace_back(diagnostic);
		}
	};
}