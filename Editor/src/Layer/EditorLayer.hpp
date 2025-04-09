#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class EditorLayer : public GUILayer {
		public:
			void update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;
	};
}