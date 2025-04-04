#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class DemoLayer : public Layer {
		private:
			ComputeShaderDemoRenderer renderer;

		public:
			DemoLayer(const TTFFile& ttfFile);

			virtual ~DemoLayer() = default;

			void update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;
	};
}