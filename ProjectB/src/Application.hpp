// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Engine/API/Window/LayerGroup/UILayerGroup.hpp"
#include "Engine/Application/Application.hpp"

#include "Layer/DebugMenuLayer.hpp"
#include "Layer/SceneLayer.hpp"
#include "Layer/ViewModelLayer.hpp"

namespace Trident {
	class ProjectBApplication : public Application {
		public:
			explicit ProjectBApplication(const ProjectDescriptor& projectDescriptor, uint32 argumentCount, char8** argumentValues) :
				Application(projectDescriptor)
			{
				setConsoleArguments(argumentCount, argumentValues);

				registerLayer<SceneLayer>();
				registerLayer<ViewModelLayer>();
				registerLayer<DebugMenuLayer>();

				registerLayerGroup<UILayerGroup>(range<uint32, uint32>(2, 1));
			}
	};
}