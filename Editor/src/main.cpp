#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/GUILayer.hpp"

namespace ProjectA {
	class EditorApplication : public Application {
		public:
			EditorApplication() : Application("ProjectA Editor") {
				pushLayer(new GUILayer());
			}

			~EditorApplication() {}
	};

	Application* createApplication() {
		return new EditorApplication();
	}
}