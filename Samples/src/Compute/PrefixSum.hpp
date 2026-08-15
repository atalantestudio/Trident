// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class PrefixSumSampleRenderer : public Renderer {
		private:
			static constexpr index ELEMENT_COUNT = 16;
			static constexpr uint32 ELEMENT_COUNTS[ELEMENT_COUNT] = {2, 5, 6, 14, 4, 4, 5, 10, 7, 0, 11, 3, 8, 4, 6, 5};

		public:
			void build() override {
				const ref<Device> device = getDevice();

				const DescriptorSetLayoutBinding prefixSumDescriptorSetLayoutBindings[] = {
					// Element count
					{
						.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::COMPUTE,
					},

					// Element start
					{
						.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::COMPUTE,
					},
				};

				const ref<DescriptorSetLayout> prefixSumDescriptorSetLayout = createDescriptorSetLayout(prefixSumDescriptorSetLayoutBindings);

				const ref<PipelineLayout> prefixSumPipelineLayout = createPipelineLayout({
					.descriptorSetLayouts = {prefixSumDescriptorSetLayout},
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Prefix sum",
					#endif
				});

				const ref<ComputePipeline> prefixSumPipeline = createComputePipeline({
					.pipelineLayout = prefixSumPipelineLayout,
					.computeShaderModule = loadShaderModule(
						{
							.stage = ShaderStage::COMPUTE,
							.entryPoint = "main",
							#if defined(DEBUG_OBJECT_NAME)
								.name = "Prefix sum compute",
							#endif
						},
						"Compute/PrefixSum/PrefixSum.comp"
					),
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Prefix sum",
					#endif
				});

				const ref<Buffer> elementCountBuffer = createDynamicBuffer({
					.size = sizeof(uint32) * ELEMENT_COUNT,
					.usage = BufferUsage::READ_ONLY_STORAGE,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Element count",
					#endif
				});
				const ref<Buffer> elementStartBuffer = createDynamicBuffer({
					.size = elementCountBuffer->getSize(),
					.usage = BufferUsage::TRANSFER_SRC | BufferUsage::READ_WRITE_STORAGE,
					.memoryProperties = MemoryProperty::DEVICE_LOCAL,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Element start",
					#endif
				});
				const ref<Buffer> elementStartStagingBuffer = createDynamicBuffer({
					.size = elementStartBuffer->getSize(),
					.usage = BufferUsage::TRANSFER_DST,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Element start staging",
					#endif
				});

				const BufferView<uint32> elementCountView = elementCountBuffer->createView<uint32>(0, elementCountBuffer->getSize());
				const BufferView<uint32> elementStartView = elementStartBuffer->createView<uint32>(0, elementStartBuffer->getSize());
				const BufferView<uint32> elementStartStagingView = elementStartStagingBuffer->createView<uint32>(0, elementStartStagingBuffer->getSize());

				uint32* elementCountMappedRange = elementCountView.map();

				std::copy_n(ELEMENT_COUNTS, ELEMENT_COUNT, elementCountMappedRange);

				elementCountBuffer->unmap();

				const sequence<ref<DescriptorSet>> descriptorSets = device->allocateDescriptorSets(prefixSumDescriptorSetLayout);
				const ref<DescriptorSet> prefixSumDescriptorSet = descriptorSets[0];

				const WriteDescriptorSet writeDescriptorSets[] = {
					{
						.descriptorSet = prefixSumDescriptorSet,
						.bindingIndex = 0,
						.descriptorBufferViews = elementCountView,
					},
					{
						.descriptorSet = prefixSumDescriptorSet,
						.bindingIndex = 1,
						.descriptorBufferViews = elementStartView,
					},
				};

				device->updateDescriptorSets(writeDescriptorSets);

				const ref<Queue> queue = device->getQueue(QueueFamilyCapability::COMPUTE);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Prefix sum",
					#endif
				});

				commandEncoder->begin();
				commandEncoder->bindComputePipeline(prefixSumPipeline);
				commandEncoder->bindDescriptorSets(prefixSumPipelineLayout, 0, prefixSumDescriptorSet);
				commandEncoder->dispatch(1, 1, 1);

				const BufferMemoryBarrier elementStartMemoryBarrier = {
					.buffer = elementStartBuffer,
					.offset = elementStartView.getOffset(),
					.size = elementStartView.getSize(),
					.newAccess = Access::TRANSFER_READ,
				};

				commandEncoder->insertMemoryBarriers(elementStartMemoryBarrier, {});

				const BufferCopyRegion elementStartCopyRegion = {
					.sourceOffset = elementStartView.getOffset(),
					.destinationOffset = 0,
					.size = elementStartView.getSize(),
				};

				commandEncoder->copyBufferToBuffer(elementStartBuffer, elementStartStagingBuffer, elementStartCopyRegion);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();

				const uint32* elementStartMappedRange = elementStartStagingView.map();

				ENGINE_TRACE("Element count buffer:");

				index elementIndex = 0;

				while (elementIndex < ELEMENT_COUNT) {
					engineLogger << ELEMENT_COUNTS[elementIndex] << ' ';

					elementIndex += 1;
				}

				engineLogger << '\n';

				ENGINE_TRACE("Element start buffer:");

				elementIndex = 0;

				while (elementIndex < ELEMENT_COUNT) {
					engineLogger << elementStartMappedRange[elementIndex] << ' ';

					elementIndex += 1;
				}

				engineLogger << '\n';

				elementStartStagingBuffer->unmap();
			}

			void render(ref<CommandEncoder> commandEncoder) const override {}
	};

	class PrefixSumSampleProcess final : public Process {
		public:
			sint32 main(view<void*> arguments) override {
				renderer.build();

				return 0;
			}

		private:
			PrefixSumSampleRenderer renderer;
	};
}