// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GraphicsPipeline {
		public:
			~GraphicsPipeline();

		protected:
			explicit GraphicsPipeline(const Device* device);

		protected:
			const Device* device;
	};
}