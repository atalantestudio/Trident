// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class FrameStatisticsLayer : public GUILayer {
		public:
			void registerAssets() override {
				fontRegular = loadFontAsset("test-regular");
			}

			void render() override {
				time += getApplication()->getCurrentFrameTime();

				if (time >= 1.0_s) {
					time = 0.0_s;
					framesPerSecond = static_cast<uint64>(round(getApplication()->getFramesPerSecond()));
				}

				GUIComponent({
					.size = {FIT, FIT},
					.padding = {8.0_px, 8.0_px},
					.layoutDirection = LayoutDirection::VERTICAL,
				}) {
					GUIComponent({
						.size = {FIT, FIT},
						.padding = {4.0_px, 4.0_px},
						.layoutDirection = LayoutDirection::VERTICAL,
						.color = 0x22222275,
					}) {
						PhysicalDeviceName();

						GUIComponent({
							.size = {FIT, FIT},
							.layoutSpacing = 16_px,
						}) {
							GraphicsAPIName();
							FramesPerSecond();
						}
					}
				}
			}

		private:
			void PhysicalDeviceName() {
				Text(getApplication()->getLayerCompositor()->physicalDevice->getName(), {
					.color = 0x76b900ff,
					.font = fontRegular,
					.fontSize = 18_px,
				});
			}

			void GraphicsAPIName() {
				Text(toString(getApplication()->getLayerCompositor()->adapter->getApi()), {
					.color = 0x777777ff,
					.font = fontRegular,
					.fontSize = 18_px,
				});
			}

			void FramesPerSecond() {
				const sequence<char8> string = Logger::format("[] FPS", framesPerSecond);

				Text(string, {
					.color = 0xffffffff,
					.font = fontRegular,
					.fontSize = 18_px,
				});
			}

		private:
			FontID fontRegular;

			float64 time = 0.0_s;
			uint64 framesPerSecond = 0;
	};
}