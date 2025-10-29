#pragma once

namespace ProjectA {
	enum class Annotation : uint32 {
		BINDING,
		CONSTANT,
		INSTANCE_INDEX,
		LOCATION,
		POSITION,
		SET,
		VERTEX_INDEX,

		__COUNT,
	};

	AS_FLAG(Annotation);

	static constexpr uint32 ANNOTATION_TYPE_NONE =
		BIT(static_cast<uint32>(Annotation::INSTANCE_INDEX)) |
		BIT(static_cast<uint32>(Annotation::POSITION)) |
		BIT(static_cast<uint32>(Annotation::VERTEX_INDEX));
	static constexpr uint32 ANNOTATION_TYPE_UINT32 =
		BIT(static_cast<uint32>(Annotation::BINDING)) |
		BIT(static_cast<uint32>(Annotation::CONSTANT)) |
		BIT(static_cast<uint32>(Annotation::LOCATION)) |
		BIT(static_cast<uint32>(Annotation::SET));

	static constexpr const char8* ANNOTATION_STRINGS[static_cast<uint32>(Annotation::__COUNT)] = {
		"binding",
		"constant",
		"instanceIndex",
		"location",
		"position",
		"set",
		"vertexIndex",
	};
}