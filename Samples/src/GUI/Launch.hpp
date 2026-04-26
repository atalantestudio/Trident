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
				const sint32* size = getApplication()->getWindow()->getSize();

				GUIComponent({
					.size = {static_cast<float32>(size[0]), static_cast<float32>(size[1])},
					.anchorPoint = AnchorPoint::HCENTER | AnchorPoint::VCENTER,
				}) {
					GUIComponent({
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