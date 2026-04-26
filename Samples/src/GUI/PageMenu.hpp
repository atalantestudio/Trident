// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUIPageMenuLayer : public GUILayer {
		private:
			static constexpr uint32 LIST_ITEM_COUNT = 5;

		public:
			void registerAssets() override {
				fontBold = loadFontAsset("test-bold");
				fontRegular = loadFontAsset("test-regular");
			}

			void render() override {
				const Extent2D extent = getApplication()->getWindow()->getExtent();

				GUIComponent({
					.size = {static_cast<float32>(extent.width - 24_px), static_cast<float32>(extent.height - 16_px)},
					.padding = {8_px, 8_px},
					.layoutDirection = HORIZONTAL,
					.layoutSpacing = 8_px,
					.color = 0x000000ff,
				}) {
					// Option list
					GUIComponent({
						.size = {FIT, EXPAND},
						.layoutDirection = VERTICAL,
						.layoutSpacing = -1_px,
					}) {
						uint32 listItemIndex = 0;

						while (listItemIndex < LIST_ITEM_COUNT) {
							GUIText(Logger::format("Option []", listItemIndex), {
								.color = 0xa5a5a5ff,
								.font = fontBold,
								.fontSize = 22_px,
							});

							GUIText(Logger::format("Description for option []", listItemIndex), {
								.color = 0x828282ff,
								.font = fontRegular,
								.fontSize = 18_px,
							});

							listItemIndex += 1;
						}
					}

					// Menu
					GUIComponent({
						.size = {EXPAND, EXPAND},
						.padding = {1_px, 1_px},
						.color = 0x3a3a3aff,
					}) {
						GUIComponent({
							.size = {EXPAND, EXPAND},
							.padding = {8_px, 8_px},
							.layoutDirection = VERTICAL,
							.color = 0x0a0a0aff,
						}) {
							GUIText("Option Menu", {
								.color = 0xc2c2c2ff,
								.font = fontBold,
								.fontSize = 28_px,
							});

							GUIText("Configure the option properties there.", {
								.color = 0x828282ff,
								.font = fontRegular,
								.fontSize = 18_px,
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