/*
** ETIB PROJECT, 2025
** maverik
** File description:
** AssetsManager
*/

#include "AssetsManager.hpp"

std::shared_ptr<maverik::FileAsset> maverik::vk::AssetsManager::addAsset(const std::string &path)
{
    if (this->assetExists(path)) {
        return std::make_shared<maverik::FileAsset>(_assets[path]);
    }
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return nullptr;
    }
    std::string content;
    file.seekg(0, std::ios::end);
    content.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&content[0], content.size());
    file.close();
    _assets[path] = content;
    return std::make_shared<maverik::FileAsset>(_assets[path]);
}