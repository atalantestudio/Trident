#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/DemoGUILayer.hpp"
#include "Layer/DemoLayer.hpp"

namespace ProjectA {
	class EditorApplication : public Application {
		public:
			EditorApplication(const ConfigurationFile& configurationFile) :
				Application("ProjectA Editor", configurationFile)
			{
				registerLayer<DemoLayer>();
				registerLayer<DemoGUILayer>();
			}

			~EditorApplication() {}
	};

	Application* createApplication() {
		const ConfigurationFile configurationFile = Filesystem::load<ConfigurationFile>("Editor.tco");

		return new EditorApplication(configurationFile);
	}
}