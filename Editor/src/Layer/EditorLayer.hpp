#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class EditorLayer : public GUILayer {
		public:
			void renderGUI() override {
				PROFILE_SCOPE(FUNC_SIG);

				const mat3x3<float32> world1 = mat3x3<float32>::scaling({100.0f, 40.0f}) * mat3x3<float32>::translation({0.0f, 20.0f});
				const mat3x3<float32> world2 = mat3x3<float32>::scaling({140.0f, 200.0f}) * mat3x3<float32>::translation({80.0f, 120.0f});
				const mat3x3<float32> world3 = mat3x3<float32>::scaling({80.0f, 80.0f}) * mat3x3<float32>::translation({280.0f, 10.0f});

				GUIQuad(world1);
				GUIQuad(world2, {1.0f, 0.2f, 0.0f, 1.0f});
				GUIQuad(world3, {0.1f, 0.2f, 0.3f, 1.0f});
			}
	};
}