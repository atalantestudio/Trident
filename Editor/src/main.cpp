#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/EditorLayer.hpp"

namespace ProjectA {
	class EditorApplication : public Application {
		public:
			EditorApplication(const ApplicationDescriptor& applicationDescriptor) :
				Application(applicationDescriptor)
			{
				registerLayer<EditorLayer>();
			}

			~EditorApplication() {}
	};

	Application* createApplication() {
		ApplicationDescriptor applicationDescriptor{};
		applicationDescriptor.projectName = (std::string(ENGINE_NAME) + " " + PROJECT_NAME).data();

		return new EditorApplication(applicationDescriptor);
	}
}