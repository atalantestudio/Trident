// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/ECS/Entity.hpp"
#include "Engine/ECS/Pool/Pool.hpp"

namespace Trident {
	template<typename TagType>
	class TagPool : public Pool {
		private:
			static constexpr uint32 NULL_TAG_INDEX = max<uint32>();

		public:
			explicit TagPool() {
				std::memset(&entityToTagIndex, NULL_TAG_INDEX, MAX_ENTITIES);
			}

			bool hasTag(Entity entity) const {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] is overflowing.", entity);

				return entityToTagIndex[entity] != NULL_TAG_INDEX;
			}

			/// Sets or replaces the tag belonging to the provided entity.
			void setTag(Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] is overflowing.", entity);

				const uint32 tagIndex = entityToTagIndex[entity];

				if (tagIndex != NULL_TAG_INDEX) {
					return;
				}

				entityToTagIndex[entity] = tagCount;

				tagIndexToEntity.emplace_back(entity);

				tagCount += 1;
			}

			/// Erases the tag belonging to the provided entity.
			void resetTag(Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] is overflowing.", entity);

				const uint32 tagIndex = entityToTagIndex[entity];

				if (tagIndex == NULL_TAG_INDEX) {
					ENGINE_ERROR("The tag for entity [] is already reset.", entity);

					return;
				}

				// Don't touch the deleted data; instead wait for another tag to overwrite it.
				tagCount -= 1;

				const Entity lastTaggedEntity = tagIndexToEntity[tagCount];

				// The entity pointing to the last tag now points to the tag at tagIndex.
				entityToTagIndex[lastTaggedEntity] = tagIndex;

				// Reset the index of the tag to be deleted.
				entityToTagIndex[entity] = NULL_TAG_INDEX;
			}

		public:
			// (sparse array)
			std::array<uint32, MAX_ENTITIES> entityToTagIndex;

			// (dense array)
			std::vector<Entity> tagIndexToEntity;

			// Number of currently alive tags.
			uint32 tagCount = 0;
	};
}