#pragma once

namespace ProjectA {
	struct Definition {
		const char8* identifier;
		uint32 identifierSize;
		uint32 structureStatementIndex = static_cast<uint32>(~0);
	};
}