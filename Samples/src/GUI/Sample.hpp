// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUISampleLayer : public GUILayer {
		public:
			void registerAssets() override {
				fontBold = loadFontAsset("test-bold");
				fontRegular = loadFontAsset("test-regular");
				fontRegularItalic = loadFontAsset("test-regular-italic");

				image = loadImageAsset("Cameraman");
			}

			void render() override {
				GUIComponent({
					.size = {1280_px, 720_px},
					.padding = {12_px, 12_px},
					.layoutDirection = LayoutDirection::VERTICAL,
					.layoutSpacing = 16_px,
					.color = 0xffffffff,
				}) {
					Text("Lorem Ipsum", {
						.color = 0x000000ff,
						.font = fontBold,
						.fontSize = 28_px,
					});

					Text(
						"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
						"Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\n"
						"Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.\n"
						"Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
						{
							.color = 0x000000ff,
							.font = fontRegular,
							.fontSize = 20_px,
						}
					);

					GUIComponent({
						.size = {FIT, FIT},
						.layoutDirection = LayoutDirection::VERTICAL,
						.layoutSpacing = 4_px,
					}) {
						GUIComponent({
							.size = {256_px, 256_px},
							.color = 0xffffffff,
							.image = image,
						});

						Text("The Cameraman test image.", {
							.color = 0x757575ff,
							.font = fontRegularItalic,
							.fontSize = 16_px,
						});
					}
				}
			}

		private:
			FontID fontBold;
			FontID fontRegular;
			FontID fontRegularItalic;
			ImageID image;
	};
}