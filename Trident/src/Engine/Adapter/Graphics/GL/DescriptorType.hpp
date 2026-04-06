// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/DescriptorType.hpp"

namespace Trident::GL {
	inline GLenum getBufferDescriptorType(DescriptorType descriptorType) {
		static constexpr GLenum DESCRIPTOR_TYPES[] = {
			0,
			0,
			0,
			0,
			GL_UNIFORM_BUFFER,
			GL_SHADER_STORAGE_BUFFER,
			GL_SHADER_STORAGE_BUFFER,
		};

		ADAPTER_ASSERT(descriptorType < DescriptorType::count, "Invalid descriptor type.");

		const GLenum glDescriptorType = DESCRIPTOR_TYPES[static_cast<uint32>(descriptorType)];

		ADAPTER_ASSERT(glDescriptorType != 0, "The provided type is not abuffer descriptor type.");

		return glDescriptorType;
	}
}