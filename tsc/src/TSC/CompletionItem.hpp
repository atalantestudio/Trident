#pragma once

#include "CompletionItemKind.hpp"
#include "DocumentPosition.hpp"

namespace ProjectA {
	struct CompletionItem {
		uint32 getKindIndex() const {
			return static_cast<uint32>(kind);
		}

		std::string label;
		CompletionItemKind kind;
		DocumentPosition end;
	};
}