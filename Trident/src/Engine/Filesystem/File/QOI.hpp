// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ImageFormat.hpp"
#include "Engine/Filesystem/File.hpp"

namespace Trident {
	template<>
	struct File<FileFormat::QOI> {
		ImageFormat format;
		Extent3D extent;

		// Uncompressed and filtered data.
		sequence<uint8> data;
	};

	struct qoi_rgb {
		uint8 r;
		uint8 g;
		uint8 b;

		bool operator ==(qoi_rgb other) const {
			return (
				r == other.r &&
				g == other.g &&
				b == other.b
			);
		}
	};

	struct qoi_rgba {
		uint8 r;
		uint8 g;
		uint8 b;
		uint8 a;

		bool operator ==(qoi_rgba other) const {
			return *pointer_cast<const uint32*>(this) == *pointer_cast<const uint32*>(&other);
		}
	};

	struct qoi_diff {
		uint8 db : 2;
		uint8 dg : 2;
		uint8 dr : 2;
		uint8 tag : 2;
	};

	struct qoi_luma {
		uint8 dg : 6;
		uint8 tag : 2;
		uint8 db_dg : 4;
		uint8 dr_dg : 4;
	};

	struct qoi_index {
		uint8 index : 6;
		uint8 tag : 2;
	};

	struct qoi_run {
		uint8 run : 6;
		uint8 tag : 2;
	};

	inline qoi_index rgbaHash(qoi_rgba p) {
		qoi_index index{};
		index.index = (p.r * 3 + p.g * 5 + p.b * 7 + p.a * 11) % 64;

		return index;
	}
}