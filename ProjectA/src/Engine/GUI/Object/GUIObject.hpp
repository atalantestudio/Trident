#pragma once

#include "Engine/GUI/GUISubcomponent.hpp"

namespace ProjectA {
	class GUIObject {
		public:
			GUIObject();

			const vec2<float32>& getPosition() const;

			void setPosition(const vec2<float32>& position);

			uint32 getColorIndex() const;

			void setColorIndex(uint32 colorIndex);

			uint32 getTextureIndex() const;

			void setTextureIndex(uint32 textureIndex);

			const chain<GUISubcomponent>& getSubcomponents() const;

		protected:
			vec2<float32> position = {0.0f, 0.0f};
			uint32 colorIndex = 0;
			uint32 textureIndex = 0;
			chain<GUISubcomponent> subcomponents;
	};
}