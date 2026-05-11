// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUISampleLayer : public GUILayer {
		public:
			void registerAssets() override {
				fontBold = loadFontAsset("test-bold");
				fontRegular = loadFontAsset("test-regular");

				image = loadImageAsset("Cameraman");
			}

			void render() override {
				const Extent2D extent = getApplication()->getWindow()->getExtent();

				Container({
					.size = {static_cast<float32>(extent.width - 16_px), static_cast<float32>(extent.height - 16_px)},
					.padding = {8_px, 8_px},
					.layoutDirection = LayoutDirection::VERTICAL,
					.layoutSpacing = 16_px,
					.color = 0xfefefeff,
				}) {
					Text("Lorem Ipsum", {
						.color = 0x0a0a0aff,
						.font = fontBold,
						.fontSize = 26.4_px,
					});

					Text(
						"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
						"Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\n"
						"Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.\n"
						"Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
						{
							.color = 0x0a0a0aff,
							.font = fontRegular,
							.fontSize = 17.6_px,
						}
					);

					Container({
						.size = {FIT, FIT},
						.layoutDirection = LayoutDirection::VERTICAL,
						.layoutSpacing = 4_px,
					}) {
						Container({
							.size = {256_px, 256_px},
							.color = 0xffffffff,
							.image = image,
						});

						Text("The Cameraman test image.", {
							.color = 0x757575ff,
							.font = fontRegular,
							.fontSize = 14_px,
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