// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUILaunchLayer : public GUILayer {
		public:
			void registerAssets() override {
				logo = loadImageAsset("Cameraman");
			}

			void render() override {
				GUIComponent({
					.size = {400_px, 400_px},
					.image = logo,
				});
			}

		private:
			ImageID logo;
	};
}