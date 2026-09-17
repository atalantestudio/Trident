// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Renderer/Forward/ColorImage.hpp"
// #include "Engine/Renderer/Forward/DebugContactManifoldBuffer.hpp"
// #include "Engine/Renderer/Forward/DebugContactManifoldPipeline.hpp"
// #include "Engine/Renderer/Forward/DebugContactManifoldPipelineLayout.hpp"
// #include "Engine/Renderer/Forward/DebugHullWireframeBuffer.hpp"
// #include "Engine/Renderer/Forward/DebugHullWireframePipeline.hpp"
// #include "Engine/Renderer/Forward/DebugHullWireframePipelineLayout.hpp"
#include "Engine/Renderer/Forward/DepthStencilImage.hpp"
#include "Engine/Renderer/Forward/DirectionalLightBuffer.hpp"
#include "Engine/Renderer/Forward/DirectionalShadowMap/DescriptorSetLayout.hpp"
#include "Engine/Renderer/Forward/DirectionalShadowMap/Framebuffer.hpp"
#include "Engine/Renderer/Forward/DirectionalShadowMap/Image.hpp"
#include "Engine/Renderer/Forward/DirectionalShadowMap/ImageView.hpp"
#include "Engine/Renderer/Forward/DirectionalShadowMap/Pipeline.hpp"
#include "Engine/Renderer/Forward/DirectionalShadowMap/PipelineLayout.hpp"
#include "Engine/Renderer/Forward/DirectionalShadowMap/RenderPass.hpp"
#include "Engine/Renderer/Forward/DirectionalShadowMap/Sampler.hpp"
#include "Engine/Renderer/Forward/DynamicPipeline.hpp"
#include "Engine/Renderer/Forward/Framebuffers.hpp"
#include "Engine/Renderer/Forward/GeometryDescriptorSets.hpp"
#include "Engine/Renderer/Forward/IndexBuffer.hpp"
#include "Engine/Renderer/Forward/InstanceBuffer.hpp"
#include "Engine/Renderer/Forward/LightmapAtlasDescriptorSetLayout.hpp"
#include "Engine/Renderer/Forward/LightmapAtlasImage.hpp"
#include "Engine/Renderer/Forward/LightmapAtlasSampler.hpp"
#include "Engine/Renderer/Forward/LightmapCoordinateBuffer.hpp"
#include "Engine/Renderer/Forward/Material.hpp"
#include "Engine/Renderer/Forward/MaterialSampler.hpp"
#include "Engine/Renderer/Forward/PointLightBuffer.hpp"
#include "Engine/Renderer/Forward/Renderer.hpp"
#include "Engine/Renderer/Forward/RenderPass.hpp"
#include "Engine/Renderer/Forward/SceneDescriptorSetLayout.hpp"
#include "Engine/Renderer/Forward/Skybox.hpp"
#include "Engine/Renderer/Forward/StaticPipeline.hpp"
#include "Engine/Renderer/Forward/VertexBuffer.hpp"

