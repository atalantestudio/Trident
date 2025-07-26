#pragma once

#include "Engine/API/Rendering/ImageFormat.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"

namespace ProjectA {
	class ImageAsset : public Asset {
		public:
			explicit ImageAsset() {}

			ImageFormat getFormat() const {
				return format;
			}

			const Extent3D& getExtent() const {
				return extent;
			}

			const sequence<uint8>& getData() const {
				return data;
			}

		private:
			ImageFormat format;
			Extent3D extent;
			sequence<uint8> data;
	};
}