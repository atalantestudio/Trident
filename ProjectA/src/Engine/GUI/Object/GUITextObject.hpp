#pragma once

#include "Engine/GUI/Font/Font.hpp"
#include "Engine/GUI/Object/GUIObject.hpp"

namespace ProjectA {
	struct GUITextObjectDescriptor {
		ref<Font> font;
	};

	class GUITextObject : public GUIObject {
		public:
			GUITextObject(const std::string& text, const GUITextObjectDescriptor& descriptor);
	};
}