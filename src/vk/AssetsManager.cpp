/*
** ETIB PROJECT, 2025
** maverik
** File description:
** AssetsManager
*/

#include "AssetsManager.hpp"

std::shared_ptr<maverik::FileAsset> maverik::vk::AssetsManager::addAsset(const std::string &path)
{
    if (assetExists(path)) {
        return std::make_shared<maverik::FileAsset>(_assets[path]);
    }
    std::uifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl; // TODO use custom logger.
        return nullptr;
    }
    std::ustring content((std::istreambuf_iterator<unsigned char>(file)), std::istreambuf_iterator<unsigned char>());
    file.close();
    _assets[path] = content;
    return std::make_shared<maverik::FileAsset>(content);
}