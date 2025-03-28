#include "DemoGUILayer.hpp"

namespace ProjectA {
	DemoGUILayer::DemoGUILayer() {
		loadDefaultFont();

		//appendRoot(ColoredQuad({0.0f, 0.0f}, {64.0f, 64.0f}, 0));
		//appendRoot(ColoredQuad({16.0f, 80.0f}, {128.0f, 48.0f}, 1));
		//appendRoot(ColoredQuad({80.0f, 128.0f}, {256.0f, 512.0f}, 2));

		//appendRoot(TexturedQuad({80.0f, 128.0f}, {256.0f, 512.0f}, 0));

		//GUITextObjectDescriptor textDescriptor;
		//textDescriptor.font = fonts[0];

		//appendRoot(GUITextObject("text", textDescriptor));

		appendRoot<PersonListWindow>();

		compute();
		registerEntityPool();
	}
}