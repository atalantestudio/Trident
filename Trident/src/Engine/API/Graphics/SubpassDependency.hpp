// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct SubpassDependency {
		static constexpr uint32 EXTERNAL_SUBPASS = max<uint32>();

		uint32 sourceSubpassIndex;
		uint32 destinationSubpassIndex;
	};
}