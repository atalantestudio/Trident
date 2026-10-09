// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/VertexInputAttributeFormat.hpp"

namespace Trident::GL {
	struct VertexInputAttribute {
		GLint size;
		GLenum type;
		GLboolean normalized;
	};

	inline VertexInputAttribute getAttribute(VertexInputAttributeFormat format) {
		static constexpr VertexInputAttribute ATTRIBUTES[static_cast<uint32>(VertexInputAttributeFormat::count)] = {
			{1, GL_BYTE, GL_FALSE}, // R8_SINT
			{1, GL_UNSIGNED_BYTE, GL_FALSE}, // R8_UINT
			{4, GL_UNSIGNED_INT_2_10_10_10_REV, GL_TRUE}, // R10G10B10A2_UNORM
			{1, GL_SHORT, GL_FALSE}, // R16_SINT
			{1, GL_UNSIGNED_SHORT, GL_FALSE}, // R16_UINT
			{1, GL_INT, GL_FALSE}, // R32_SINT
			{1, GL_UNSIGNED_INT, GL_FALSE}, // R32_UINT
			{1, GL_FLOAT, GL_FALSE}, // R32_FLOAT
			{2, GL_UNSIGNED_SHORT, GL_TRUE}, // R16G16_UNORM
			{2, GL_HALF_FLOAT, GL_FALSE}, // R16G16_FLOAT
			{2, GL_INT, GL_FALSE}, // R32G32_SINT
			{2, GL_UNSIGNED_INT, GL_FALSE}, // R32G32_UINT
			{2, GL_FLOAT, GL_FALSE}, // R32G32_FLOAT
			{3, GL_INT, GL_FALSE}, // R32G32B32_SINT
			{3, GL_UNSIGNED_INT, GL_FALSE}, // R32G32B32_UINT
			{3, GL_FLOAT, GL_FALSE}, // R32G32B32_FLOAT
			{4, GL_INT, GL_FALSE}, // R32G32B32A32_SINT
			{4, GL_UNSIGNED_INT, GL_FALSE}, // R32G32B32A32_UINT
			{4, GL_FLOAT, GL_FALSE}, // R32G32B32A32_FLOAT
		};

		ADAPTER_ASSERT(format < VertexInputAttributeFormat::count, "Invalid format.");

		return ATTRIBUTES[static_cast<uint32>(format)];
	}
}