#pragma once

namespace ProjectA {
	template<typename T>
	class ObjectWrapper {
		public:
			const T object;

		protected:
			ObjectWrapper(const T object) : object(object) {}
	};
}