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

	// 2D and 3D geometries share the same index map.
	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<Geometry2DAsset>() const {
		return geometryAssetPathToIndex;
	}

	// 2D and 3D geometries share the same index map.
	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<Geometry3DAsset>() const {
		return geometryAssetPathToIndex;
	}

	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<ImageAsset>() const {
		return imageAssetPathToIndex;
	}

	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<MaterialAsset>() const {
		return materialAssetPathToIndex;
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

	uint32 AssetLibrary::getLoadedAssetIndex(uint32 identifier) const {
		auto index = assetIdentifierToIndex.find(identifier);

		ENGINE_ASSERT(index != assetIdentifierToIndex.end(), "Asset [] is not loaded.", identifier);

		return index->second;
	}

	template<>
	view<ref<FontAsset>> AssetLibrary::getLoadedAssets() const {
		return view<ref<FontAsset>>(&fontAssets[0], fontAssets.size());
	}

	template<>
	view<ref<Geometry2DAsset>> AssetLibrary::getLoadedAssets() const {
		return view<ref<Geometry2DAsset>>(&geometry2DAssets[0], geometry2DAssets.size());
	}

	template<>
	view<ref<Geometry3DAsset>> AssetLibrary::getLoadedAssets() const {
		return view<ref<Geometry3DAsset>>(&geometry3DAssets[0], geometry3DAssets.size());
	}

	template<>
	view<ref<ImageAsset>> AssetLibrary::getLoadedAssets() const {
		return view<ref<ImageAsset>>(&imageAssets[0], imageAssets.size());
	}

	template<>
	view<ref<MaterialAsset>> AssetLibrary::getLoadedAssets() const {
		return view<ref<MaterialAsset>>(&materialAssets[0], materialAssets.size());
	}

	template<>
	view<ref<Scene2DAsset>> AssetLibrary::getLoadedAssets() const {
		return view<ref<Scene2DAsset>>(&scene2DAssets[0], scene2DAssets.size());
	}

	template<>
	view<ref<Scene3DAsset>> AssetLibrary::getLoadedAssets() const {
		return view<ref<Scene3DAsset>>(&scene3DAssets[0], scene3DAssets.size());
	}

	template<>
	view<ref<ShaderModuleAsset>> AssetLibrary::getLoadedAssets() const {
		return view<ref<ShaderModuleAsset>>(&shaderModuleAssets[0], shaderModuleAssets.size());
	}

	template<>
	void AssetLibrary::loadCachedAsset<FontAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Font", path, ".tfo");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::FONT, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<FontAsset> fontAsset = Filesystem::decodeAsset<FontAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(fontAssets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({fontAsset->identifier, assetIndex});
		fontAssetPathToIndex.insert({stem, assetIndex});
		fontAssets.emplace_back(fontAsset);
	}

	template<>
	void AssetLibrary::loadCachedAsset<Geometry2DAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Geometry", path, ".tga");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::GEOMETRY, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<Geometry2DAsset> geometryAsset = Filesystem::decodeAsset<Geometry2DAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(geometry2DAssets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({geometryAsset->identifier, assetIndex});
		geometryAssetPathToIndex.insert({stem, assetIndex});
		geometry2DAssets.emplace_back(geometryAsset);
	}

	template<>
	void AssetLibrary::loadCachedAsset<Geometry3DAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Geometry", path, ".tga");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::GEOMETRY, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<Geometry3DAsset> geometryAsset = Filesystem::decodeAsset<Geometry3DAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(geometry3DAssets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({geometryAsset->identifier, assetIndex});
		geometryAssetPathToIndex.insert({stem, assetIndex});
		geometry3DAssets.emplace_back(geometryAsset);
	}

	template<>
	void AssetLibrary::loadCachedAsset<ImageAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Image", path, ".tia");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::IMAGE, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<ImageAsset> imageAsset = Filesystem::decodeAsset<ImageAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(imageAssets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({imageAsset->identifier, assetIndex});
		imageAssetPathToIndex.insert({stem, assetIndex});
		imageAssets.emplace_back(imageAsset);
	}

	template<>
	void AssetLibrary::loadCachedAsset<MaterialAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Material", path, ".tma");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::MATERIAL, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<MaterialAsset> materialAsset = Filesystem::decodeAsset<MaterialAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(materialAssets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({materialAsset->identifier, assetIndex});
		materialAssetPathToIndex.insert({stem, assetIndex});
		materialAssets.emplace_back(materialAsset);
	}

	template<>
	void AssetLibrary::loadCachedAsset<Scene2DAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Scene", path, ".tsa");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::SCENE, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<Scene2DAsset> sceneAsset = Filesystem::decodeAsset<Scene2DAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(scene2DAssets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({sceneAsset->identifier, assetIndex});
		sceneAssetPathToIndex.insert({stem, assetIndex});
		scene2DAssets.emplace_back(sceneAsset);
	}

	template<>
	void AssetLibrary::loadCachedAsset<Scene3DAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Scene", path, ".tsa");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::SCENE, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<Scene3DAsset> sceneAsset = Filesystem::decodeAsset<Scene3DAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(scene3DAssets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({sceneAsset->identifier, assetIndex});
		sceneAssetPathToIndex.insert({stem, assetIndex});
		scene3DAssets.emplace_back(sceneAsset);
	}

	template<>
	void AssetLibrary::loadCachedAsset<ShaderModuleAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Shader", path, ".tsm");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::SHADER_MODULE, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<ShaderModuleAsset> shaderModuleAsset = Filesystem::decodeAsset<ShaderModuleAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(shaderModuleAssets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({shaderModuleAsset->identifier, assetIndex});
		shaderModuleAssetPathToIndex.insert({stem, assetIndex});
		shaderModuleAssets.emplace_back(shaderModuleAsset);
	}
}