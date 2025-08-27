/*
** ETIB PROJECT, 2025
** maverik
** File description:
** AAssetsManager
*/

#include "AAssetsManager.hpp"

bool maverik::AAssetsManager::exists(const std::string &path) const
{
    return _assets.find(path) != _assets.end();
}

std::shared_ptr<maverik::FileAsset> maverik::AAssetsManager::get(const std::string &path)
{
    if (exists(path)) {
        return _assets[path];
    }
    return nullptr;
}