#pragma once

#include "Engine/GUI/GUISubcomponent.hpp"

namespace ProjectA {
	struct GUIObjectDescriptor {
		vec2<float32> position;
		vec2<float32> size;
		uint32 colorIndex = 0;
		uint32 textureIndex = 0;
	};

	class GUIObject {
		public:
			GUIObject(const GUIObjectDescriptor& descriptor);

			const vec2<float32>& getPosition() const;

			uint32 getColorIndex() const;

			uint32 getTextureIndex() const;

			const chain<GUISubcomponent>& getSubcomponents() const;

		protected:
			vec2<float32> position;
			uint32 colorIndex;
			uint32 textureIndex;
			chain<GUISubcomponent> subcomponents;
	};
}