// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Engine/API/GUI/GUILayer.hpp"

#include "UI/DefaultTheme.hpp"

namespace Trident {
	class DebugMenuLayer : public UIDefaultTheme {
		public:
			void registerAssets() override {
				UIDefaultTheme::registerAssets();

				updateThemeColors(true);
			}

		public:
			void render() final override {
				Container({
					.size = {256_px, FIT},
					.padding = {8_px, 8_px, 8_px, 8_px},
					.layoutDirection = LayoutDirection::VERTICAL,
					.layoutSpacing = 8_px,
				}) {
					// TODO(project-b/layer/debug-menu)
				}
			}
	};
}