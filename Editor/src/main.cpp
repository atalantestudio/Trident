#include "ProjectA.hpp"
#include "ProjectA/entry_point.hpp"

#include "Layer/EditorLayer.hpp"

namespace ProjectA {
	class EditorApplication : public Application {
		public:
			EditorApplication() : Application("ProjectA Editor") {
				pushLayer(new EditorLayer());
			}

			~EditorApplication() {}
	};

	Application* createApplication() {
		return new EditorApplication();
	}
}