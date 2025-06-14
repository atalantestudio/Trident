#pragma once

namespace ProjectA {
	struct GUISubcomponent {
		svec2<float32> center;
		svec2<float32> halfInnerSize;
		mat3x3<float32> worldTransform;
		mat3x3<float32> textureTransform;
		uint32 layerIndex;
		vec4<float32> color;
		float32 radius;
	};

	struct GUIGlyphSubcomponent {
		mat3x3<float32> world;
		mat3x3<float32> textureWorld;
		uint32 layerIndex;
		vec4<float32> color;
	};
}