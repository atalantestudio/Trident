#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/DemoLayer.hpp"

namespace ProjectA {
	class EditorApplication : public Application {
		public:
			EditorApplication() : Application("ProjectA Editor") {
				pushLayer(new DemoLayer());
			}

			~EditorApplication() {}
	};

	Application* createApplication() {
		return new EditorApplication();
	}
}