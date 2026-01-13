// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/API/Rendering/ImageView.hpp"

namespace ProjectA {
	class DX12::ImageView final : public ProjectA::ImageView {
		public:
			explicit ImageView(const ImageViewDescriptor& descriptor, const ProjectA::Image* image) :
				ProjectA::ImageView(descriptor, image),
				image(*static_cast<const DX12::Image*>(image))
			{}

		private:
			const DX12::Image& image;
	};
}