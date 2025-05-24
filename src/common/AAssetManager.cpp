/*
** ETIB PROJECT, 2025
** Visual Studio Live Share (Workspace)
** File description:
** AAssetManager
*/

#include "AAssetManager.hpp"

std::shared_ptr<maverik::FileAsset> maverik::AAssetManager::addAsset(const std::string &path, const std::ustring& content)
{
    _assets[path] = content;
    return std::make_shared<maverik::FileAsset>(_assets[path]);
}

void maverik::AAssetManager::removeAsset(const std::string &path)
{
    _assets.erase(path);
}

bool maverik::AAssetManager::assetExists(const std::string &path) const
{
    return _assets.find(path) != _assets.end();
}

std::shared_ptr<maverik::FileAsset> maverik::AAssetManager::getAsset(const std::string &path)
{
    if (assetExists(path)) {
        return std::make_shared<maverik::FileAsset>(_assets[path]);
    }
    return nullptr;
}