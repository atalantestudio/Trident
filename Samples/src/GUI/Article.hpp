// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "GUI/Theme/DefaultTheme.hpp"

namespace Trident {
	class GUIArticleLayer : public GUIDefaultTheme {
		public:
			void registerAssets() override {
				GUIDefaultTheme::registerAssets();

				updateThemeColors(false);

				const ref<ImageAsset> imageAsset = getApplication()->getAssetLibrary()->getAsset<ImageAsset>("Cameraman");

				image = registerImage(imageAsset);
			}

			void render() override {
				Container({
					.size = {EXPAND, FIT},
					.anchorPoint = AnchorPoint::HCENTER | AnchorPoint::TOP,
				}) {
					Container({
						.size = {992_px, FIT},
						.padding = {16_px, 16_px, 0_px, 0_px},
						.layoutDirection = LayoutDirection::VERTICAL,
						.layoutSpacing = 16_px,
					}) {
						Container({
							.size = {0_px, 0_px},
						});

						Text("Lorem Ipsum", {
							.color = textColor,
							.font = fontBold,
							.fontSize = 26.4_px,
						});

						Text(
							"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
							"Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\n"
							"Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.\n"
							"Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
							{
								.color = textColor,
								.font = fontRegular,
								.fontSize = 16_px,
							}
						);

						Quote("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt...");

						Text(
							"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
							"Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.",
							{
								.color = textColor,
								.font = fontRegular,
								.fontSize = 16_px,
							}
						);

						InfoNote("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt...");
						ErrorNote("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt...");
						WarningNote("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt...");

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
								.color = secondaryColor,
								.font = fontRegular,
								.fontSize = 14_px,
							});
						}
					}
				}
			}

		private:
			ImageID image;
	};
}