namespace Trident {
	void ForwardRenderer::render(ref<CommandEncoder> commandEncoder) const {
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();

		encodeDirectionalShadowMapPass(commandEncoder);

		const ref<Framebuffer> framebuffer = framebuffers[getSwapchainImageIndex()];
		const Extent2D extent = framebuffer->getExtent();
		const float32 width = static_cast<float32>(extent.width);
		const float32 height = static_cast<float32>(extent.height);
		const svec3<float32> skyColor = scene->getAsset()->skyColor;

		commandEncoder->beginRenderPass({
			.framebuffer = framebuffer,
			.clearValues = {
				{skyColor.x, skyColor.y, skyColor.z, 1.0f},
				{
					.depth = 1.0f,
					.stencil = 0,
				},
				#if FORWARD_SAMPLE_COUNT > 1
					{0.0f, 0.0f, 0.0f, 1.0f},
				#endif
			},
		});
		commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
		commandEncoder->setScissor(0, 0, extent);

		if (sceneAsset->skyboxImageAsset != nullptr) {
			encodeSkyboxPass(commandEncoder);
		}

		if (visualizationMode & VisualizationMode::DEFAULT) {
			const ref<DescriptorSet> descriptorSets[] = {sceneDescriptorSet, lightmapAtlasDescriptorSet};

			uint32 materialShaderIndex = 0;

			while (materialShaderIndex < sceneAsset->dynamicMaterialShaderOffset) {
				const MaterialShader& materialShader = sceneAsset->materialShaders[materialShaderIndex];

				commandEncoder->bindGraphicsPipeline(materialPipelines[materialShaderIndex]);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
				commandEncoder->bindVertexBuffer(2, instanceBuffer, 0);
				commandEncoder->bindIndexBuffer<IndexFormat::R32_UINT>(indexBuffer, 0);
				commandEncoder->bindDescriptorSets(materialPipelineLayouts[materialShaderIndex], 0, descriptorSets);

				uint32 materialIndex = 0;

				while (materialIndex < materialShader.materialRange.count) {
					const Material material = sceneAsset->materials[materialShader.materialRange.offset + materialIndex];

					commandEncoder->bindDescriptorSets(materialPipelineLayouts[materialShaderIndex], 2, materialDescriptorSets[materialShader.materialRange.offset + materialIndex]);

					uint32 materialGeometryIndex = 0;

					while (materialGeometryIndex < material.geometryRange.count) {
						const MaterialGeometry materialGeometry = sceneAsset->materialGeometries[material.geometryRange.offset + materialGeometryIndex];
						const range<uint32, uint32> indexRange = sceneAsset->geometryIndexRanges[materialGeometry.geometryIndex];
						const ref<Geometry3DAsset> geometryAsset = sceneAsset->geometryAssets[materialGeometry.geometryIndex];

						uint64 vertexCount = 0;
						uint32 instanceIndex = 0;

						if (geometryAsset != nullptr) {
							vertexCount = geometryAsset->vertices.count();
						}

						while (instanceIndex < materialGeometry.instanceRange.count) {
							// TODO(renderer/forward) Don't rebind the lightmap coordinate buffer every time an instance is drawn.
							commandEncoder->bindVertexBuffer(1, lightmapCoordinateBuffer, instanceIndex * vertexCount * sizeof(uint32));

							commandEncoder->drawIndexed(indexRange.count, 1, indexRange.offset, 0, materialGeometry.instanceRange.offset + instanceIndex);

							instanceIndex += 1;
						}

						materialGeometryIndex += 1;
					}

					materialIndex += 1;
				}

				materialShaderIndex += 1;
			}

			while (materialShaderIndex < sceneAsset->materialShaders.count()) {
				const MaterialShader& materialShader = sceneAsset->materialShaders[materialShaderIndex];

				commandEncoder->bindGraphicsPipeline(materialPipelines[materialShaderIndex]);
				commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
				commandEncoder->bindVertexBuffer(1, instanceBuffer, 0);
				commandEncoder->bindIndexBuffer<IndexFormat::R32_UINT>(indexBuffer, 0);
				commandEncoder->bindDescriptorSets(materialPipelineLayouts[materialShaderIndex], 0, sceneDescriptorSet);

				uint32 materialIndex = 0;

				while (materialIndex < materialShader.materialRange.count) {
					const Material material = sceneAsset->materials[materialShader.materialRange.offset + materialIndex];

					commandEncoder->bindDescriptorSets(materialPipelineLayouts[materialShaderIndex], 1, materialDescriptorSets[materialShader.materialRange.offset + materialIndex]);

					uint32 materialGeometryIndex = 0;

					while (materialGeometryIndex < material.geometryRange.count) {
						const MaterialGeometry materialGeometry = sceneAsset->materialGeometries[material.geometryRange.offset + materialGeometryIndex];
						const range<uint32, uint32> indexRange = sceneAsset->geometryIndexRanges[materialGeometry.geometryIndex];

						commandEncoder->drawIndexed(indexRange.count, materialGeometry.instanceRange.count, indexRange.offset, 0, materialGeometry.instanceRange.offset);

						materialGeometryIndex += 1;
					}

					materialIndex += 1;
				}

				materialShaderIndex += 1;
			}
		}

		if (visualizationMode & VisualizationMode::HULL_WIREFRAMES && debugHullWireframeBuffer != nullptr) {
			// TODO(renderer/forward): Restore debug modes.
			/* commandEncoder->bindGraphicsPipeline(debugHullWireframePipeline);
			commandEncoder->bindDescriptorSets(debugHullWireframePipelineLayout, 0, dynamicDescriptorSet);
			commandEncoder->bindVertexBuffer(0, instanceBuffer, 0);
			commandEncoder->bindIndexBuffer<IndexFormat::R16_UINT>(debugHullWireframeBuffer, 0);

			uint32 materialShaderIndex = sceneAsset->dynamicMaterialShaderOffset;

			while (materialShaderIndex < sceneAsset->materialShaders.count()) {
				const MaterialShader& materialShader = sceneAsset->materialShaders[materialShaderIndex];

				uint32 materialIndex = 0;

				while (materialIndex < materialShader.materialRange.count) {
					const Material& material = sceneAsset->materials[materialShader.materialRange.offset + materialIndex];

					uint32 materialGeometryIndex = 0;

					while (materialGeometryIndex < material.geometryRange.count) {
						const MaterialGeometry& materialGeometry = sceneAsset->materialGeometries[material.geometryRange.offset + materialGeometryIndex];

						// TODO(renderer/forward)
						const Geometry3D& geometry = scene->geometries[0];

						if (geometry.hullIndex >= scene->hulls.count()) {
							materialGeometryIndex += 1;

							continue;
						}

						const Hull3D& hull = scene->hulls[geometry.hullIndex];

						// TODO(graphics/dx12/draw-indexed): DX12 doesn't pass BaseVertexOffset to the shader so indexing the vertex buffer doesn't return the same values as OpenGL/Vulkan.
						commandEncoder->drawIndexed(hull.lineIndexRange.count, 1, hull.lineIndexRange.offset, geometry.getVertexPositionComponentRange().offset / 3, materialGeometry.instanceRange.offset);

						materialGeometryIndex += 1;
					}

					materialIndex += 1;
				}

				materialShaderIndex += 1;
			} */
		}

		if (visualizationMode & VisualizationMode::CONTACT_MANIFOLDS) {
			// TODO(renderer/forward): Restore debug visualization modes.
			/* commandEncoder->bindGraphicsPipeline(debugContactManifoldPipeline);
			commandEncoder->bindDescriptorSets(debugContactManifoldPipelineLayout, 0, dynamicDescriptorSet);
			commandEncoder->bindVertexBuffer(0, debugContactManifoldBuffer, 0);
			commandEncoder->draw(contactCount, 1, 0, 0); */
		}

		commandEncoder->endRenderPass();
	}

