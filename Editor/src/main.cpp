#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/PNGTestLayer.hpp"

namespace ProjectA {
	class EditorApplication : public Application {
		public:
			EditorApplication() : Application("ProjectA Editor") {
				pushLayer(new PNGTestLayer());
			}

			~EditorApplication() {}
	};

	Application* createApplication() {
		return new EditorApplication();
	}
}