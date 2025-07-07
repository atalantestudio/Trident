#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/EditorLayer.hpp"

namespace ProjectA {
	class EditorApplication : public Application {
		public:
			EditorApplication(const ApplicationManifest& applicationManifest) :
				Application(applicationManifest)
			{
				registerLayer<EditorLayer>();
			}
	};

	Application* createApplication() {
		ApplicationManifest applicationManifest{};
		applicationManifest.projectName = (std::string(ENGINE_NAME) + " " + PROJECT_NAME).data();

		return new EditorApplication(applicationManifest);
	}
}