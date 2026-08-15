// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	template<typename T>
	class ref {
		public:
			ref() = default;

			ref(std::nullptr_t) {}

			// When converting a loaded asset to a ref.
			ref(const T& object) :
				object(const_cast<T*>(&object))
			{}

			ref(const T* object) :
				object(const_cast<T*>(object))
			{}

			ref(T* object) :
				object(object)
			{}

			ref(const ref& other) :
				object(other.object),
				owning(false)
			{}

			template<typename U, typename = std::enable_if<std::is_convertible<U*, T*>::value>::type>
			ref(ref<U>&& other) :
				object(static_cast<T*>(other)),
				owning(other.owning)
			{
				other.owning = false;
			}

			/// Dynamically allocates T with the provided arguments.
			/* template<typename... U>
			explicit ref(U&&... arguments) :
				object(new T(std::forward<U>(arguments)...))
			{} */

			~ref() {
				if (owning) {
					delete object;
				}
			}

			template<typename U, typename = std::enable_if<std::is_convertible<U*, T*>::value>::type>
			ref& operator=(ref<U>& other) {
				if (owning) {
					delete object;
				}

				object = static_cast<T*>(other);

				owning = other.owning;
				other.owning = false;

				return *this;
			}

			template<typename U, typename = std::enable_if<std::is_convertible<U*, T*>::value>::type>
			ref& operator=(ref<U>&& other) {
				object = static_cast<T*>(other);

				owning = other.owning;
				other.owning = false;

				return *this;
			}

			T* operator->() const {
				return object;
			}

			operator T*() const {
				return object;
			}

		private:
			T* object = nullptr;

		public:
			// TODO: Make private.
			bool owning = false;
	};

	template<typename T, typename U>
	inline ref<T> static_pointer_cast(ref<U> r) {
		ref<T> ref(pointer_cast<T*>(r));
		ref.owning = false;

		return ref;
	}

	template<typename T>
	using scope = std::unique_ptr<T>;
}