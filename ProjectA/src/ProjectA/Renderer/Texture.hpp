#pragma once

namespace ProjectA {
	class Texture {
		public:
			virtual ~Texture() = default;

			virtual void bind(uint32_t slot) const = 0;

			virtual uint32_t getWidth() const = 0;
			virtual uint32_t getHeight() const = 0;
	};
}