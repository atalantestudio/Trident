#pragma once

#include "CompletionItemKind.hpp"
#include "DocumentPosition.hpp"

namespace ProjectA {
	struct CompletionItem {
		std::string getLabel() const {
			return std::string(label, labelSize);
		}

		uint32 getKindIndex() const {
			return static_cast<uint32>(kind);
		}

		const char8* label;
		uint32 labelSize;
		CompletionItemKind kind;
		DocumentPosition end;
	};
}