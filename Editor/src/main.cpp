#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/DemoGUILayer.hpp"
//#include "Layer/DemoLayer.hpp"

namespace ProjectA {
	class EditorApplication : public Application {
		public:
			EditorApplication() : Application("ProjectA Editor") {
				//pushLayer(new DemoLayer());
				pushLayer(new DemoGUILayer());
			}

			~EditorApplication() {}
	};

	Application* createApplication() {
		return new EditorApplication();
	}
}