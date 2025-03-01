#include "ProjectA.hpp"
#include "Engine/entry_point.hpp"

#include "Layer/TriangleDemoLayer.hpp"

namespace ProjectA {
	class EditorApplication : public Application {
		public:
			EditorApplication() : Application("ProjectA Editor") {
				pushLayer(new TriangleDemoLayer());
			}

			~EditorApplication() {}
	};

	Application* createApplication() {
		return new EditorApplication();
	}
}