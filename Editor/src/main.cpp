#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/EditorLayer.hpp"

namespace ProjectA {
	class EditorApplication : public Application {
		public:
			EditorApplication(const ConfigurationFile& configurationFile) :
				Application("ProjectA Editor", configurationFile)
			{
				registerLayer<EditorLayer>();
			}

			~EditorApplication() {}
	};

	Application* createApplication() {
		const ConfigurationFile configurationFile = Filesystem::load<ConfigurationFile>("Editor.tco");

		return new EditorApplication(configurationFile);
	}
}