	void ForwardRenderer::encodeDirectionalShadowMapPass(ref<CommandEncoder> commandEncoder) const {
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const uint32 swapchainImageCount = getSwapchain()->getImageCount();

		uint32 cascadeIndex = 0;

		while (cascadeIndex < DIRECTIONAL_SHADOW_MAP_CASCADE_COUNT) {
			const ref<Framebuffer> framebuffer = directionalShadowMapFramebuffers[getSwapchainImageIndex() * swapchainImageCount + cascadeIndex];
			const Extent2D extent = framebuffer->getExtent();
			const float32 width = static_cast<float32>(extent.width);
			const float32 height = static_cast<float32>(extent.height);

			commandEncoder->beginRenderPass({
				.framebuffer = framebuffer,
				.clearValues = {
					{
						.depth = 1.0f,
						.stencil = 0,
					},
				},
			});
			commandEncoder->setViewport({0.0f, height}, {width, -height}, 0.0f, 1.0f);
			commandEncoder->setScissor(0, 0, extent);
			commandEncoder->bindGraphicsPipeline(directionalShadowMapPipeline);
			commandEncoder->bindVertexBuffer(0, vertexBuffer, 0);
			commandEncoder->bindVertexBuffer(1, instanceBuffer, 0);
			commandEncoder->bindIndexBuffer<IndexFormat::R32_UINT>(indexBuffer, 0);
			commandEncoder->bindDescriptorSets(directionalShadowMapPipelineLayout, 0, directionalShadowMapCascadeDescriptorSets[cascadeIndex]);

			for (const MaterialShader& materialShader : sceneAsset->materialShaders) {
				uint32 materialIndex = 0;

				while (materialIndex < materialShader.materialRange.count) {
					const Material material = sceneAsset->materials[materialShader.materialRange.offset + materialIndex];

					uint32 materialGeometryIndex = 0;

					while (materialGeometryIndex < material.geometryRange.count) {
						const MaterialGeometry materialGeometry = sceneAsset->materialGeometries[material.geometryRange.offset + materialGeometryIndex];
						const range<uint32, uint32> indexRange = sceneAsset->geometryIndexRanges[materialGeometry.geometryIndex];

						commandEncoder->drawIndexed(indexRange.count, materialGeometry.instanceRange.count, indexRange.offset, 0, materialGeometry.instanceRange.offset);

						materialGeometryIndex += 1;
					}

					materialIndex += 1;
				}
			}

			commandEncoder->endRenderPass();

			cascadeIndex += 1;
		}
	}

	void ForwardRenderer::encodeSkyboxPass(ref<CommandEncoder> commandEncoder) const {
		commandEncoder->bindGraphicsPipeline(skyboxPipeline);
		commandEncoder->bindDescriptorSets(skyboxPipelineLayout, 0, skyboxDescriptorSet);
		commandEncoder->draw(36, 1, 0, 0);
	}

