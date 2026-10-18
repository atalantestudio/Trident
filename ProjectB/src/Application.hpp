// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Engine/API/Window/LayerGroup/UILayerGroup.hpp"
#include "Engine/Application/Application.hpp"

#include "Layer/DebugMenuLayer.hpp"
#include "Layer/SceneLayer.hpp"
#include "Layer/ViewModelLayer.hpp"

namespace ProjectB {
	class Application : public Trident::Application {
		public:
			explicit Application(const ProjectDescriptor& projectDescriptor, uint32 argumentCount, char8** argumentValues) :
				Trident::Application(projectDescriptor)
			{
				setConsoleArguments(argumentCount, argumentValues);

				registerLayer<SceneLayer>();
				registerLayer<ViewModelLayer>();
				registerLayer<DebugMenuLayer>();

				registerLayerGroup<LayerGroup>(range<uint32, uint32>(0, 2));
				registerLayerGroup<UILayerGroup>(range<uint32, uint32>(2, 1));
			}
	};
}