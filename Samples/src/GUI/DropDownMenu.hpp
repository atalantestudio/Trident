// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUIDropDownMenuLayer : public GUILayer {
		private:
			static constexpr uint32 ITEM_COUNT = 5;

			inline static constexpr const view<char8> ITEM_LABELS[ITEM_COUNT] = {
				"Copy",
				"Paste",
				"Delete",
				"Layer",
				"Comment",
			};

		public:
			void registerAssets() override {
				font = loadFontAsset("test-regular");
			}

			void render() override {
				Container({
					.size = {FIT, FIT},
					.padding = {8_px, 8_px},
					.layoutDirection = VERTICAL,
					.layoutSpacing = 8_px,
					.color = 0xa675a2ff,
					.debugName = "menu",
				}) {
					uint32 itemIndex = 0;

					while (itemIndex < ITEM_COUNT) {
						const view<char8> label = ITEM_LABELS[itemIndex];

						Container({
							.size = {EXPAND, 40_px},
							.minimumSize = {0_px, 40_px},
							.padding = {8_px, 8_px},
							.anchorPoint = AnchorPoint::LEFT | AnchorPoint::VCENTER,
							.layoutSpacing = 8_px,
							.color = 0xbd95bcff,
							.debugName = "item",
						}) {
							Container({
								.size = {EXPAND, FIT},
								.color = 0x00000000,
								.debugName = "labelContainer",
							}) {
								Text(label, {
									.color = 0xffffffff,
									.font = font,
									.fontSize = 32,
								});
							}

							Container({
								.size = {32_px, 32_px},
								.color = 0xff9800ff,
								.debugName = "itemImage",
							});
						}

						itemIndex += 1;
					}
				}
			}

		private:
			FontID font;
	};
}