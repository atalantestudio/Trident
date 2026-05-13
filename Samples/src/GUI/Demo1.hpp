// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUIDemo1Layer : public GUILayer {
		public:
			void render() override {
				Container({
					.size = {800_px, FIT},
					.padding = {32_px, 32_px, 32_px, 32_px},
					.layoutDirection = LayoutDirection::HORIZONTAL,
					.layoutSpacing = 32_px,
					.color = 0x0a6e89ff,
					.debugName = "blue",
				}) {
					Container({
						.size = {300_px, EXPAND},
						.color = 0xfb938fff,
						.debugName = "pink",
					});
					Container({
						.size = {EXPAND, 200_px},
						.color = 0xfed84dff,
						.debugName = "yellow",
					});
					Container({
						.size = {EXPAND, 300_px},
						.color = 0x5ecbe4ff,
						.debugName = "lightBlue",
					});
				}
			}
	};
}