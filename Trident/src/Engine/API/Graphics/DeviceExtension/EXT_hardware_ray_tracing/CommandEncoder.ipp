// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

namespace Trident {
	template<typename CommandEncoder>
	void CommandEncoderDeviceExtension<EXT_hardware_ray_tracing, CommandEncoder>::buildAccelerationStructures(view<ref<AccelerationStructure>> accelerationStructures, view<BaseBufferView> accelerationStructureViews, view<BaseBufferView> scratchViews) {
		CommandEncoder* commandEncoder = static_cast<CommandEncoder*>(this);

		commandEncoder->device->adapter->buildAccelerationStructuresImplementation(commandEncoder, accelerationStructures, accelerationStructureViews, scratchViews);
	}
}