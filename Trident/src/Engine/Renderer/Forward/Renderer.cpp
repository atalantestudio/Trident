// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Renderer/Forward/ColorImage.hpp"
#include "Engine/Renderer/Forward/DebugContactManifoldBuffer.hpp"
#include "Engine/Renderer/Forward/DebugContactManifoldPipeline.hpp"
#include "Engine/Renderer/Forward/DebugContactManifoldPipelineLayout.hpp"
#include "Engine/Renderer/Forward/DebugHullWireframeBuffer.hpp"
#include "Engine/Renderer/Forward/DebugHullWireframePipeline.hpp"
#include "Engine/Renderer/Forward/DebugHullWireframePipelineLayout.hpp"
#include "Engine/Renderer/Forward/DepthStencilImage.hpp"
#include "Engine/Renderer/Forward/ForwardDescriptorSetLayout.hpp"
#include "Engine/Renderer/Forward/InstanceBuffer.hpp"
#include "Engine/Renderer/Forward/LightmapAtlasImage.hpp"
#include "Engine/Renderer/Forward/LightmapSampler.hpp"
#include "Engine/Renderer/Forward/Material.hpp"
#include "Engine/Renderer/Forward/MaterialSampler.hpp"
#include "Engine/Renderer/Forward/Renderer.hpp"
#include "Engine/Renderer/Forward/RenderPass.hpp"
#include "Engine/Renderer/Forward/StaticGeometryDescriptorSetLayout.hpp"
#include "Engine/Renderer/Forward/VertexBuffer.hpp"

namespace Trident {
	void ForwardRenderer::renderStaticGeometry(ref<CommandEncoder> commandEncoder) const {
		const view<StaticMaterial3D> staticMaterials = scene->getAsset()->getStaticMaterials();

		commandEncoder->bindGraphicsPipeline(materialPipelines[0]);
		commandEncoder->bindVertexBuffer(0, staticIndexBuffer, 0, sizeof(StaticVertex3D));
		commandEncoder->bindDescriptorSets(materialPipelineLayouts[0], 0, staticGeometryDescriptorSet);

		uint32 staticMaterialIndex = 0;

		while (staticMaterialIndex < staticMaterials.count()) {
			const StaticMaterial3D& staticMaterial = staticMaterials[staticMaterialIndex];

			// TODO(forward): Fix descriptor sets not being as many as the materials.
			commandEncoder->bindDescriptorSets(materialPipelineLayouts[0], 1, materialDescriptorSets[0]);

			commandEncoder->draw(staticMaterial.indexRange.count, 1, staticMaterial.indexRange.offset, 0);

			staticMaterialIndex += 1;
		}
	}
}