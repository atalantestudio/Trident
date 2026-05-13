// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUICardLayer : public GUILayer {
		public:
			void registerAssets() override {
				fontBold = loadFontAsset("test-bold");
				fontRegular = loadFontAsset("test-regular");
				image = loadImageAsset("Cameraman");
			}

			void render() override {
				// TODO: Fix height overflow (also happening in Demo1, Demo2 and DropDownMenu).
				Container({
					.size = {FIT, FIT},
					.padding = {8_px, 8_px, 8_px, 8_px},
					.layoutDirection = LayoutDirection::VERTICAL,
					.layoutSpacing = 8_px,
					.color = 0xeeeeeeff,
				}) {
					Container({
						.size = {200_px, 200_px},
						.color = 0xffffffff,
						.image = image,
					});

					Container({
						.size = {EXPAND, FIT},
						.layoutDirection = LayoutDirection::VERTICAL,
						.layoutSpacing = 2_px,
					}) {
						Text("Title", {
							.color = 0x000000ff,
							.font = fontBold,
							.fontSize = 22_px,
						});

						Text("Description", {
							.color = 0x00000080,
							.font = fontRegular,
							.fontSize = 18_px,
						});
					}
				}
			}

		private:
			FontID fontBold;
			FontID fontRegular;
			ImageID image;
	};
}