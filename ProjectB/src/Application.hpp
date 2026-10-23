// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Engine/API/Window/LayerGroup/UILayerGroup.hpp"
#include "Engine/Application/Application.hpp"

#include "Layer/DebugConsoleLayer.hpp"
#include "Layer/SceneLayer.hpp"
#include "Layer/UILayer.hpp"

namespace ProjectB {
	class Application : public Trident::Application {
		public:
			explicit Application(const ProjectDescriptor& projectDescriptor, uint32 argumentCount, char8** argumentValues) :
				Trident::Application(projectDescriptor, argumentCount, argumentValues)
			{
				registerLayer<SceneLayer>();
				registerLayer<UILayer>();
				registerLayer<DebugConsoleLayer>();

				registerLayerGroup<LayerGroup>(range<uint32, uint32>(0, 1));
				registerLayerGroup<UILayerGroup>(range<uint32, uint32>(1, 2));
			}
	};
}