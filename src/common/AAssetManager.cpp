/*
** ETIB PROJECT, 2025
** maverik
** File description:
** AAssetManager
*/

#include "AAssetManager.hpp"

std::shared_ptr<maverik::FileAsset> maverik::AAssetManager::addAsset(const std::string &path, const std::string& content)
{
    _assets[path] = std::make_shared<maverik::FileAsset>(content);
    return _assets[path];
}

bool maverik::AAssetManager::assetExists(const std::string &path) const
{
    return _assets.find(path) != _assets.end();
}

std::shared_ptr<maverik::FileAsset> maverik::AAssetManager::getAsset(const std::string &path)
{
    if (assetExists(path)) {
        return _assets[path];
    }
    return nullptr;
}