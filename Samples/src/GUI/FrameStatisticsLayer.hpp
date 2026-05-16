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

				Container({
					.size = {FIT, FIT},
					.padding = {8_px, 8_px, 8_px, 8_px},
					.layoutDirection = LayoutDirection::VERTICAL,
				}) {
					Container({
						.size = {FIT, FIT},
						.padding = {4_px, 4_px, 4_px, 4_px},
						.layoutDirection = LayoutDirection::VERTICAL,
						.color = 0x22222275,
					}) {
						PhysicalDeviceName();

						Container({
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
					.fontSize = 14.2_px,
				});
			}

			void GraphicsAPIName() {
				Text(serialize(getApplication()->getLayerCompositor()->adapter->getApi()), {
					.color = 0x777777ff,
					.font = fontRegular,
					.fontSize = 14.2_px,
				});
			}

			void FramesPerSecond() {
				const sequence<char8> string = Logger::format("[] FPS", framesPerSecond);

				Text(string, {
					.color = 0xffffffff,
					.font = fontRegular,
					.fontSize = 14.2_px,
				});
			}

		private:
			FontID fontRegular;

			float64 time = 0.0_s;
			uint64 framesPerSecond = 0;
	};
}