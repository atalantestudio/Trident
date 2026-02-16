// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct GUISubcomponent {
		// svec2<float32> minBounds;
		// svec2<float32> maxBounds;
		mat3x3<float32> worldTransform;
		mat3x3<float32> textureTransform;
		vec4<float32> color;
		// vec4<float32> outlineColor;
		uint32 textureArrayLayerIndex;
		// float32 outlineSize;
		// float32 borderRadius;
	};

	struct GUIGlyphSubcomponent {
		mat3x3<float32> world;
		mat3x3<float32> textureWorld;
		vec4<float32> color;
		uint32 textureArrayLayerIndex;
		float32 fontSize;
	};
}