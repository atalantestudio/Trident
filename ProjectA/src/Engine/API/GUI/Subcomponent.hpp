#pragma once

namespace ProjectA {
	struct GUISubcomponent {
		svec2<float32> minBounds;
		svec2<float32> maxBounds;
		mat3x3<float32> worldTransform;
		mat3x3<float32> textureTransform;
		vec4<float32> color;
		vec4<float32> outlineColor;
		uint32 layerIndex;
		float32 outlineSize;
		float32 borderRadius;
	};

	struct GUIGlyphSubcomponent {
		mat3x3<float32> world;
		mat3x3<float32> textureWorld;
		uint32 layerIndex;
		vec4<float32> color;
	};
}