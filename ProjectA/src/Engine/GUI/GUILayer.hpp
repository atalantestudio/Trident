#pragma once

#include "Engine/GUI/GUIRenderer.hpp"
#include "Engine/GUI/Object/GUIObject.hpp"
#include "Engine/Layer/Layer.hpp"

namespace ProjectA {
	class GUILayer : public Layer {
		public:
			GUILayer();

			void append(const GUIObject& object);

		protected:
			GUIRenderer renderer;
	};
}