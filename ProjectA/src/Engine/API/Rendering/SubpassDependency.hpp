#pragma once

namespace ProjectA {
	struct SubpassDependency {
		uint32 sourceSubpassIndex;
		uint32 destinationSubpassIndex;

		static constexpr uint32 EXTERNAL_SUBPASS = max<uint32>();
	};
}