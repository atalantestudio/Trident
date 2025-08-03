#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Process/BuildProcess.hpp"

namespace ProjectA {
	class AssetBuilderApplication : public Application {
		public:
			explicit AssetBuilderApplication(const ApplicationManifest& applicationManifest, uint32 argumentCount, char** argumentValues) :
				Application(applicationManifest)
			{
				setConsoleArguments(argumentCount, argumentValues);

				registerProcess<BuildProcess>(getConsoleArguments());
			}
	};

	Application* createApplication(uint32 argumentCount, char** argumentValues) {
		ApplicationManifest applicationManifest{};
		applicationManifest.projectName = "ProjectA Asset Builder";

		return new AssetBuilderApplication(applicationManifest, argumentCount, argumentValues);
	}
}