// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct SubpassDependency {
		public:
			static constexpr uint32 EXTERNAL_SUBPASS = max<uint32>();

		public:
			uint32 sourceSubpassIndex;
			uint32 destinationSubpassIndex;
	};
}