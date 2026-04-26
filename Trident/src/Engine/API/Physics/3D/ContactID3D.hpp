// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct ContactID3D {
		inline bool operator==(const ContactID3D& other) const {
			return (
				*(pointer_cast<const uint64*>(this) + 0) == *(pointer_cast<const uint64*>(&other) + 0) &&
				*(pointer_cast<const uint64*>(this) + 1) == *(pointer_cast<const uint64*>(&other) + 1)
			);
		}

		inline void flip() {
			std::swap(referenceEdgeAIndex, referenceEdgeBIndex);
			std::swap(referenceFaceAIndex, referenceFaceBIndex);

			std::swap(incidentEdgeAIndex, incidentEdgeBIndex);
			std::swap(incidentFaceAIndex, incidentFaceBIndex);
		}

		uint16 referenceEdgeAIndex = max<uint16>();
		uint16 referenceEdgeBIndex = max<uint16>();
		uint16 referenceFaceAIndex = max<uint16>();
		uint16 referenceFaceBIndex = max<uint16>();

		uint16 incidentEdgeAIndex = max<uint16>();
		uint16 incidentEdgeBIndex = max<uint16>();
		uint16 incidentFaceAIndex = max<uint16>();
		uint16 incidentFaceBIndex = max<uint16>();
	};
}