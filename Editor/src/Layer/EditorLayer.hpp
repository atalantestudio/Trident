#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class EditorLayer : public GUILayer {
		public:
			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;
	};
}