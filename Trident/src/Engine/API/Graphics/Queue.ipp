// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

namespace Trident {
	template<DeviceExtensionName... Extensions>
	ref<ExtendedCommandEncoder<Extensions...>> Queue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (descriptor.usage >= CommandBufferUsage::count) {
					VALIDATION_ERROR("descriptor.usage must be a valid CommandBufferUsage value.");
				}

				if (descriptor.name.count() == 0) {
					VALIDATION_WARNING("descriptor.name should provide a unique name to help identify this command encoder.");
				}
			}
		#endif

		ref<CommandEncoder> commandEncoder = device->adapter->createCommandEncoderImplementation(device, commandPool, descriptor);

		return ref_owning_cast<ExtendedCommandEncoder<Extensions...>>(commandEncoder);
	}
}