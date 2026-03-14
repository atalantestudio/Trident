// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct GUISubcomponent {
		mat3x3<float32> worldTransform;
		mat3x3<float32> textureTransform;
		vec4<float32> color;
		uint32 zIndex;
		uint32 textureArrayLayerIndex;
	};

	struct GUIGlyphSubcomponent {
		mat3x3<float32> world;
		mat3x3<float32> textureWorld;
		vec4<float32> color;
		uint32 textureArrayLayerIndex;
		float32 fontSize;
	};
}