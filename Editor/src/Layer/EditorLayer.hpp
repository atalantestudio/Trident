#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class EditorLayer : public GUILayer {
		public:
			void GUI() override {
				ENGINE_ASSERT(false, "EDITOR");
			}

		private:
			static inline const vec4<float32> GRAY = {0.122f, 0.122f, 0.122f, 1.0f};
			static inline const vec4<float32> LIGHT_GRAY = {0.196f, 0.204f, 0.196f, 1.0f};
	};
}