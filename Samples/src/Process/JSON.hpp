// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Process/Process.hpp"

namespace Trident {
	class ProcessJSONProcess final : public Process {
		public:
			sint32 main(view<void*> arguments) override {
				using tsl::Object;
				using tsl::FileFormat::JSON;

				Object<JSON> textDocumentSync;
				textDocumentSync["openClose"] = true;
				textDocumentSync["change"] = 1;

				sequence<Object<JSON>> triggerCharacters(2);
				triggerCharacters[0] = ".";
				triggerCharacters[1] = "[";

				Object<JSON> completionItem;
				completionItem["labelDetailsSupport"] = true;

				Object<JSON> completionProvider;
				completionProvider["triggerCharacters"] = triggerCharacters;

				completionProvider["completionItem"] = completionItem;

				Object<JSON> signatureHelpProvider;

				{
					sequence<Object<JSON>> triggerCharacters(1);
					triggerCharacters[0] = "(";

					sequence<Object<JSON>> retriggerCharacters(1);
					retriggerCharacters[0] = ",";

					signatureHelpProvider["triggerCharacters"] = triggerCharacters;
					signatureHelpProvider["retriggerCharacters"] = retriggerCharacters;
				}

				Object<JSON> legend;
				legend["tokenTypes"] = {
					"struct",
					"parameter",
					"variable",
					"property",
					"enumMember",
					"function",
					"keyword",
					"comment",
					"string",
					"number",
					"operator",
				};
				legend["tokenModifiers"] = {
					"definition",
					"readonly",
					"modification",
					"defaultLibrary",
				};

				Object<JSON> full;
				full["delta"] = false;

				Object<JSON> semanticTokensProvider;
				semanticTokensProvider["legend"] = legend;
				semanticTokensProvider["range"] = false;
				semanticTokensProvider["full"] = full;

				Object<JSON> capabilities;
				capabilities["textDocumentSync"] = textDocumentSync;
				capabilities["completionProvider"] = completionProvider;
				capabilities["hoverProvider"] = true;
				capabilities["signatureHelpProvider"] = signatureHelpProvider;
				capabilities["definitionProvider"] = true;
				capabilities["typeDefinitionProvider"] = true;
				capabilities["semanticTokensProvider"] = semanticTokensProvider;

				Object<JSON> result;
				result["capabilities"] = capabilities;

				std::cout << result.dump(false) << '\n';

				return 0;
			}
	};
}