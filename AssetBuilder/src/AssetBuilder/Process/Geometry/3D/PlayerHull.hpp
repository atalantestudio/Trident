// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Geometry/3D/UnitBox.hpp"
#include "AssetBuilder/Process/Geometry/Geometry3DProcess.hpp"

namespace Trident {
	class PlayerHullGeometry3D final : public Geometry3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "playerhull";
			}

			BuildGeometry3D createGeometry() const final override {
				return UnitBoxGeometry3D::createGeometry({33.0f, 77.0f, 33.0f});
			}

		public:
			using Geometry3DProcess::Geometry3DProcess;
	};
}