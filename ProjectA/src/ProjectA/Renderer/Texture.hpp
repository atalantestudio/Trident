#pragma once

#include "pch.hpp"

namespace ProjectA {
	class Texture {
		public:
			virtual ~Texture() = default;

			virtual void bind(uint32 slot) const = 0;
			virtual void unbind() const = 0;

			virtual uint32 getWidth() const = 0;
			virtual uint32 getHeight() const = 0;

			virtual void setData(const void* data, uint32 size) const = 0;
	};
}