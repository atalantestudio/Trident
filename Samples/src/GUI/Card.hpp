// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUICardLayer : public GUILayer {
		public:
			void registerAssets() override {
				fontBold = loadFontAsset("test-bold");
				fontRegular = loadFontAsset("test-regular");
			}

			void render() override {
				const sint32* size = getApplication()->getWindow()->getSize();

				GUIComponent({
					.size = {static_cast<float32>(size[0]), static_cast<float32>(size[1])},
					.color = 0xffffffff,
				}) {
					GUIComponent({
						.size = {FIT, FIT},
						.padding = {8_px, 8_px},
						.layoutDirection = VERTICAL,
						.layoutSpacing = 8_px,
						.color = 0xeeeeeeff,
					}) {
						GUIComponent({
							.size = {200_px, 200_px},
							.color = 0xccccccff,
						});

						GUIComponent({
							.size = {EXPAND, FIT},
							.layoutDirection = VERTICAL,
							.layoutSpacing = 2_px,
						}) {
							GUIText("Card title", {
								.color = 0x000000ff,
								.font = fontBold,
								.fontSize = 22.0_px,
							});

							GUIText("Card description", {
								.color = 0x00000080,
								.font = fontRegular,
								.fontSize = 18.0_px,
							});
						}
					}
				}
			}

		private:
			FontID fontBold;
			FontID fontRegular;
	};
}