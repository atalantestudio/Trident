#pragma once

namespace ProjectA {
	// Temporary memory shared by all instances of the application.
	class SharedMemory {
		public:
			SharedMemory(std::string_view name, uint64 size);

			~SharedMemory();

			// Offset and size are in bytes.
			void* map(uint64 offset, uint64 size) const;

			void unmap(void* data) const;

		private:
			SharedMemory() = default;

		private:
			void* data;
	};
}