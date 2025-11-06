#pragma once

namespace ProjectA {
	struct UserDefinedType {
		uint32 identifierIndex;
		uint32 memberCount;
		uint32 size;
		uint32 alignment;

		// Whether this type is used as an entry-point IO structure.
		// TODO: Use a ShaderStage mask instead.
		bool entryPoint;
	};
}