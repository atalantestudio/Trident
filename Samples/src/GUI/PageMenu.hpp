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

				Container({
					.size = {static_cast<float32>(extent.width - 24_px), static_cast<float32>(extent.height - 16_px)},
					.padding = {8_px, 8_px, 8_px, 8_px},
					.layoutDirection = HORIZONTAL,
					.layoutSpacing = 8_px,
					.color = 0x000000ff,
				}) {
					// Option list
					Container({
						.size = {FIT, EXPAND},
						.layoutDirection = VERTICAL,
						.layoutSpacing = -1_px,
					}) {
						uint32 listItemIndex = 0;
						sequence<char8> text;

						while (listItemIndex < LIST_ITEM_COUNT) {
							text = Logger::format("Option []", listItemIndex);

							Text(text, {
								.color = 0xa5a5a5ff,
								.font = fontBold,
								.fontSize = 22_px,
							});

							text = Logger::format("Description for option []", listItemIndex);

							Text(text, {
								.color = 0x828282ff,
								.font = fontRegular,
								.fontSize = 18_px,
							});

							listItemIndex += 1;
						}
					}

					// Menu
					Container({
						.size = {EXPAND, EXPAND},
						.padding = {1_px, 1_px, 1_px, 1_px},
						.color = 0x3a3a3aff,
					}) {
						Container({
							.size = {EXPAND, EXPAND},
							.padding = {8_px, 8_px, 8_px, 8_px},
							.layoutDirection = VERTICAL,
							.color = 0x0a0a0aff,
						}) {
							Text("Option Menu", {
								.color = 0xc2c2c2ff,
								.font = fontBold,
								.fontSize = 28_px,
							});

							Text("Configure the option properties there.", {
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