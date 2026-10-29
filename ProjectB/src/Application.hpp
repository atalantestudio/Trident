// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Application/Application.hpp"

#include "Layer/ConsoleLayer.hpp"
#include "Layer/MapLayer.hpp"
#include "Layer/UILayer.hpp"
#include "LayerGroup/FinalUILayerGroup.hpp"
#include "LayerGroup/UILayerGroup.hpp"
#include "Renderer/MapRenderer/MapRenderer.hpp"

namespace ProjectB {
	class Application : public Trident::Application {
		public:
			explicit Application(const ProjectDescriptor& projectDescriptor, uint32 argumentCount, char8** argumentValues) :
				Trident::Application(projectDescriptor, argumentCount, argumentValues)
			{
				registerLayer<UILayer>();
				registerLayer<MapLayer>();
				registerLayer<ConsoleLayer>();

				registerLayerGroup<UILayerGroup>(range<uint32, uint32>(0, 1));
				registerLayerGroup<LayerGroup>(range<uint32, uint32>(1, 1));
				registerLayerGroup<FinalUILayerGroup>(range<uint32, uint32>(2, 1));
			}

			Renderer* getMainRenderer() const override {
				const ref<MapLayer> mapLayer = ref_cast<MapLayer>(getLayerCompositor()->getLayers()[1]);

				return &mapLayer->renderer;
			}

		public:
			bool showConsole = false;
	};
}