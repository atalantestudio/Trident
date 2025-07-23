#pragma once

#include "Engine/API/Physics/Contact/Contact.hpp"

namespace ProjectA {
	class ContactManifold2D {
		public:
			ContactManifold2D() :
				contactCount(0)
			{}

			void setContactCount(uint32 contactCount) {
				this->contactCount = contactCount - 1;
			}

			uint32 getContactCount() const {
				return contactCount + 1;
			}

		public:
			vec2<float32> normal;
			vec2<float32> tangent;
			Contact2D contacts[2];

		private:
			// TODO: Use fewer bits.
			uint8 contactCount : 1;
	};
}