#pragma once

namespace ProjectA {
	class DropDownMenuLayer : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFont("../ProjectA/src/Asset/Font/test-regular.ttf");
			}

			void GUI() override {
				//
			}

		private:
			FontID font;
	};
}