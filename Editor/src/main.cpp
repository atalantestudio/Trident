#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/EditorLayer.hpp"

namespace ProjectA {
	class EditorApplication : public Application {
		public:
			EditorApplication(const WindowDescriptor& windowDescriptor, const ConfigurationFile& configurationFile) :
				Application(windowDescriptor, configurationFile)
			{
				registerLayer<EditorLayer>();
			}

			~EditorApplication() {}
	};

	Application* createApplication() {
		const ConfigurationFile configurationFile = Filesystem::load<ConfigurationFile>("Editor.tco");

		WindowDescriptor windowDescriptor{};
		windowDescriptor.name = "ProjectA Editor";
		windowDescriptor.width = configurationFile.get<uint32>("ExtentWidth");
		windowDescriptor.height = configurationFile.get<uint32>("ExtentHeight");

		return new EditorApplication(windowDescriptor, configurationFile);
	}
}