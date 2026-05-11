// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Theme/DefaultTheme.hpp"

namespace Trident {
	class GUIPropertyPanelLayer : public GUIDefaultTheme {
		public:
			void registerAssets() override {
				GUIDefaultTheme::registerAssets();
			}

			void render() override {
				const Extent2D extent = getApplication()->getWindow()->getExtent();

				Container({
					.size = {static_cast<float32>(extent.width - 16_px), static_cast<float32>(extent.height - 16_px)},
					.padding = {8_px, 8_px},
					.layoutDirection = LayoutDirection::VERTICAL,
					.layoutSpacing = 8_px,
					.color = PRIMARY_COLOR,
				}) {
					Button("Light");
				}
			}
	};
}