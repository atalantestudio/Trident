#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/EditorLayer.hpp"

namespace ProjectA {
	class EditorApplication : public Application {
		public:
			EditorApplication(const WindowDescriptor& windowDescriptor) :
				Application(windowDescriptor)
			{
				registerLayer<EditorLayer>();
			}

			~EditorApplication() {}
	};

	Application* createApplication() {
		WindowDescriptor windowDescriptor{};
		windowDescriptor.name = "ProjectA Editor";
		windowDescriptor.width = 800;
		windowDescriptor.height = 800;

		return new EditorApplication(windowDescriptor);
	}
}