// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// Formats of index buffer indices.
	enum class IndexFormat {
		/// Specifies that indices are 16-bit unsigned integers.
		R16_UINT,

		/// Specifies that indices are 32-bit unsigned integers.
		R32_UINT,

		count,
	};

	inline uint32 getIndexSize(IndexFormat format) {
		static constexpr uint32 INDEX_SIZES[] = {
			sizeof(uint16),
			sizeof(uint32),
		};

		return INDEX_SIZES[static_cast<uint32>(format)];
	}
}