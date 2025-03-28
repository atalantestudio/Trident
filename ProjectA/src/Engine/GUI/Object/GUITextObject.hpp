#pragma once

#include "Engine/GUI/Font/Font.hpp"
#include "Engine/GUI/Object/GUIObject.hpp"

namespace ProjectA {
	class GUITextObject : public GUIObject {
		public:
			GUITextObject(const std::string& text);

			void compute(ECS& ecs, const vec2<float32>& parentSize, const vec2<float32>& parentMargin, chain<vec4<float32>>& colors, const ref<Font>& font) override;

		private:
			std::string text;
	};
}