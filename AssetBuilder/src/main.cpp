#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Process/BuildProcess.hpp"

namespace ProjectA {
	class AssetBuilderApplication : public Application {
		public:
			AssetBuilderApplication(const ApplicationManifest& applicationManifest) :
				Application(applicationManifest)
			{
				registerProcess<BuildProcess>();
			}
	};

	Application* createApplication() {
		ApplicationManifest applicationManifest{};
		applicationManifest.projectName = "ProjectA Asset Builder";

		return new AssetBuilderApplication(applicationManifest);
	}
}