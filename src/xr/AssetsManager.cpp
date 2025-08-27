/*
** EPITECH PROJECT, 2025
** maverik
** File description:
** AssetsManager
*/

#include "xr/AssetsManager.hpp"

// TODO Log error when logger will be implemented for Android

maverik::xr::AssetsManager::AssetsManager(AAssetManager* assetManager) : _assetManager(assetManager)
{
}

void maverik::xr::AssetsManager::init(AAssetManager* assetManager)
{
    _assetManager = assetManager;
}


std::shared_ptr<maverik::FileAsset> maverik::xr::AssetsManager::add(const std::string& path)
{
    if (this->exists(path)) {
        return this->get(path);
    }

    std::shared_ptr<maverik::FileAsset> asset = nullptr;

    if (_assetManager && this->isApkAsset(path)) {
        asset = this->loadApkAsset(path);
    }

    if (!asset) {
        asset = this->loadRegularFile(path);
    }
    if (asset) {
        _assets[path] = asset;
    } else {
        // TODO Log error: "Failed to load asset from path: " + path
    }
    return asset;
}

void maverik::xr::AssetsManager::remove(const std::string& path, bool save)
{
    if (!this->exists(path)) {
        return;
    }
    if (save && !this->isApkAsset(path)) {
        this->save(path);
    }
    _assets.erase(path);
}

bool maverik::xr::AssetsManager::save(const std::string& path, const std::string& newPath)
{
    if (!this->exists(path)) {
        // TODO Log error: "Asset does not exist: " + path
        return false;
    }

    if (this->isApkAsset(path) && newPath.empty()) {
        // TODO Log error: "Cannot save APK asset to original path (read-only): " + path
        return false;
    }

    auto asset = this->get(path);

    if (!asset) {
        // TODO Log error: "Asset not found in manager: " + path
        return false;
    }
    std::string savePath = newPath.empty() ? path : newPath;
    std::string content = asset->content();

    return this->saveRegularFile(savePath, content);
}

bool maverik::xr::AssetsManager::isApkAsset(const std::string& path) const
{
    if (!_assetManager) {
        // TODO Log error: "Asset manager not initialized"
        return false;
    }

    AAsset* asset = AAssetManager_open(_assetManager, path.c_str(), AASSET_MODE_STREAMING);
    if (asset) {
        AAsset_close(asset);
        return true;
    }
    return false;
}

std::shared_ptr<maverik::FileAsset> maverik::xr::AssetsManager::loadApkAsset(const std::string& path)
{
    if (!_assetManager) {
        // TODO Log error: "Asset manager not initialized"
        return nullptr;
    }

    AAsset* asset = AAssetManager_open(_assetManager, path.c_str(), AASSET_MODE_STREAMING);

    if (!asset) {
        // TODO Log error: "Failed to open APK asset: " + path
        return nullptr;
    }

    off_t length = AAsset_getLength(asset);
    std::string content(length, '\0');

    if (AAsset_read(asset, content.data(), length) != length) {
        // TODO Log error: "Failed to read APK asset: " + path
        AAsset_close(asset);
        return nullptr;
    }
    AAsset_close(asset);
    return std::make_shared<maverik::FileAsset>(content);
}

std::shared_ptr<maverik::FileAsset> maverik::xr::AssetsManager::loadRegularFile(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        // TODO Log error: "Failed to open regular file: " + path
        return nullptr;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::string content(size, '\0');
    if (!file.read(content.data(), size)) {
        // TODO Log error: "Failed to read regular file: " + path
        return nullptr;
    }
    file.close();
    return std::make_shared<maverik::FileAsset>(content);
}

bool maverik::xr::AssetsManager::saveRegularFile(const std::string& path, const std::string& content)
{
    size_t lastSlash = path.find_last_of("/");
    if (lastSlash != std:string::npos) {
        std::string dir = path.substr(0, lastSlash);
        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
    }
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        // TODO Log error: "Failed to open file for saving: " + path
        return false;
    }


    file.write(content.data(), content.size());
    if (!file.good()) {
        // TODO Log error: "Failed to write to file: " + path
        return false;
    }
    file.close();
    return true;
}

std::string maverik::xr::AssetsManager::getExternalStoragePath(const std::string& folderType) const
{
    std::string path = "/storage/emulated/0/";
    if (folderType.empty()) {
        return path;
    }
    if (folderType == "Download" || folderType == "Downloads") {
        path += "Download/";
    } else if (folderType == "Documents") {
        path += "Documents/";
    } else {
        // TODO Log error: "Unknown folder type: " + folderType
        return "";
    }
}