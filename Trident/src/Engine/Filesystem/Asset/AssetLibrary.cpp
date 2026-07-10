// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Asset/AssetSignature.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	AssetLibrary::AssetLibrary(std::filesystem::path basePath) :
		basePath(basePath)
	{}

	const std::unordered_map<uint32, view<char8>>& AssetLibrary::getAssetMap() const {
		return assetMap.map;
	}

	void AssetLibrary::loadAssetMap() {
		const std::string projectNameString = basePath.stem().string();
		const view<char8> projectName(&projectNameString[0], projectNameString.size());
		const std::filesystem::path absolutePath = getAbsolutePath("Asset", projectName, ".tam");
		const sequence<char8> assetPath = absolutePath.string();

		assetMap.data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&assetMap.data[0], assetMap.data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::ASSET_MAP, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature) + sizeof(VersionNumber));

		while (stream.getRemainingSize() > 0) {
			const uint32 identifier = stream.read<uint32>();
			const uint32 pathCharacterCount = stream.read<uint32>();
			const view<char8> path(pointer_cast<const char8*>(stream.getBytePosition()), pathCharacterCount);

			stream.jump(pathCharacterCount);

			assetMap.map.insert({identifier, path});
		}
	}

	std::filesystem::path AssetLibrary::getAbsolutePath(view<char8> base, view<char8> relativePath, view<char8> extension) const {
		sequence<char8> path(relativePath.count() + extension.count());

		std::copy(relativePath.begin(), relativePath.end(), &path[0]);
		std::copy(extension.begin(), extension.end(), &path[relativePath.count()]);

		return basePath / std::filesystem::path(base.begin(), base.end()) / std::filesystem::path(path.begin(), path.end());
	}

	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<FontAsset>() const {
		return fontAssetPathToIndex;
	}

	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<GeometryAsset>() const {
		return geometryAssetPathToIndex;
	}

	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<ImageAsset>() const {
		return imageAssetPathToIndex;
	}

	// 2D and 3D scenes share the same index map.
	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<Scene2DAsset>() const {
		return sceneAssetPathToIndex;
	}

	// 2D and 3D scenes share the same index map.
	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<Scene3DAsset>() const {
		return sceneAssetPathToIndex;
	}

	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<ShaderModuleAsset>() const {
		return shaderModuleAssetPathToIndex;
	}

	template<>
	void AssetLibrary::loadAsset<FontAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Font", path, ".tfo");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::FONT, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<FontAsset> fontAsset = Filesystem::decodeAsset<FontAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(assets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({fontAsset->getIdentifier(), assetIndex});
		fontAssetPathToIndex.insert({stem, assetIndex});
		assets.emplace_back(fontAsset);
	}

	template<>
	void AssetLibrary::loadAsset<GeometryAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Geometry", path, ".tga");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::GEOMETRY, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<GeometryAsset> geometryAsset = Filesystem::decodeAsset<GeometryAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(assets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({geometryAsset->getIdentifier(), assetIndex});
		geometryAssetPathToIndex.insert({stem, assetIndex});
		assets.emplace_back(geometryAsset);
	}

	template<>
	void AssetLibrary::loadAsset<ImageAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Image", path, ".tia");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::IMAGE, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<ImageAsset> imageAsset = Filesystem::decodeAsset<ImageAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(assets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({imageAsset->getIdentifier(), assetIndex});
		imageAssetPathToIndex.insert({stem, assetIndex});
		assets.emplace_back(imageAsset);
	}

	template<>
	void AssetLibrary::loadAsset<Scene2DAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Scene", path, ".tsa");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::SCENE, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<Scene2DAsset> sceneAsset = Filesystem::decodeAsset<Scene2DAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(assets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({sceneAsset->getIdentifier(), assetIndex});
		sceneAssetPathToIndex.insert({stem, assetIndex});
		assets.emplace_back(sceneAsset);
	}

	template<>
	void AssetLibrary::loadAsset<Scene3DAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Scene", path, ".tsa");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::SCENE, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<Scene3DAsset> sceneAsset = Filesystem::decodeAsset<Scene3DAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(assets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({sceneAsset->getIdentifier(), assetIndex});
		sceneAssetPathToIndex.insert({stem, assetIndex});
		assets.emplace_back(sceneAsset);
	}

	template<>
	void AssetLibrary::loadAsset<ShaderModuleAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Shader", path, ".tsm");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::SHADER_MODULE, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<ShaderModuleAsset> shaderModuleAsset = Filesystem::decodeAsset<ShaderModuleAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(assets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({shaderModuleAsset->getIdentifier(), assetIndex});
		shaderModuleAssetPathToIndex.insert({stem, assetIndex});
		assets.emplace_back(shaderModuleAsset);
	}
}