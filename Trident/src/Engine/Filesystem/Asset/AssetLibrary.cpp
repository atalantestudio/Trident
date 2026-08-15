// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Asset/AssetSignature.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	AssetLibrary::AssetLibrary(std::filesystem::path basePath) :
		basePath(basePath),
		fontAssets(ASSET_COUNT_PER_TYPE),
		geometry2DAssets(ASSET_COUNT_PER_TYPE),
		geometry3DAssets(ASSET_COUNT_PER_TYPE),
		imageAssets(ASSET_COUNT_PER_TYPE),
		materialAssets(ASSET_COUNT_PER_TYPE),
		scene2DAssets(ASSET_COUNT_PER_TYPE),
		scene3DAssets(ASSET_COUNT_PER_TYPE),
		shaderModuleAssets(ASSET_COUNT_PER_TYPE)
	{}

	const std::unordered_map<uint32, view<char8>>& AssetLibrary::getAssetMap() const {
		return assetMap.map;
	}

	void AssetLibrary::loadAssetMap() {
		const std::string projectNameString = basePath.stem().string();
		const view<char8> projectName(&projectNameString[0], projectNameString.size());
		const std::filesystem::path absolutePath = getAbsolutePath("Asset", projectName, ".tam");
		const sequence<char8> assetPath = absolutePath.string();

		if (!std::filesystem::exists(absolutePath)) {
			ENGINE_WARNING("Failed to locate asset map (looked in []).", assetPath);

			return;
		}

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
	view<FontAsset> AssetLibrary::getLoadedAssets() const {
		return view<FontAsset>(&fontAssets[0], fontAssetCount);
	}

	template<>
	view<Geometry2DAsset> AssetLibrary::getLoadedAssets() const {
		return view<Geometry2DAsset>(&geometry2DAssets[0], geometry2DAssetCount);
	}

	template<>
	view<Geometry3DAsset> AssetLibrary::getLoadedAssets() const {
		return view<Geometry3DAsset>(&geometry3DAssets[0], geometry3DAssetCount);
	}

	template<>
	view<ImageAsset> AssetLibrary::getLoadedAssets() const {
		return view<ImageAsset>(&imageAssets[0], imageAssetCount);
	}

	template<>
	view<MaterialAsset> AssetLibrary::getLoadedAssets() const {
		return view<MaterialAsset>(&materialAssets[0], materialAssetCount);
	}

	template<>
	view<Scene2DAsset> AssetLibrary::getLoadedAssets() const {
		return view<Scene2DAsset>(&scene2DAssets[0], scene2DAssetCount);
	}

	template<>
	view<Scene3DAsset> AssetLibrary::getLoadedAssets() const {
		return view<Scene3DAsset>(&scene3DAssets[0], scene3DAssetCount);
	}

	template<>
	view<ShaderModuleAsset> AssetLibrary::getLoadedAssets() const {
		return view<ShaderModuleAsset>(&shaderModuleAssets[0], shaderModuleAssetCount);
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

		const FontAsset fontAsset = Filesystem::decodeAsset<FontAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(fontAssetCount);
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({fontAsset.identifier, assetIndex});
		fontAssetPathToIndex.insert({stem, assetIndex});

		fontAssets[fontAssetCount] = fontAsset;

		fontAssetCount += 1;
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

		const Geometry2DAsset geometryAsset = Filesystem::decodeAsset<Geometry2DAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(geometry2DAssetCount);
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({geometryAsset.identifier, assetIndex});
		geometryAssetPathToIndex.insert({stem, assetIndex});

		geometry2DAssets[geometry2DAssetCount] = geometryAsset;

		geometry2DAssetCount += 1;
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

		const Geometry3DAsset geometryAsset = Filesystem::decodeAsset<Geometry3DAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(geometry3DAssetCount);
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({geometryAsset.identifier, assetIndex});
		geometryAssetPathToIndex.insert({stem, assetIndex});

		geometry3DAssets[geometry3DAssetCount] = geometryAsset;

		geometry3DAssetCount += 1;
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

		const ImageAsset imageAsset = Filesystem::decodeAsset<ImageAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(imageAssetCount);
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({imageAsset.identifier, assetIndex});
		imageAssetPathToIndex.insert({stem, assetIndex});

		imageAssets[imageAssetCount] = imageAsset;

		imageAssetCount += 1;
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

		const MaterialAsset materialAsset = Filesystem::decodeAsset<MaterialAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(materialAssetCount);
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({materialAsset.identifier, assetIndex});
		materialAssetPathToIndex.insert({stem, assetIndex});

		materialAssets[materialAssetCount] = materialAsset;

		materialAssetCount += 1;
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

		const Scene2DAsset sceneAsset = Filesystem::decodeAsset<Scene2DAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(scene2DAssetCount);
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({sceneAsset.identifier, assetIndex});
		sceneAssetPathToIndex.insert({stem, assetIndex});

		scene2DAssets[scene2DAssetCount] = sceneAsset;

		scene2DAssetCount += 1;
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

		const Scene3DAsset sceneAsset = Filesystem::decodeAsset<Scene3DAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(scene3DAssetCount);
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({sceneAsset.identifier, assetIndex});
		sceneAssetPathToIndex.insert({stem, assetIndex});

		scene3DAssets[scene3DAssetCount] = sceneAsset;

		scene3DAssetCount += 1;
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

		const ShaderModuleAsset shaderModuleAsset = Filesystem::decodeAsset<ShaderModuleAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(shaderModuleAssetCount);
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({shaderModuleAsset.identifier, assetIndex});
		shaderModuleAssetPathToIndex.insert({stem, assetIndex});

		shaderModuleAssets[shaderModuleAssetCount] = shaderModuleAsset;

		shaderModuleAssetCount += 1;
	}
}