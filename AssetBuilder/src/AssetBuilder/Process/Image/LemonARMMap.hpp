// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Image/ARMMapFromAORoughnessProcess.hpp"

namespace Trident {
	class LemonARMMap final : public ARMMapFromAORoughnessProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "lemon_arm_1k";
			}

			ARMMapDescriptor getDescriptor() const final override {
				return {
					.aoMapPath = "lemon_1k/lemon_ao_1k.png",
					.roughnessMapPath = "lemon_1k/lemon_rough_1k.png",
				};
			}

		public:
			using ARMMapFromAORoughnessProcess::ARMMapFromAORoughnessProcess;
	};
}