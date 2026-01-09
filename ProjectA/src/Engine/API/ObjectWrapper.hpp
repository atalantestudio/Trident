#pragma once

namespace ProjectA {
	// TODO: Deprecate.
	template<typename T>
	class ObjectWrapper {
		public:
			T object;

		protected:
			ObjectWrapper() :
				object{}
			{}

			ObjectWrapper(const T& object) :
				object(object)
			{}
	};
}