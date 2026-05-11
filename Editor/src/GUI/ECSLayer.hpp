// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Engine/API/GUI/GUILayer.hpp"

namespace Trident {
	class ECSLayer : public GUILayer {
		public:
			void registerAssets() override {}

			void render() override {}
	};
}