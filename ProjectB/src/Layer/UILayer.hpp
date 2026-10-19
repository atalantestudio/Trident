// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Engine/API/GUI/GUILayer.hpp"

namespace ProjectB {
	class UILayer : public Trident::GUILayer {
		public:
			void registerAssets() override {
				ref<ImageAsset> studioIconImageAsset = getApplication()->getAssetLibrary()->getAsset<ImageAsset>("studio_icon");
				sequence<uint8> data(studioIconImageAsset->extent.x * studioIconImageAsset->extent.y * getByteSize(ImageFormat::R8G8B8A8_UNORM));

				convert<ImageFormat::R8_UNORM, ImageFormat::R8G8B8A8_UNORM>(studioIconImageAsset->data, &data[0], data.count());

				studioIconImageAsset->format = ImageFormat::R8G8B8A8_UNORM;
				studioIconImageAsset->data = data;

				studioIcon = registerImage(studioIconImageAsset);
			}

		public:
			void render() final override {
				const Extent2D extent = getApplication()->getWindow()->getExtent();

				Container({
					.size = {static_cast<float32>(extent.x), static_cast<float32>(extent.y)},
					.anchorPoint = AnchorPoint::HCENTER | AnchorPoint::VCENTER,
				}) {
					Container({
						.size = {256_px, 256_px},
						.color = 0xffffffff,
						.image = studioIcon,
					});
				}
			}

		private:
			ImageID studioIcon;
	};
}