// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUIDemo2Layer : public GUILayer {
		public:
			void render() override {
				const ContainerStyle yellow = {
					.size = {64_px, 64_px},
					.color = 0xfed84dff,
					.debugName = "yellow",
				};

				Container({
					.size = {FIT, FIT},
					.padding = {8_px, 8_px},
					.layoutDirection = LayoutDirection::VERTICAL,
					.layoutSpacing = 8_px,
					.color = 0x0a6e89ff,
					.debugName = "blue",
				}) {
					index i = 0;

					while (i < 3) {
						Container({
							.size = {FIT, FIT},
							.padding = {8_px, 8_px},
							.color = 0xfb938fff,
							.debugName = "pink",
						}) {
							Container({
								.size = {FIT, FIT},
								.padding = {8_px, 8_px},
								.layoutDirection = LayoutDirection::HORIZONTAL,
								.layoutSpacing = 8_px,
								.color = 0x5ecbe4ff,
								.debugName = "lightBlue",
							}) {
								Container(yellow);
								Container(yellow);
							}
						}

						i += 1;
					}
				}
			}
	};
}