// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class ComputePrefixSumRenderer : public Renderer {
		private:
			static constexpr index ELEMENT_COUNT = 16;
			static constexpr uint32 ELEMENT_COUNTS[ELEMENT_COUNT] = {
				2, 5, 6, 14, 4, 4, 5, 10, 7, 0, 11, 3, 8, 4, 6, 5,
			};

		public:
			void build() override {
				const ref<Device> device = getDevice();

				DescriptorSetLayoutBinding elementCountBinding{};
				elementCountBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
				elementCountBinding.descriptorCount = 1;
				elementCountBinding.shaderStages = ShaderStage::COMPUTE;

				DescriptorSetLayoutBinding elementStartBinding{};
				elementStartBinding.descriptorType = DescriptorType::READ_WRITE_STORAGE_BUFFER;
				elementStartBinding.descriptorCount = 1;
				elementStartBinding.shaderStages = ShaderStage::COMPUTE;

				const ref<DescriptorSetLayout> prefixSumDescriptorSetLayout = createDescriptorSetLayout({elementCountBinding, elementStartBinding});

				PipelineLayoutDescriptor prefixSumPipelineLayoutDescriptor{};
				prefixSumPipelineLayoutDescriptor.descriptorSetLayouts = {prefixSumDescriptorSetLayout};

				SET_DEBUG_NAME(prefixSumPipelineLayoutDescriptor, "Prefix sum");

				const ref<PipelineLayout> prefixSumPipelineLayout = createPipelineLayout(prefixSumPipelineLayoutDescriptor);

				ShaderModuleDescriptor prefixSumShaderModuleDescriptor{};
				prefixSumShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
				prefixSumShaderModuleDescriptor.entryPoint = "main";

				SET_DEBUG_NAME(prefixSumShaderModuleDescriptor, "Prefix sum compute");

				ComputePipelineDescriptor prefixSumPipelineDescriptor{};
				prefixSumPipelineDescriptor.pipelineLayout = prefixSumPipelineLayout;
				prefixSumPipelineDescriptor.computeShaderModule = loadShaderModule(prefixSumShaderModuleDescriptor, "Compute/PrefixSum/PrefixSum.comp");

				SET_DEBUG_NAME(prefixSumPipelineDescriptor, "Prefix sum");

				const ref<ComputePipeline> prefixSumPipeline = createComputePipeline(prefixSumPipelineDescriptor);

				BufferDescriptor elementCountBufferDescriptor{};
				elementCountBufferDescriptor.size = sizeof(uint32) * ELEMENT_COUNT;
				elementCountBufferDescriptor.usage = BufferUsage::READ_ONLY_STORAGE;
				elementCountBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
				elementCountBufferDescriptor.persistent = false;

				SET_DEBUG_NAME(elementCountBufferDescriptor, "Element count");

				const ref<Buffer2<uint32>> elementCountBuffer = createBuffer2<uint32>(elementCountBufferDescriptor);

				BufferDescriptor elementStartBufferDescriptor{};
				elementStartBufferDescriptor.size = sizeof(uint32) * ELEMENT_COUNT;
				elementStartBufferDescriptor.usage = BufferUsage::READ_WRITE_STORAGE | BufferUsage::TRANSFER_SRC;
				elementStartBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL;
				elementStartBufferDescriptor.persistent = false;

				SET_DEBUG_NAME(elementStartBufferDescriptor, "Element start");

				const ref<Buffer2<uint32>> elementStartBuffer = createBuffer2<uint32>(elementStartBufferDescriptor);

				BufferDescriptor elementStartStagingBufferDescriptor{};
				elementStartStagingBufferDescriptor.size = elementStartBuffer->getSize();
				elementStartStagingBufferDescriptor.usage = BufferUsage::TRANSFER_DST;
				elementStartStagingBufferDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::HOST_VISIBLE;
				elementStartStagingBufferDescriptor.persistent = false;

				SET_DEBUG_NAME(elementStartStagingBufferDescriptor, "Element start staging");

				const ref<Buffer2<uint32>> elementStartStagingBuffer = createBuffer2<uint32>(elementStartStagingBufferDescriptor);

				uint32* elementCountMappedRange = elementCountBuffer->map(0, elementCountBuffer->getSize());

				std::copy_n(ELEMENT_COUNTS, ELEMENT_COUNT, elementCountMappedRange);

				elementCountBuffer->unmap();

				const sequence<ref<DescriptorSet>> descriptorSets = device->allocateDescriptorSets({prefixSumDescriptorSetLayout});
				const ref<DescriptorSet> prefixSumDescriptorSet = descriptorSets[0];

				DescriptorBuffer elementCountDescriptorBuffer{};
				elementCountDescriptorBuffer.buffer = elementCountBuffer;
				elementCountDescriptorBuffer.offset = 0;
				elementCountDescriptorBuffer.size = elementCountBuffer->getSize();

				DescriptorBuffer elementStartDescriptorBuffer{};
				elementStartDescriptorBuffer.buffer = elementStartBuffer;
				elementStartDescriptorBuffer.offset = 0;
				elementStartDescriptorBuffer.size = elementStartBuffer->getSize();

				WriteDescriptorSet elementCountWriteDescriptorSet{};
				elementCountWriteDescriptorSet.descriptorSet = prefixSumDescriptorSet;
				elementCountWriteDescriptorSet.bindingIndex = 0;
				elementCountWriteDescriptorSet.descriptorBuffers = {elementCountDescriptorBuffer};

				WriteDescriptorSet elementStartWriteDescriptorSet{};
				elementStartWriteDescriptorSet.descriptorSet = prefixSumDescriptorSet;
				elementStartWriteDescriptorSet.bindingIndex = 1;
				elementStartWriteDescriptorSet.descriptorBuffers = {elementStartDescriptorBuffer};

				device->updateDescriptorSets({elementCountWriteDescriptorSet, elementStartWriteDescriptorSet});

				CommandBufferDescriptor commandBufferDescriptor{};
				commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

				SET_DEBUG_NAME(commandBufferDescriptor, "Run prefix sum");

				const ref<Queue> queue = device->getQueue(QueueFamilyCapability::COMPUTE);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder(commandBufferDescriptor);

				commandEncoder->begin();
				commandEncoder->bindComputePipeline(prefixSumPipeline);
				commandEncoder->bindDescriptorSets(prefixSumPipelineLayout, 0, {prefixSumDescriptorSet});
				commandEncoder->dispatch(1, 1, 1);

				// TODO
				BREAKPOINT();

				/* BufferMemoryBarrier elementStartMemoryBarrier{};
				elementStartMemoryBarrier.buffer = elementStartBuffer;
				elementStartMemoryBarrier.offset = 0;
				elementStartMemoryBarrier.size = elementStartBuffer->getSize();
				elementStartMemoryBarrier.sourceAccess = Access::SHADER_WRITE;
				elementStartMemoryBarrier.destinationAccess = Access::TRANSFER_READ;

				commandEncoder->insertBufferMemoryBarriers({elementStartMemoryBarrier});

				BufferCopyRegion elementStartCopyRegion{};
				elementStartCopyRegion.sourceOffset = 0;
				elementStartCopyRegion.destinationOffset = 0;
				elementStartCopyRegion.size = elementStartBuffer->getSize();

				commandEncoder->copyBufferToBuffer(elementStartBuffer, elementStartStagingBuffer, {elementStartCopyRegion});

				queue->submit({commandEncoder->end()});
				queue->waitIdle();

				const uint32* elementStartMappedRange = elementStartStagingBuffer->map(0, elementStartBuffer->getSize());

				ENGINE_DEBUG("Element count buffer:");

				index elementIndex = 0;

				while (elementIndex < ELEMENT_COUNT) {
					std::cout << ELEMENT_COUNTS[elementIndex] << ' ';

					elementIndex += 1;
				}

				std::cout << '\n';

				ENGINE_DEBUG("Element start buffer:");

				elementIndex = 0;

				while (elementIndex < ELEMENT_COUNT) {
					std::cout << elementStartMappedRange[elementIndex] << ' ';

					elementIndex += 1;
				}

				std::cout << '\n'; */
			}

			void render(ref<CommandEncoder> commandEncoder) const override {}
	};

	class ComputePrefixSumProcess : public Process {
		public:
			sint32 main(view<void*> arguments) override {
				renderer.build();

				return 0;
			}

		private:
			ComputePrefixSumRenderer renderer;
	};
}