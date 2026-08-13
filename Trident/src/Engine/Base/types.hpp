// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	template<typename T>
	class private_ref;

	template<typename T>
	class ref {
		friend class private_ref<T>;

		public:
			ref() = default;

			ref(const T& object) :
				object(const_cast<T*>(&object))
			{}

			template<typename U, typename = std::enable_if<std::is_convertible<U*, T*>::value>::type>
			ref(const ref<U>& other) :
				object(static_cast<T*>(other))
			{}

			ref(T* object) :
				object(object)
			{}

			/// Dynamically allocates T with the provided arguments.
			/* template<typename... U>
			explicit ref(U&&... arguments) :
				object(new T(std::forward<U>(arguments)...))
				#if defined(DEBUG_TRACK_REF_LEAKS)
					, leak(true)
				#endif
			{} */

			#if defined(DEBUG_TRACK_REF_LEAKS)
				~ref() {
					ASSERT(!leak);
				}
			#endif

			T* operator->() const {
				return object;
			}

			operator T*() const {
				return object;
			}

			operator private_ref<T>() const {
				return object;
			}

		private:
			T* object;

			#if defined(DEBUG_TRACK_REF_LEAKS)
				bool leak = false;
			#endif
	};

	template<typename T>
	class private_ref {
		public:
			private_ref() = default;

			private_ref(ref<T>&& other) :
				object(other.object)
			{}

			/// For layers, layer groups and processes.
			template<typename U, typename = std::enable_if<std::is_convertible<U*, T*>::value>::type>
			private_ref(ref<U> other) :
				object(static_cast<T*>(other))
			{}

			~private_ref() {
				delete object;
			}

			private_ref& operator=(ref<T>&& other) {
				object = other.object;

				#if defined(DEBUG_TRACK_REF_LEAKS)
					other.leak = false;
				#endif

				return *this;
			}

			operator T*() const {
				return object;
			}

			operator ref<T>() const {
				return ref<T>(const_cast<T*>(object));
			}

			T* operator->() const {
				return object;
			}

		private:
			T* object;

			#if defined(DEBUG_TRACK_REF_LEAKS)
				// Padding to match ref's size.
				uint8 _;
			#endif
	};

	template<typename T, typename U>
	inline ref<T> static_pointer_cast(ref<U> r) {
		T* a = pointer_cast<T*>(r);

		return a;
	}

	template<typename T>
	using scope = std::unique_ptr<T>;
}