	void ForwardRenderer::createFrameObjects() {
		#if FORWARD_SAMPLE_COUNT > 1
			colorImage = createColorImage();
			colorImageView = createColorImageView();
		#endif

		depthStencilImage = createDepthStencilImage();
		depthStencilImageView = createDepthStencilImageView();

		createFramebuffers();
	}

	void ForwardRenderer::updateCamera() {
		const PerspectiveCamera3D& camera = getCameraController<PerspectiveCamera3D>().getCamera();

		mat4x4<float32> view = camera.getView();
		view[3] = {0.0f, 0.0f, 0.0f, 1.0f};

		cameraMappedRange->projectionView = camera.getProjectionView();
		cameraMappedRange->position = svec3<float32>(camera.position);
		cameraMappedRange->skyboxProjectionView = view * camera.getProjection();
		cameraMappedRange->view = camera.getView();

		initializeLightProjectionViews();
	}

	void ForwardRenderer::initializeLightProjectionViews() const {
		const PerspectiveCamera3D& camera = getCameraController<PerspectiveCamera3D>().getCamera();
		const float32 cameraZLength = camera.farPlane - camera.nearPlane;
		const vec3<float32> lightDirection = normalized(vec3<float32>(scene->getAsset()->sunDirection));

		vec4<float32> frustumVertices[8];
		float32 nearPlane = camera.nearPlane;
		uint32 cascadeIndex = 0;

		while (cascadeIndex < DIRECTIONAL_SHADOW_MAP_CASCADE_COUNT) {
			const float32 farPlane = camera.nearPlane + cameraZLength * DIRECTIONAL_SHADOW_MAP_CASCADE_FAR_PLANES[cascadeIndex];
			const mat4x4<float32> cameraProjection = perspective(camera.fieldOfView, camera.aspectRatio, nearPlane, farPlane, 1.0f);
			const mat4x4<float32> cameraProjectionView = camera.getView() * cameraProjection;

			calculateFrustum(frustumVertices, inverseFix(cameraProjectionView));

			vec3<float32> frustumCenter = {0.0f, 0.0f, 0.0f};

			for (const vec4<float32> vertex : frustumVertices) {
				frustumCenter += vec3<float32>(vertex.m);
			}

			frustumCenter /= 8.0f;

			const mat4x4<float32> lightView = mat4x4<float32>::lookAt(frustumCenter + lightDirection, frustumCenter, UP);

			float32 minX = max<float32>();
			float32 maxX = min<float32>();
			float32 minY = max<float32>();
			float32 maxY = min<float32>();
			float32 minZ = max<float32>();
			float32 maxZ = min<float32>();

			for (const vec4<float32> vertex : frustumVertices) {
				const vec4<float32> lightViewVertex = vertex * lightView;

				minX = min(minX, lightViewVertex[0]);
				maxX = max(maxX, lightViewVertex[0]);
				minY = min(minY, lightViewVertex[1]);
				maxY = max(maxY, lightViewVertex[1]);
				minZ = min(minZ, lightViewVertex[2]);
				maxZ = max(maxZ, lightViewVertex[2]);
			}

			// TODO(renderer/forward/directional-shadow-map): Use the scene bounds.
			constexpr float32 Z_SCALE = 10.0f;

			if (minZ < 0.0f) {
				minZ *= Z_SCALE;
			}
			else {
				minZ /= Z_SCALE;
			}

			if (maxZ < 0.0f) {
				maxZ /= Z_SCALE;
			}
			else {
				maxZ *= Z_SCALE;
			}

			// TODO(lyah/mat4x4/orthographic): Add helper similar to orthoLH_ZO.
			const mat4x4<float32> lightProjection = {
				 2.0f / (maxX - minX),           0.0f,                           0.0f,                  0.0f,
				 0.0f,                           2.0f / (maxY - minY),           0.0f,                  0.0f,
				 0.0f,                           0.0f,                           1.0f / (maxZ - minZ),  0.0f,
				-(minX + maxX) / (maxX - minX), -(minY + maxY) / (maxY - minY), -minZ / (maxZ - minZ),  1.0f,
			};

			directionalLightMappedRange->projectionViews[cascadeIndex] = lightView * lightProjection;

			nearPlane = farPlane;
			cascadeIndex += 1;
		}
	}

	void ForwardRenderer::calculateFrustum(vec4<float32>* vertices, const mat4x4<float32>& inverseProjectionView) const {
		uint32 vertexIndex = 0;

		while (vertexIndex < 8) {
			const vec4<float32> vertex = FRUSTUM_VERTICES[vertexIndex] * inverseProjectionView;

			vertices[vertexIndex] = vertex / vertex[3];

			vertexIndex += 1;
		}
	}
}