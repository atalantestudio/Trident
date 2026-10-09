// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUILaunchLayer : public GUILayer {
		public:
			void registerAssets() override {
				icon = loadImageAsset("icon-transparent");
			}

			void render() override {
				const Extent2D extent = getApplication()->getWindow()->getExtent();

				Container({
					.size = {static_cast<float32>(extent.x), static_cast<float32>(extent.y)},
					.anchorPoint = AnchorPoint::HCENTER | AnchorPoint::VCENTER,
				}) {
					Container({
						.size = {256_px, 256_px},
						.color = 0xffffffff,
						.image = icon,
					});
				}
			}

		private:
			ImageID icon;
	};
}