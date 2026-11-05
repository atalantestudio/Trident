// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	void Physics2DRenderer::build() {
		renderPass = createRenderPass();

		if (SAMPLE_COUNT != ImageSampleCount::x1) {
			if (layerInput->swapchainImage == nullptr) {
				recreateMultisampleImage();
				recreateMultisampleImageView();
			}
		}

		depthStencilImage = createDepthStencilImage();
		depthStencilImageView = createDepthStencilImageView();

		createFramebuffers();

		createCameraBuffer();
		createViewBuffer();
		createPlaceholderBuffer();

		#if defined(SHOW_DISTANCE)
			createClosestPointBuffer();
		#endif

		#if defined(SHOW_FEATURES)
			createVertexFeatureBuffer();
			createEdgeFeatureBuffer();
		#endif

		createCameraViewDescriptorSetLayout();
		createPlaceholderDescriptorSetLayout();

		allocateDescriptorSets();

		updateCameraViewDescriptorSet();
		updatePlaceholderDescriptorSet();

		createCameraViewPipelineLayout();
		createPlaceholderPipelineLayout();

		gridPipeline = createGridPipeline();
		circleFillPipeline = createCircleFillPipeline();
		rigidBodyFillPipeline = createRigidBodyFillPipeline();
		rigidBodyStrokePipeline = createRigidBodyStrokePipeline();
		centerOfMassPipeline = createCenterOfMassPipeline();
		rigidBodyPlaceholderFillPipeline = createRigidBodyPlaceholderFillPipeline();
		rigidBodyPlaceholderStrokePipeline = createRigidBodyPlaceholderStrokePipeline();
		rigidBodyPlaceholderCenterOfMassPipeline = createRigidBodyPlaceholderCenterOfMassPipeline();

		#if defined(SHOW_DISTANCE)
			distancePipeline = createDistancePipeline();
		#endif

		#if defined(SHOW_FEATURES)
			vertexFeaturePipeline = createVertexFeaturePipeline();
			edgeFeaturePipeline = createEdgeFeaturePipeline();
		#endif

		updateCameraProjection();
		updateViewport();
	}

	void Physics2DRenderer::postBuild() {
		transformBufferView = transformBuffer->createView<Transform2D>(0, transformBuffer->getSize());
		transformCount = 0;
		transformMappedRange = transformBufferView.map();
	}
}