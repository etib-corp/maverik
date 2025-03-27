/*
** ETIB PROJECT, 2025
** Visual Studio Live Share (Workspace)
** File description:
** AAssetManager
*/

#include "AAssetManager.hpp"

void maverik::AAssetManager::addAsset(const std::string &path, const std::string& content)
{
    _assets[path] = std::make_pair(content, content.size());
}

void maverik::AAssetManager::removeAsset(const std::string &path)
{
    _assets.erase(path);
}

bool maverik::AAssetManager::assetExists(const std::string &path) const
{
    return _assets.find(path) != _assets.end();
}
