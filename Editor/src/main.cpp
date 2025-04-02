#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/DemoLayer.hpp"

namespace ProjectA {
	class EditorApplication : public Application {
		public:
			EditorApplication(const ConfigurationFile& configurationFile) :
				Application("ProjectA Editor", configurationFile)
			{
				const TTFFile ttfFile = Filesystem::load<TTFFile>("assets/fonts/test.ttf");

				registerLayer<DemoLayer>(ttfFile);
			}

			~EditorApplication() {}
	};

	Application* createApplication() {
		const ConfigurationFile configurationFile = Filesystem::load<ConfigurationFile>("Editor.tco");

		return new EditorApplication(configurationFile);
	}
}