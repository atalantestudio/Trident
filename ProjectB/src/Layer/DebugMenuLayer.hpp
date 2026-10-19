// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Engine/API/GUI/GUILayer.hpp"

namespace ProjectB {
	class DebugMenuLayer : public Trident::GUILayer {
		public:
			void registerAssets() override {
				clearColor = 0x00000000;
			}

		public:
			void render() final override {
				using namespace Trident;

				Container({
					.size = {512_px, 512_px},
					.color = 0x00000050,
				}) {
					// TODO(project-b/layer/debug-menu)
				}
			}
	};
}