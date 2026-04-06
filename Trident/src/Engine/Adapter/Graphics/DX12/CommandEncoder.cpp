// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/BarrierAccess.hpp"
#include "Engine/Adapter/Graphics/DX12/BarrierLayout.hpp"
#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/DX12/CommandPool.hpp"
#include "Engine/Adapter/Graphics/DX12/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/DescriptorSet.hpp"
#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/DX12/GraphicsPipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/ImageView.hpp"
#include "Engine/Adapter/Graphics/DX12/ResourceState.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/DispatchIndirectCommand.hpp"
#include "Engine/API/Graphics/DrawIndexedIndirectCommand.hpp"
#include "Engine/API/Graphics/DrawIndirectCommand.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	static void setRenderTargets(DX12::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		commandEncoder->presentRenderTargetIndex = max<UINT>();

		const ref<RenderPass> renderPass = commandEncoder->framebuffer->getRenderPass();
		const ref<Subpass> subpass = renderPass->getSubpass(commandEncoder->subpassIndex);
		const view<AttachmentReference> colorAttachmentReferences = subpass->getColorAttachmentReferences();

		if (subpass->getInputAttachmentReferenceCount() > 0) {
			const view<AttachmentReference> inputAttachmentReferences = subpass->getInputAttachmentReferences();

			sequence<D3D12_TEXTURE_BARRIER> textureBarriers(subpass->getInputAttachmentReferenceCount());
			UINT inputAttachmentReferenceIndex = 0;

			while (inputAttachmentReferenceIndex < inputAttachmentReferences.count()) {
				const AttachmentReference& inputAttachmentReference = inputAttachmentReferences[inputAttachmentReferenceIndex];

				if (inputAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
					ENGINE_ERROR_UNIMPLEMENTED();

					inputAttachmentReferenceIndex += 1;

					continue;
				}

				const ref<Trident::ImageView> imageView = commandEncoder->framebuffer->imageViews[inputAttachmentReference.index];
				Trident::Image* image = imageView->getImage();
				DX12::Image* dxImage = static_cast<DX12::Image*>(image);

				D3D12_TEXTURE_BARRIER& textureBarrier = textureBarriers[inputAttachmentReferenceIndex];
				textureBarrier.SyncBefore = D3D12_BARRIER_SYNC_ALL;
				textureBarrier.SyncAfter = D3D12_BARRIER_SYNC_ALL;
				textureBarrier.AccessBefore = dxImage->barrierAccess;
				textureBarrier.AccessAfter = D3D12_BARRIER_ACCESS_SHADER_RESOURCE;
				textureBarrier.LayoutBefore = dxImage->barrierLayout;
				textureBarrier.LayoutAfter = D3D12_BARRIER_LAYOUT_SHADER_RESOURCE;
				textureBarrier.pResource = dxImage->resource;
				textureBarrier.Subresources.IndexOrFirstMipLevel = imageView->getMipLevelOffset();
				textureBarrier.Subresources.NumMipLevels = imageView->getMipLevelCount();
				textureBarrier.Subresources.FirstArraySlice = imageView->getLayerOffset();
				textureBarrier.Subresources.NumArraySlices = imageView->getLayerCount();
				textureBarrier.Subresources.FirstPlane = 0;
				textureBarrier.Subresources.NumPlanes = 1;
				textureBarrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;

				dxImage->barrierAccess = textureBarrier.AccessAfter;
				dxImage->barrierLayout = textureBarrier.LayoutAfter;

				image->layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

				inputAttachmentReferenceIndex += 1;
			}

			D3D12_BARRIER_GROUP barrierGroup{};
			barrierGroup.Type = D3D12_BARRIER_TYPE_TEXTURE;
			barrierGroup.NumBarriers = static_cast<UINT>(textureBarriers.count());
			barrierGroup.pTextureBarriers = &textureBarriers[0];

			commandEncoder->commandList->Barrier(1, &barrierGroup);
		}

		UINT renderTargetCount = 0;

		for (const AttachmentReference& colorAttachmentReference : colorAttachmentReferences) {
			if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
				continue;
			}

			renderTargetCount += 1;
		}

		sequence<D3D12_CPU_DESCRIPTOR_HANDLE> renderTargetViewHandles(renderTargetCount);
		std::vector<D3D12_TEXTURE_BARRIER> textureBarriers;
		UINT renderTargetIndex = 0;
		UINT colorAttachmentIndex = 0;

		while (renderTargetIndex < renderTargetCount) {
			const AttachmentReference& colorAttachmentReference = colorAttachmentReferences[renderTargetIndex];

			if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
				ENGINE_ERROR_UNIMPLEMENTED();

				renderTargetIndex += 1;

				continue;
			}

			const Attachment& colorAttachment = renderPass->getAttachment(colorAttachmentReference.index);
			const ref<DX12::ImageView> imageView = std::static_pointer_cast<DX12::ImageView>(commandEncoder->framebuffer->imageViews[colorAttachmentReference.index]);

			DX12::Image* image = static_cast<DX12::Image*>(imageView->getImage());

			if (colorAttachment.finalLayout == ImageLayout::PRESENT_SRC) {
				commandEncoder->presentRenderTargetIndex = colorAttachmentReference.index;

				if (colorAttachment.loadOperation != LoadOperation::CLEAR && image->barrierLayout != D3D12_BARRIER_LAYOUT_RENDER_TARGET) {
					D3D12_TEXTURE_BARRIER textureBarrier{};
					textureBarrier.SyncBefore = D3D12_BARRIER_SYNC_ALL;
					textureBarrier.SyncAfter = D3D12_BARRIER_SYNC_ALL;
					textureBarrier.AccessBefore = image->barrierAccess;
					textureBarrier.AccessAfter = D3D12_BARRIER_ACCESS_RENDER_TARGET;
					textureBarrier.LayoutBefore = image->barrierLayout;
					textureBarrier.LayoutAfter = D3D12_BARRIER_LAYOUT_RENDER_TARGET;
					textureBarrier.pResource = image->resource;
					textureBarrier.Subresources.IndexOrFirstMipLevel = imageView->getMipLevelOffset();
					textureBarrier.Subresources.NumMipLevels = imageView->getMipLevelCount();
					textureBarrier.Subresources.FirstArraySlice = imageView->getLayerOffset();
					textureBarrier.Subresources.NumArraySlices = imageView->getLayerCount();
					textureBarrier.Subresources.FirstPlane = 0;
					textureBarrier.Subresources.NumPlanes = 1;
					textureBarrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;

					image->barrierAccess = textureBarrier.AccessAfter;
					image->barrierLayout = textureBarrier.LayoutAfter;

					image->layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

					textureBarriers.emplace_back(textureBarrier);
				}
			}

			if (image->usage & ImageUsage::INPUT_ATTACHMENT && colorAttachment.loadOperation != LoadOperation::CLEAR) {
				D3D12_TEXTURE_BARRIER textureBarrier{};
				textureBarrier.SyncBefore = D3D12_BARRIER_SYNC_ALL;
				textureBarrier.SyncAfter = D3D12_BARRIER_SYNC_ALL;
				textureBarrier.AccessBefore = image->barrierAccess;
				textureBarrier.AccessAfter = D3D12_BARRIER_ACCESS_RENDER_TARGET;
				textureBarrier.LayoutBefore = image->barrierLayout;
				textureBarrier.LayoutAfter = D3D12_BARRIER_LAYOUT_RENDER_TARGET;
				textureBarrier.pResource = image->resource;
				textureBarrier.Subresources.IndexOrFirstMipLevel = imageView->getMipLevelOffset();
				textureBarrier.Subresources.NumMipLevels = imageView->getMipLevelCount();
				textureBarrier.Subresources.FirstArraySlice = imageView->getLayerOffset();
				textureBarrier.Subresources.NumArraySlices = imageView->getLayerCount();
				textureBarrier.Subresources.FirstPlane = 0;
				textureBarrier.Subresources.NumPlanes = 1;
				textureBarrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;

				image->barrierAccess = textureBarrier.AccessAfter;
				image->barrierLayout = textureBarrier.LayoutAfter;

				image->layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;

				textureBarriers.emplace_back(textureBarrier);
			}

			renderTargetViewHandles[colorAttachmentIndex] = imageView->descriptorHandle;
			colorAttachmentIndex += 1;
			renderTargetIndex += 1;
		}

		if (textureBarriers.size() > 0) {
			D3D12_BARRIER_GROUP barrierGroup{};
			barrierGroup.Type = D3D12_BARRIER_TYPE_TEXTURE;
			barrierGroup.NumBarriers = static_cast<UINT>(textureBarriers.size());
			barrierGroup.pTextureBarriers = &textureBarriers[0];

			commandEncoder->commandList->Barrier(1, &barrierGroup);
		}

		const D3D12_CPU_DESCRIPTOR_HANDLE* depthStencilViewHandlePointer = nullptr;

		D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle{};

		if (subpass->hasDepthStencilAttachmentReference()) {
			const AttachmentReference& depthStencilAttachmentReference = subpass->getDepthStencilAttachmentReference();
			const ref<DX12::ImageView> imageView = std::static_pointer_cast<DX12::ImageView>(commandEncoder->framebuffer->imageViews[depthStencilAttachmentReference.index]);

			DX12::Image* image = static_cast<DX12::Image*>(imageView->getImage());

			if (image->usage & ImageUsage::INPUT_ATTACHMENT && renderPass->getAttachment(depthStencilAttachmentReference.index).loadOperation != LoadOperation::CLEAR) {
				D3D12_TEXTURE_BARRIER textureBarrier{};
				textureBarrier.SyncBefore = D3D12_BARRIER_SYNC_ALL;
				textureBarrier.SyncAfter = D3D12_BARRIER_SYNC_ALL;
				textureBarrier.AccessBefore = image->barrierAccess;
				textureBarrier.AccessAfter = D3D12_BARRIER_ACCESS_DEPTH_STENCIL_WRITE;
				textureBarrier.LayoutBefore = image->barrierLayout;
				textureBarrier.LayoutAfter = D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE;
				textureBarrier.pResource = image->resource;
				textureBarrier.Subresources.IndexOrFirstMipLevel = imageView->getMipLevelOffset();
				textureBarrier.Subresources.NumMipLevels = imageView->getMipLevelCount();
				textureBarrier.Subresources.FirstArraySlice = imageView->getLayerOffset();
				textureBarrier.Subresources.NumArraySlices = imageView->getLayerCount();
				textureBarrier.Subresources.FirstPlane = 0;
				textureBarrier.Subresources.NumPlanes = 1;
				textureBarrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;

				image->barrierAccess = textureBarrier.AccessAfter;
				image->barrierLayout = textureBarrier.LayoutAfter;

				image->layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				D3D12_BARRIER_GROUP barrierGroup{};
				barrierGroup.Type = D3D12_BARRIER_TYPE_TEXTURE;
				barrierGroup.NumBarriers = 1;
				barrierGroup.pTextureBarriers = &textureBarrier;

				commandEncoder->commandList->Barrier(1, &barrierGroup);
			}

			depthStencilViewHandle = imageView->descriptorHandle;
			depthStencilViewHandlePointer = &depthStencilViewHandle;
		}

		commandEncoder->commandList->OMSetRenderTargets(renderTargetCount, &renderTargetViewHandles[0], false, depthStencilViewHandlePointer);
	}

	DX12::CommandEncoder::CommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool) :
		Trident::CommandEncoder(device, commandPool)
	{}

	void DX12::begin(const Trident::CommandEncoder* commandEncoder) {}

	void DX12::beginRenderPass(Trident::CommandEncoder* commandEncoder, const BeginRenderPassDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);

		_commandEncoder->framebuffer = std::static_pointer_cast<DX12::Framebuffer>(descriptor.framebuffer);

		const ref<RenderPass> renderPass = _commandEncoder->framebuffer->getRenderPass();

		UINT attachmentIndex = 0;

		D3D12_RECT rectangle{};
		rectangle.left = 0;
		rectangle.top = 0;
		rectangle.right = _commandEncoder->framebuffer->getExtent().width;
		rectangle.bottom = _commandEncoder->framebuffer->getExtent().height;

		// TODO: Use a sequence instead.
		std::vector<D3D12_TEXTURE_BARRIER> textureBarriers;

		while (attachmentIndex < renderPass->getAttachmentCount()) {
			const ref<DX12::ImageView> imageView = std::static_pointer_cast<DX12::ImageView>(_commandEncoder->framebuffer->imageViews[attachmentIndex]);

			DX12::Image* image = static_cast<DX12::Image*>(imageView->getImage());

			if (image->barrierLayout == D3D12_BARRIER_LAYOUT_RENDER_TARGET || image->barrierLayout == D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE) {
				attachmentIndex += 1;

				continue;
			}

			D3D12_TEXTURE_BARRIER textureBarrier{};
			textureBarrier.SyncBefore = D3D12_BARRIER_SYNC_ALL;
			textureBarrier.SyncAfter = D3D12_BARRIER_SYNC_ALL;
			textureBarrier.AccessBefore = image->barrierAccess;
			textureBarrier.LayoutBefore = image->barrierLayout;

			if (isDepthFormat(image->getFormat())) {
				textureBarrier.AccessAfter = D3D12_BARRIER_ACCESS_DEPTH_STENCIL_WRITE;
				textureBarrier.LayoutAfter = D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE;

				image->layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}
			else {
				textureBarrier.AccessAfter = D3D12_BARRIER_ACCESS_RENDER_TARGET;
				textureBarrier.LayoutAfter = D3D12_BARRIER_LAYOUT_RENDER_TARGET;

				image->layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
			}

			textureBarrier.pResource = image->resource;
			textureBarrier.Subresources.IndexOrFirstMipLevel = imageView->getMipLevelOffset();
			textureBarrier.Subresources.NumMipLevels = imageView->getMipLevelCount();
			textureBarrier.Subresources.FirstArraySlice = imageView->getLayerOffset();
			textureBarrier.Subresources.NumArraySlices = imageView->getLayerCount();
			textureBarrier.Subresources.FirstPlane = 0;
			textureBarrier.Subresources.NumPlanes = 1;
			textureBarrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;

			image->barrierAccess = textureBarrier.AccessAfter;
			image->barrierLayout = textureBarrier.LayoutAfter;

			textureBarriers.emplace_back(textureBarrier);

			attachmentIndex += 1;
		}

		if (textureBarriers.size() > 0) {
			D3D12_BARRIER_GROUP barrierGroup{};
			barrierGroup.Type = D3D12_BARRIER_TYPE_TEXTURE;
			barrierGroup.NumBarriers = static_cast<UINT>(textureBarriers.size());
			barrierGroup.pTextureBarriers = &textureBarriers[0];

			_commandEncoder->commandList->Barrier(1, &barrierGroup);
		}

		attachmentIndex = 0;

		while (attachmentIndex < renderPass->getAttachmentCount()) {
			const Attachment& attachment = renderPass->getAttachment(attachmentIndex);

			if (attachment.loadOperation != LoadOperation::CLEAR) {
				attachmentIndex += 1;

				continue;
			}

			const ref<DX12::ImageView> imageView = std::static_pointer_cast<DX12::ImageView>(_commandEncoder->framebuffer->imageViews[attachmentIndex]);
			const ClearValue& clearValue = descriptor.clearValues[attachmentIndex];

			if (isDepthFormat(imageView->getImage()->getFormat())) {
				_commandEncoder->commandList->ClearDepthStencilView(imageView->descriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, clearValue.depth, static_cast<UINT8>(clearValue.stencil), 1, &rectangle);
			}
			else {
				_commandEncoder->commandList->ClearRenderTargetView(imageView->descriptorHandle, clearValue.color, 1, &rectangle);
			}

			attachmentIndex += 1;
		}

		setRenderTargets(_commandEncoder);

		_commandEncoder->commandCount += 1;
	}

	void DX12::bindComputePipeline(Trident::CommandEncoder* commandEncoder, ref<Trident::ComputePipeline> computePipeline) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);
		const ref<DX12::ComputePipeline> pipeline = std::static_pointer_cast<DX12::ComputePipeline>(computePipeline);

		_commandEncoder->commandList->SetPipelineState(pipeline->pipelineState);
		_commandEncoder->commandList->SetComputeRootSignature(pipeline->pipelineLayout->rootSignature);

		_commandEncoder->computePipelineBound = true;
		_commandEncoder->commandCount += 1;
	}

	void DX12::bindDescriptorSets(Trident::CommandEncoder* commandEncoder, ref<Trident::PipelineLayout> pipelineLayout, uint32 descriptorSetIndexOffset, view<ref<Trident::DescriptorSet>> descriptorSets) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Device* device = static_cast<const DX12::Device*>(commandEncoder->device);
		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);
		const ref<DX12::MemoryAllocator> memoryAllocator = std::static_pointer_cast<DX12::MemoryAllocator>(device->getMemoryAllocator());

		ID3D12DescriptorHeap* viewDescriptorHeap = memoryAllocator->getCurrentViewDescriptorHeapAllocation().descriptorHeap;
		ID3D12DescriptorHeap* samplerDescriptorHeap = memoryAllocator->getCurrentSamplerDescriptorHeapAllocation().descriptorHeap;
		ID3D12DescriptorHeap* descriptorHeaps[2] = {viewDescriptorHeap, samplerDescriptorHeap};

		_commandEncoder->commandList->SetDescriptorHeaps(2, &descriptorHeaps[0]);

		UINT rootParameterIndex = 0;

		if (_commandEncoder->computePipelineBound) {
			while (rootParameterIndex < descriptorSets.count()) {
				const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(descriptorSets[rootParameterIndex]);

				D3D12_GPU_DESCRIPTOR_HANDLE descriptorOffset = viewDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				descriptorOffset.ptr += descriptorSet->viewDescriptorHandleOffset * memoryAllocator->viewDescriptorHandleIncrementSize;

				_commandEncoder->commandList->SetComputeRootDescriptorTable(descriptorSetIndexOffset + rootParameterIndex, descriptorOffset);

				rootParameterIndex += 1;
			}

			if (_commandEncoder->samplerDescriptorTableRootParameterIndex != max<UINT>()) {
				UINT64 samplerDescriptorHandleOffset = 0;

				rootParameterIndex = 0;

				while (rootParameterIndex < descriptorSets.count()) {
					const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(descriptorSets[rootParameterIndex]);

					samplerDescriptorHandleOffset = max(samplerDescriptorHandleOffset, descriptorSet->samplerDescriptorHandleOffset);
					rootParameterIndex += 1;
				}

				D3D12_GPU_DESCRIPTOR_HANDLE descriptorOffset = samplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				descriptorOffset.ptr += samplerDescriptorHandleOffset * memoryAllocator->samplerDescriptorHandleIncrementSize;

				_commandEncoder->commandList->SetComputeRootDescriptorTable(_commandEncoder->samplerDescriptorTableRootParameterIndex, descriptorOffset);
			}
		}
		else {
			while (rootParameterIndex < descriptorSets.count()) {
				const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(descriptorSets[rootParameterIndex]);

				D3D12_GPU_DESCRIPTOR_HANDLE descriptorOffset = viewDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				descriptorOffset.ptr += descriptorSet->viewDescriptorHandleOffset * memoryAllocator->viewDescriptorHandleIncrementSize;

				_commandEncoder->commandList->SetGraphicsRootDescriptorTable(descriptorSetIndexOffset + rootParameterIndex, descriptorOffset);

				rootParameterIndex += 1;
			}

			if (_commandEncoder->samplerDescriptorTableRootParameterIndex != max<UINT>()) {
				UINT64 samplerDescriptorHandleOffset = 0;

				rootParameterIndex = 0;

				while (rootParameterIndex < descriptorSets.count()) {
					const ref<DX12::DescriptorSet> descriptorSet = std::static_pointer_cast<DX12::DescriptorSet>(descriptorSets[rootParameterIndex]);

					samplerDescriptorHandleOffset = max(samplerDescriptorHandleOffset, descriptorSet->samplerDescriptorHandleOffset);
					rootParameterIndex += 1;
				}

				D3D12_GPU_DESCRIPTOR_HANDLE descriptorOffset = samplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				descriptorOffset.ptr += samplerDescriptorHandleOffset * memoryAllocator->samplerDescriptorHandleIncrementSize;

				_commandEncoder->commandList->SetGraphicsRootDescriptorTable(_commandEncoder->samplerDescriptorTableRootParameterIndex, descriptorOffset);
			}
		}

		_commandEncoder->commandCount += 1;
	}

	void DX12::bindGraphicsPipeline(Trident::CommandEncoder* commandEncoder, ref<Trident::GraphicsPipeline> graphicsPipeline) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);
		const ref<DX12::GraphicsPipeline> pipeline = std::static_pointer_cast<DX12::GraphicsPipeline>(graphicsPipeline);

		_commandEncoder->commandList->SetPipelineState(pipeline->pipelineState);
		_commandEncoder->commandList->SetGraphicsRootSignature(pipeline->pipelineLayout->rootSignature);
		_commandEncoder->commandList->IASetPrimitiveTopology(pipeline->primitiveTopology);

		_commandEncoder->samplerDescriptorTableRootParameterIndex = pipeline->pipelineLayout->samplerDescriptorTableRootParameterIndex;

		_commandEncoder->computePipelineBound = false;
		_commandEncoder->commandCount += 1;
	}

	void DX12::bindIndexBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::BaseBuffer> indexBuffer, uint64 offset, uint8 indexTypeSize) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ADAPTER_ASSERT(offset < indexBuffer->getSize(), "The index buffer offset is overflowing.");

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);

		D3D12_INDEX_BUFFER_VIEW indexBufferView{};
		indexBufferView.BufferLocation = std::static_pointer_cast<DX12::BaseBuffer>(indexBuffer)->resource->GetGPUVirtualAddress() + offset;
		indexBufferView.SizeInBytes = static_cast<UINT>(indexBuffer->getSize() - offset);
		indexBufferView.Format = getIndexBufferFormat(indexTypeSize);

		_commandEncoder->commandList->IASetIndexBuffer(&indexBufferView);

		_commandEncoder->commandCount += 1;
	}

	void DX12::bindVertexBuffer(Trident::CommandEncoder* commandEncoder, uint32 bindingIndex, ref<Trident::BaseBuffer> vertexBuffer, uint64 offset, uint32 stride) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ADAPTER_ASSERT(offset < vertexBuffer->getSize(), "The vertex buffer offset is overflowing.");

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		vertexBufferView.BufferLocation = std::static_pointer_cast<DX12::BaseBuffer>(vertexBuffer)->resource->GetGPUVirtualAddress() + offset;
		vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBuffer->getSize() - offset);
		vertexBufferView.StrideInBytes = stride;

		_commandEncoder->commandList->IASetVertexBuffers(bindingIndex, 1, &vertexBufferView);

		_commandEncoder->commandCount += 1;
	}

	void DX12::copyBufferToBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::BaseBuffer> sourceBuffer, ref<Trident::BaseBuffer> destinationBuffer, view<BufferCopyRegion> copyRegions) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);
		const ref<DX12::BaseBuffer> _destinationBuffer = std::static_pointer_cast<DX12::BaseBuffer>(destinationBuffer);

		// NOTE
		// D3D12_HEAP_TYPE_UPLOAD heaps only allow CPU writes.
		// The source and destination buffers are mapped to emulate a GPU copy command.
		if (_destinationBuffer->heapType == D3D12_HEAP_TYPE_UPLOAD) {
			const uint8* sourceMappedRange = static_cast<const uint8*>(std::static_pointer_cast<Buffer<uint8>>(sourceBuffer)->map(0, sourceBuffer->getSize()));
			uint8* destinationMappedRange = static_cast<uint8*>(std::static_pointer_cast<Buffer<uint8>>(destinationBuffer)->map(0, destinationBuffer->getSize()));

			for (const BufferCopyRegion& copyRegion : copyRegions) {
				std::copy_n(sourceMappedRange + copyRegion.sourceOffset, copyRegion.size, destinationMappedRange + copyRegion.destinationOffset);
			}

			sourceBuffer->unmap();
			destinationBuffer->unmap();

			_commandEncoder->commandCount += 1;

			return;
		}

		const ref<DX12::BaseBuffer> _sourceBuffer = std::static_pointer_cast<DX12::BaseBuffer>(sourceBuffer);

		for (const BufferCopyRegion& copyRegion : copyRegions) {
			_commandEncoder->commandList->CopyBufferRegion(_destinationBuffer->resource, copyRegion.destinationOffset, _sourceBuffer->resource, copyRegion.sourceOffset, copyRegion.size);
		}

		_commandEncoder->commandCount += 1;
	}

	void DX12::copyBufferToImage(Trident::CommandEncoder* commandEncoder, ref<Trident::BaseBuffer> sourceBuffer, ref<Trident::Image> destinationImage, view<BufferImageCopyRegion> copyRegions) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const ref<DX12::BaseBuffer> srcBuffer = std::static_pointer_cast<DX12::BaseBuffer>(sourceBuffer);
		const ref<DX12::Image> dstImage = std::static_pointer_cast<DX12::Image>(destinationImage);
		const D3D12_RESOURCE_DESC dstImageResourceDescriptor = dstImage->resource->GetDesc();
		const UINT channelCount = getChannelCount(dstImage->getFormat());

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);

		D3D12_TEXTURE_COPY_LOCATION sourceBufferCopyLocation{};
		sourceBufferCopyLocation.pResource = srcBuffer->resource;
		sourceBufferCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		sourceBufferCopyLocation.PlacedFootprint.Footprint.Format = dstImageResourceDescriptor.Format;

		D3D12_TEXTURE_COPY_LOCATION destinationTextureCopyLocation{};
		destinationTextureCopyLocation.pResource = dstImage->resource;
		destinationTextureCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

		for (const BufferImageCopyRegion& copyRegion : copyRegions) {
			const UINT64 layerSize = copyRegion.imageExtent.width * copyRegion.imageExtent.height * copyRegion.imageExtent.depth * channelCount;

			sourceBufferCopyLocation.PlacedFootprint.Footprint.Width = copyRegion.imageExtent.width;
			sourceBufferCopyLocation.PlacedFootprint.Footprint.Height = copyRegion.imageExtent.height;
			sourceBufferCopyLocation.PlacedFootprint.Footprint.Depth = copyRegion.imageExtent.depth;
			sourceBufferCopyLocation.PlacedFootprint.Footprint.RowPitch = copyRegion.imageExtent.width * channelCount;

			UINT layerIndex = 0;

			while (layerIndex < copyRegion.imageLayerCount) {
				sourceBufferCopyLocation.PlacedFootprint.Offset = copyRegion.bufferOffset + layerSize * layerIndex;

				destinationTextureCopyLocation.SubresourceIndex = copyRegion.imageLayerOffset + layerIndex;

				_commandEncoder->commandList->CopyTextureRegion(&destinationTextureCopyLocation, copyRegion.imageOffsetX, copyRegion.imageOffsetY, copyRegion.imageOffsetZ, &sourceBufferCopyLocation, nullptr);

				layerIndex += 1;
			}
		}

		_commandEncoder->commandCount += 1;
	}

	void DX12::copyImageToBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::Image> sourceImage, ref<Trident::BaseBuffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const ref<DX12::Image> srcImage = std::static_pointer_cast<DX12::Image>(sourceImage);
		const ref<DX12::BaseBuffer> dstBuffer = std::static_pointer_cast<DX12::BaseBuffer>(destinationBuffer);
		const D3D12_RESOURCE_DESC srcImageResourceDescriptor = srcImage->resource->GetDesc();
		const UINT channelCount = getChannelCount(srcImage->getFormat());

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);

		D3D12_TEXTURE_COPY_LOCATION sourceTextureCopyLocation{};
		sourceTextureCopyLocation.pResource = srcImage->resource;
		sourceTextureCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

		D3D12_TEXTURE_COPY_LOCATION destinationBufferCopyLocation{};
		destinationBufferCopyLocation.pResource = dstBuffer->resource;
		destinationBufferCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		destinationBufferCopyLocation.PlacedFootprint.Footprint.Format = srcImageResourceDescriptor.Format;

		for (const BufferImageCopyRegion& copyRegion : copyRegions) {
			const UINT64 layerSize = copyRegion.imageExtent.width * copyRegion.imageExtent.height * copyRegion.imageExtent.depth * channelCount;

			destinationBufferCopyLocation.PlacedFootprint.Footprint.Width = copyRegion.imageExtent.width;
			destinationBufferCopyLocation.PlacedFootprint.Footprint.Height = copyRegion.imageExtent.height;
			destinationBufferCopyLocation.PlacedFootprint.Footprint.Depth = copyRegion.imageExtent.depth;
			destinationBufferCopyLocation.PlacedFootprint.Footprint.RowPitch = copyRegion.imageExtent.width * channelCount;

			UINT layerIndex = 0;

			while (layerIndex < copyRegion.imageLayerCount) {
				sourceTextureCopyLocation.SubresourceIndex = copyRegion.imageLayerOffset + layerIndex;

				destinationBufferCopyLocation.PlacedFootprint.Offset = copyRegion.bufferOffset + layerSize * layerIndex;

				D3D12_BOX sourceBox{};
				sourceBox.left = copyRegion.imageOffsetX;
				sourceBox.top = copyRegion.imageOffsetY;
				sourceBox.front = copyRegion.imageOffsetZ;
				sourceBox.right = copyRegion.imageOffsetX + copyRegion.imageExtent.width;
				sourceBox.bottom = copyRegion.imageOffsetY + copyRegion.imageExtent.height;
				sourceBox.back = copyRegion.imageOffsetZ + copyRegion.imageExtent.depth;

				_commandEncoder->commandList->CopyTextureRegion(&destinationBufferCopyLocation, 0, 0, 0, &sourceTextureCopyLocation, &sourceBox);

				layerIndex += 1;
			}
		}

		_commandEncoder->commandCount += 1;
	}

	ref<Trident::CommandEncoder> DX12::createCommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool, const CommandBufferDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Device* _device = static_cast<const DX12::Device*>(device);
		const DX12::CommandPool* _commandPool = static_cast<const DX12::CommandPool*>(commandPool);
		HRESULT result;

		ref<DX12::CommandEncoder> commandEncoder = createRef<DX12::CommandEncoder>(device, commandPool);
		commandEncoder->commandAllocator = _commandPool->commandAllocator;
		commandEncoder->usage = descriptor.usage;
		commandEncoder->computeCommandList = _commandPool->commandListType == D3D12_COMMAND_LIST_TYPE_COMPUTE;

		result = _device->device->CreateCommandList(0, _commandPool->commandListType, _commandPool->commandAllocator, nullptr, IID_PPV_ARGS(&commandEncoder->commandList));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command list.");

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				const LPWSTR name = ConvertUTF8ToUTF16(&descriptor.name[0]);

				result = commandEncoder->commandList->SetName(name);

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to set command list name.");

				free(name);
			}
		#endif

		// Only 1 open command encoder at a time.
		if (descriptor.usage == CommandBufferUsage::AUTO) {
			commandEncoder->end();
		}

		return commandEncoder;
	}

	void DX12::destroyCommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool, const Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::CommandEncoder* _commandEncoder = static_cast<const DX12::CommandEncoder*>(commandEncoder);

		_commandEncoder->commandList->Release();
	}

	void DX12::dispatch(Trident::CommandEncoder* commandEncoder, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);

		_commandEncoder->commandList->Dispatch(groupCountX, groupCountY, groupCountZ);

		_commandEncoder->commandCount += 1;
	}

	void DX12::dispatchIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::BaseBuffer> buffer, uint64 bufferOffset) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ADAPTER_ASSERT(buffer->getUsage() & BufferUsage::INDIRECT, "The buffer is not an indirect buffer.");

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);
		const DX12::Device* _device = static_cast<const DX12::Device*>(commandEncoder->device);
		const ref<DX12::BaseBuffer> indirectBuffer = std::static_pointer_cast<DX12::BaseBuffer>(buffer);

		// TODO: Create command signature when creating the buffer.
		if (!indirectBuffer->commandSignature) {
			HRESULT result;

			D3D12_INDIRECT_ARGUMENT_DESC indirectArgumentDescriptor{};
			indirectArgumentDescriptor.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;

			D3D12_COMMAND_SIGNATURE_DESC commandSignatureDescriptor{};
			commandSignatureDescriptor.ByteStride = sizeof(DispatchIndirectCommand);
			commandSignatureDescriptor.NumArgumentDescs = 1;
			commandSignatureDescriptor.pArgumentDescs = &indirectArgumentDescriptor;
			commandSignatureDescriptor.NodeMask = 0;

			result = _device->device->CreateCommandSignature(&commandSignatureDescriptor, nullptr, IID_PPV_ARGS(&indirectBuffer->commandSignature));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command signature.");
		}

		_commandEncoder->commandList->ExecuteIndirect(
			indirectBuffer->commandSignature,
			1,
			indirectBuffer->resource,
			bufferOffset,
			nullptr,
			0
		);

		_commandEncoder->commandCount += 1;
	}

	void DX12::draw(Trident::CommandEncoder* commandEncoder, uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);

		_commandEncoder->commandList->DrawInstanced(vertexCount, instanceCount, vertexOffset, instanceOffset);

		_commandEncoder->commandCount += 1;
	}

	void DX12::drawIndexed(Trident::CommandEncoder* commandEncoder, uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);

		_commandEncoder->commandList->DrawIndexedInstanced(indexCount, instanceCount, indexOffset, indexBias, instanceOffset);

		_commandEncoder->commandCount += 1;
	}

	void DX12::drawIndexedIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::BaseBuffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ADAPTER_ASSERT(buffer->getUsage() & BufferUsage::INDIRECT, "The buffer is not an indirect buffer.");

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);
		const DX12::Device* _device = static_cast<const DX12::Device*>(commandEncoder->device);
		const ref<DX12::BaseBuffer> indirectBuffer = std::static_pointer_cast<DX12::BaseBuffer>(buffer);

		// TODO: Create command signature when creating the buffer.
		if (!indirectBuffer->commandSignature) {
			D3D12_INDIRECT_ARGUMENT_DESC indirectArgumentDescriptor{};
			indirectArgumentDescriptor.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

			D3D12_COMMAND_SIGNATURE_DESC commandSignatureDescriptor{};
			commandSignatureDescriptor.ByteStride = sizeof(DrawIndexedIndirectCommand);
			commandSignatureDescriptor.NumArgumentDescs = drawCount;
			commandSignatureDescriptor.pArgumentDescs = &indirectArgumentDescriptor;
			commandSignatureDescriptor.NodeMask = 0;

			const HRESULT result = _device->device->CreateCommandSignature(&commandSignatureDescriptor, nullptr, IID_PPV_ARGS(&indirectBuffer->commandSignature));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command signature.");
		}

		_commandEncoder->commandList->ExecuteIndirect(
			indirectBuffer->commandSignature,
			drawCount,
			indirectBuffer->resource,
			bufferOffset,
			nullptr,
			0
		);

		_commandEncoder->commandCount += 1;
	}

	void DX12::drawIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::BaseBuffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ADAPTER_ASSERT(buffer->getUsage() & BufferUsage::INDIRECT, "The buffer is not an indirect buffer.");

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);
		const DX12::Device* _device = static_cast<const DX12::Device*>(commandEncoder->device);
		const ref<DX12::BaseBuffer> indirectBuffer = std::static_pointer_cast<DX12::BaseBuffer>(buffer);

		// TODO: Create command signature when creating the buffer.
		if (!indirectBuffer->commandSignature) {
			D3D12_INDIRECT_ARGUMENT_DESC indirectArgumentDescriptor{};
			indirectArgumentDescriptor.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

			D3D12_COMMAND_SIGNATURE_DESC commandSignatureDescriptor{};
			commandSignatureDescriptor.ByteStride = sizeof(DrawIndirectCommand);
			commandSignatureDescriptor.NumArgumentDescs = drawCount;
			commandSignatureDescriptor.pArgumentDescs = &indirectArgumentDescriptor;
			commandSignatureDescriptor.NodeMask = 0;

			const HRESULT result = _device->device->CreateCommandSignature(&commandSignatureDescriptor, nullptr, IID_PPV_ARGS(&indirectBuffer->commandSignature));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create command signature.");
		}

		_commandEncoder->commandList->ExecuteIndirect(
			indirectBuffer->commandSignature,
			drawCount,
			indirectBuffer->resource,
			bufferOffset,
			nullptr,
			0
		);

		_commandEncoder->commandCount += 1;
	}

	ref<Trident::CommandBuffer> DX12::end(const Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::CommandEncoder* _commandEncoder = static_cast<const DX12::CommandEncoder*>(commandEncoder);
		HRESULT result;

		result = _commandEncoder->commandList->Close();

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to close command list.");

		return createRef<DX12::CommandBuffer>(_commandEncoder->commandList, _commandEncoder->commandCount);
	}

	void DX12::endRenderPass(Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);

		if (_commandEncoder->presentRenderTargetIndex < _commandEncoder->framebuffer->imageViews.count()) {
			ref<DX12::ImageView> presentImageView = std::static_pointer_cast<DX12::ImageView>(_commandEncoder->framebuffer->imageViews[_commandEncoder->presentRenderTargetIndex]);
			DX12::Image* presentImage = static_cast<DX12::Image*>(presentImageView->getImage());

			if (presentImage->getSampleCount() > 1) {
				const ref<Subpass> subpass = _commandEncoder->framebuffer->getRenderPass()->getSubpass(_commandEncoder->subpassIndex);
				const ref<DX12::ImageView> resolveImageView = presentImageView;

				DX12::Image* resolveImage = presentImage;

				presentImageView = std::static_pointer_cast<DX12::ImageView>(_commandEncoder->framebuffer->imageViews[subpass->getResolveAttachmentReferenceCount() + (subpass->hasDepthStencilAttachmentReference() ? 1 : 0) + _commandEncoder->presentRenderTargetIndex]);
				presentImage = static_cast<DX12::Image*>(presentImageView->getImage());

				D3D12_TEXTURE_BARRIER textureBarriers[2];

				D3D12_TEXTURE_BARRIER& resolveTextureBarrier = textureBarriers[0];
				resolveTextureBarrier.SyncBefore = D3D12_BARRIER_SYNC_ALL;
				resolveTextureBarrier.SyncAfter = D3D12_BARRIER_SYNC_ALL;
				resolveTextureBarrier.AccessBefore = resolveImage->barrierAccess;
				resolveTextureBarrier.AccessAfter = D3D12_BARRIER_ACCESS_RESOLVE_SOURCE;
				resolveTextureBarrier.LayoutBefore = resolveImage->barrierLayout;
				resolveTextureBarrier.LayoutAfter = D3D12_BARRIER_LAYOUT_RESOLVE_SOURCE;
				resolveTextureBarrier.pResource = resolveImage->resource;
				resolveTextureBarrier.Subresources.IndexOrFirstMipLevel = resolveImageView->getMipLevelOffset();
				resolveTextureBarrier.Subresources.NumMipLevels = resolveImageView->getMipLevelCount();
				resolveTextureBarrier.Subresources.FirstArraySlice = resolveImageView->getLayerOffset();
				resolveTextureBarrier.Subresources.NumArraySlices = resolveImageView->getLayerCount();
				resolveTextureBarrier.Subresources.FirstPlane = 0;
				resolveTextureBarrier.Subresources.NumPlanes = 1;
				resolveTextureBarrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;

				// resolveImage->resourceState = resolveTextureBarrier.Transition.StateAfter;
				resolveImage->barrierAccess = resolveTextureBarrier.AccessAfter;
				resolveImage->barrierLayout = resolveTextureBarrier.LayoutAfter;

				D3D12_TEXTURE_BARRIER& presentTextureBarrier = textureBarriers[1];
				presentTextureBarrier.SyncBefore = D3D12_BARRIER_SYNC_ALL;
				presentTextureBarrier.SyncAfter = D3D12_BARRIER_SYNC_ALL;
				presentTextureBarrier.AccessBefore = presentImage->barrierAccess;
				presentTextureBarrier.AccessAfter = D3D12_BARRIER_ACCESS_RESOLVE_DEST;
				presentTextureBarrier.LayoutBefore = presentImage->barrierLayout;
				presentTextureBarrier.LayoutAfter = D3D12_BARRIER_LAYOUT_RESOLVE_DEST;
				presentTextureBarrier.pResource = presentImage->resource;
				presentTextureBarrier.Subresources.IndexOrFirstMipLevel = presentImageView->getMipLevelOffset();
				presentTextureBarrier.Subresources.NumMipLevels = presentImageView->getMipLevelCount();
				presentTextureBarrier.Subresources.FirstArraySlice = presentImageView->getLayerOffset();
				presentTextureBarrier.Subresources.NumArraySlices = presentImageView->getLayerCount();
				presentTextureBarrier.Subresources.FirstPlane = 0;
				presentTextureBarrier.Subresources.NumPlanes = 1;
				presentTextureBarrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE; 

				// presentImage->resourceState = presentTextureBarrier.Transition.StateAfter;
				presentImage->barrierAccess = presentTextureBarrier.AccessAfter;
				presentImage->barrierLayout = presentTextureBarrier.LayoutAfter;

				D3D12_BARRIER_GROUP barrierGroup{};
				barrierGroup.Type = D3D12_BARRIER_TYPE_TEXTURE;
				barrierGroup.NumBarriers = 2;
				barrierGroup.pTextureBarriers = &textureBarriers[0];

				_commandEncoder->commandList->Barrier(1, &barrierGroup);

				_commandEncoder->commandList->ResolveSubresource(
					presentImage->resource,
					presentImageView->getLayerOffset(),
					resolveImage->resource,
					resolveImageView->getLayerOffset(),
					getImageFormat(presentImage->getFormat())
				);
			}

			D3D12_TEXTURE_BARRIER textureBarrier{};
			textureBarrier.SyncBefore = D3D12_BARRIER_SYNC_ALL;
			textureBarrier.SyncAfter = D3D12_BARRIER_SYNC_ALL;
			textureBarrier.AccessBefore = presentImage->barrierAccess;
			textureBarrier.AccessAfter = D3D12_BARRIER_ACCESS_COMMON;
			textureBarrier.LayoutBefore = presentImage->barrierLayout;
			textureBarrier.LayoutAfter = D3D12_BARRIER_LAYOUT_PRESENT;
			textureBarrier.pResource = presentImage->resource;
			textureBarrier.Subresources.IndexOrFirstMipLevel = presentImageView->getMipLevelOffset();
			textureBarrier.Subresources.NumMipLevels = presentImageView->getMipLevelCount();
			textureBarrier.Subresources.FirstArraySlice = presentImageView->getLayerOffset();
			textureBarrier.Subresources.NumArraySlices = presentImageView->getLayerCount();
			textureBarrier.Subresources.FirstPlane = 0;
			textureBarrier.Subresources.NumPlanes = 1;
			textureBarrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE; 

			presentImage->barrierAccess = textureBarrier.AccessAfter;
			presentImage->barrierLayout = textureBarrier.LayoutAfter;

			presentImage->layout = ImageLayout::PRESENT_SRC;

			D3D12_BARRIER_GROUP barrierGroup{};
			barrierGroup.Type = D3D12_BARRIER_TYPE_TEXTURE;
			barrierGroup.NumBarriers = 1;
			barrierGroup.pTextureBarriers = &textureBarrier;

			_commandEncoder->commandList->Barrier(1, &barrierGroup);
		}

		_commandEncoder->commandCount += 1;
	}

	void DX12::insertMemoryBarriers(Trident::CommandEncoder* commandEncoder, view<BufferMemoryBarrier> bufferMemoryBarriers, view<ImageMemoryBarrier> imageMemoryBarriers) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);
		sequence<D3D12_BARRIER_GROUP> barrierGroups(2);
		sequence<D3D12_BUFFER_BARRIER> bufferBarriers(bufferMemoryBarriers.count());
		sequence<D3D12_TEXTURE_BARRIER> textureBarriers(imageMemoryBarriers.count());
		UINT bufferBarrierIndex = 0;
		index imageMemoryBarrierIndex = 0;

		// TODO: Fix driver warning:
		// "ExecuteCommandLists references command lists that have recorded only Barrier commands. Since there is no other GPU work to synchronize against,
		// all barriers should use AccessAfter / AccessBefore = D3D12_BARRIER_ACCESS_NO_ACCESS and SyncBefore / SyncAfter = D3D12_BARRIER_SYNC_NONE.
		// This information can be used as an optimization hint by some drivers."

		while (bufferBarrierIndex < bufferMemoryBarriers.count()) {
			const BufferMemoryBarrier& bufferMemoryBarrier = bufferMemoryBarriers[bufferBarrierIndex];

			D3D12_BUFFER_BARRIER& bufferBarrier = bufferBarriers[bufferBarrierIndex];
			bufferBarrier.SyncBefore = D3D12_BARRIER_SYNC_ALL;
			bufferBarrier.SyncAfter = D3D12_BARRIER_SYNC_ALL;
			bufferBarrier.AccessBefore = getBufferBarrierAccess(bufferMemoryBarrier.buffer->access);
			bufferBarrier.AccessAfter = getBufferBarrierAccess(bufferMemoryBarrier.newAccess);
			bufferBarrier.pResource = std::static_pointer_cast<DX12::BaseBuffer>(bufferMemoryBarrier.buffer)->resource;
			bufferBarrier.Offset = bufferMemoryBarrier.offset;
			bufferBarrier.Size = bufferMemoryBarrier.size;

			bufferMemoryBarrier.buffer->access = bufferMemoryBarrier.newAccess;

			bufferBarrierIndex += 1;
		}

		D3D12_BARRIER_GROUP& bufferBarrierGroup = barrierGroups[0];
		bufferBarrierGroup.Type = D3D12_BARRIER_TYPE_BUFFER;
		bufferBarrierGroup.NumBarriers = static_cast<UINT>(bufferBarriers.count());
		bufferBarrierGroup.pBufferBarriers = &bufferBarriers[0];

		while (imageMemoryBarrierIndex < imageMemoryBarriers.count()) {
			const ImageMemoryBarrier& imageMemoryBarrier = imageMemoryBarriers[imageMemoryBarrierIndex];
			const ref<DX12::Image> dxImage = std::static_pointer_cast<DX12::Image>(imageMemoryBarrier.image);

			D3D12_TEXTURE_BARRIER& textureBarrier = textureBarriers[imageMemoryBarrierIndex];
			textureBarrier.SyncBefore = D3D12_BARRIER_SYNC_ALL;
			textureBarrier.SyncAfter = D3D12_BARRIER_SYNC_ALL;
			textureBarrier.AccessBefore = dxImage->barrierAccess;
			textureBarrier.AccessAfter = getImageBarrierAccess(imageMemoryBarrier.newAccess, imageMemoryBarrier.newLayout);
			textureBarrier.LayoutBefore = dxImage->barrierLayout;
			textureBarrier.LayoutAfter = getBarrierLayout(imageMemoryBarrier.newLayout, imageMemoryBarrier.newAccess);
			textureBarrier.pResource = std::static_pointer_cast<DX12::Image>(imageMemoryBarrier.image)->resource;
			textureBarrier.Subresources.IndexOrFirstMipLevel = 0;
			textureBarrier.Subresources.NumMipLevels = imageMemoryBarrier.image->getMipLevelCount();
			textureBarrier.Subresources.FirstArraySlice = 0;
			textureBarrier.Subresources.NumArraySlices = imageMemoryBarrier.image->getLayerCount();
			textureBarrier.Subresources.FirstPlane = 0;
			textureBarrier.Subresources.NumPlanes = 1;
			textureBarrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;

			dxImage->barrierAccess = textureBarrier.AccessAfter;
			dxImage->barrierLayout = textureBarrier.LayoutAfter;

			imageMemoryBarrier.image->layout = imageMemoryBarrier.newLayout;
			imageMemoryBarrier.image->access = imageMemoryBarrier.newAccess;

			imageMemoryBarrierIndex += 1;
		}

		D3D12_BARRIER_GROUP& textureBarrierGroup = barrierGroups[1];
		textureBarrierGroup.Type = D3D12_BARRIER_TYPE_TEXTURE;
		textureBarrierGroup.NumBarriers = static_cast<UINT>(textureBarriers.count());
		textureBarrierGroup.pTextureBarriers = &textureBarriers[0];

		_commandEncoder->commandList->Barrier(static_cast<UINT>(barrierGroups.count()), &barrierGroups[0]);
		_commandEncoder->commandCount += 1;
	}

	void DX12::nextSubpass(Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);
		_commandEncoder->subpassIndex += 1;

		setRenderTargets(_commandEncoder);

		_commandEncoder->commandCount += 1;
	}

	void DX12::reset(Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);

		if (_commandEncoder->commandCount == 0 && _commandEncoder->usage == CommandBufferUsage::ONE_TIME_SUBMIT) {
			return;
		}

		HRESULT result;

		result = _commandEncoder->commandAllocator->Reset();

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to reset command allocator.");

		result = _commandEncoder->commandList->Reset(_commandEncoder->commandAllocator, nullptr);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to reset command list.");

		_commandEncoder->commandCount = 0;
		_commandEncoder->computePipelineBound = false;
		_commandEncoder->framebuffer = nullptr;
		_commandEncoder->subpassIndex = 0;
		_commandEncoder->presentRenderTargetIndex = max<UINT>();
		_commandEncoder->samplerDescriptorTableRootParameterIndex = max<UINT>();
	}

	void DX12::setLineWidth(Trident::CommandEncoder* commandEncoder, float32 lineWidth) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);
		_commandEncoder->commandCount += 1;
	}

	void DX12::setScissor(Trident::CommandEncoder* commandEncoder, sint32 xOffset, sint32 yOffset, Extent2D extent) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);

		D3D12_RECT rectangle{};
		rectangle.left = xOffset;
		rectangle.top = yOffset;
		rectangle.right = xOffset + extent.width;
		rectangle.bottom = yOffset + extent.height;

		_commandEncoder->commandList->RSSetScissorRects(1, &rectangle);
		_commandEncoder->commandCount += 1;
	}

	void DX12::setViewport(Trident::CommandEncoder* commandEncoder, vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DX12::CommandEncoder* _commandEncoder = static_cast<DX12::CommandEncoder*>(commandEncoder);

		D3D12_VIEWPORT viewport{};
		viewport.TopLeftX = origin[0];

		// TODO: Allow the origin's Y to be flipped similarly to Vulkan?
		viewport.TopLeftY = 0.0f;

		viewport.Width = abs(size[0]);
		viewport.Height = abs(size[1]);
		viewport.MinDepth = minDepth;
		viewport.MaxDepth = maxDepth;

		_commandEncoder->commandList->RSSetViewports(1, &viewport);
		_commandEncoder->commandCount += 1;
	}
}