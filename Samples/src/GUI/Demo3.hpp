// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUIDemo3Layer : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFontAsset("test-regular");
			}

			void render() override {
				Container({
					.size = {1000_px, 500_px},
					.padding = {32_px, 32_px, 32_px, 32_px},
					.anchorPoint = AnchorPoint::LEFT | AnchorPoint::VCENTER,
					.layoutSpacing = 32_px,
					.color = 0x0a6e89ff,
					.debugName = "blue",
				}) {
					Text("One Two Three Four Five", {
						.color = 0xf99394ff,
						.font = font,
						.fontSize = 64_px,
						.debugName = "text1",
					});

					Container({
						.size = {300_px, 300_px},
						.color = 0xfed84dff,
						.debugName = "yellow",
					});

					Container({
						.size = {FIT, FIT},
						.padding = {32_px, 32_px, 32_px, 32_px},
						.color = 0x5ecbe4ff,
						.debugName = "lightBlue",
					}) {
						Text("Five Six Seven Eight Nine Ten", {
							.color = 0xb45bd0ff,
							.font = font,
							.fontSize = 64_px,
							.debugName = "text2",
						});
					}
				}
			}

		private:
			FontID font;
	};
}