#pragma once

namespace ProjectA {
	struct SubpassDependency {
		public:
			static constexpr uint32 EXTERNAL_SUBPASS = max<uint32>();

		public:
			uint32 sourceSubpassIndex;
			uint32 destinationSubpassIndex;
	};
}