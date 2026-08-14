// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/CommandEncoder.hpp"
#include "Engine/API/Graphics/RenderPass.hpp"
#include "Engine/Layer/Layer.hpp"

namespace Trident {
	class LayerGroup {
		public:
			explicit LayerGroup(range<uint32, uint32> layerRange);

			void setApplication(Application* application);

			virtual void initialize();

			virtual void render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const;

			virtual void resize();

		protected:
			view<ref<Layer>> layers;

		private:
			Application* application;
			range<uint32, uint32> layerRange;
	};
}