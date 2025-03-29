#pragma once

namespace ProjectA {
	struct Person {
		uint32 id;
		std::string name;
		std::string iconUrl;
	};

	class TestGUILayer : public GUILayer {
		public:
			TestGUILayer() {
				GUIWindow();

				if (GUIButton()) {
					GUIText("Button pressed.");
				}
			}
	};
}