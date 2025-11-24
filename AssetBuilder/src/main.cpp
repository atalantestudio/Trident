#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Process/BuildFontProcess.hpp"
#include "Process/BuildGeometryProcess.hpp"
#include "Process/BuildShaderModuleProcess.hpp"

namespace ProjectA {
	class AssetBuilderApplication : public Application {
		public:
			explicit AssetBuilderApplication(const ProjectDescriptor& projectDescriptor, uint32 argumentCount, char8** argumentValues) :
				Application(projectDescriptor)
			{
				setConsoleArguments(argumentCount, argumentValues);

				// registerProcess<BuildFontProcess>();
				// registerProcess<BuildGeometryProcess>();
				registerProcess<BuildShaderModuleProcess>();
			}
	};

	Application* createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::BUILD;
		projectDescriptor.name = "ProjectA Asset Builder";

		return new AssetBuilderApplication(projectDescriptor, argumentCount, argumentValues);
	}
}