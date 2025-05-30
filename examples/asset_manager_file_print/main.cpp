#include "AssetsManager.hpp"

int main(int ac, char **av)
{
    maverik::vk::AssetsManager assetsManager;

    if (ac < 2) {
        std::cerr << "Usage: " << av[0] << " <path_to_asset>" << std::endl;
        return 1;
    }
    auto file = assetsManager.addAsset(av[1]);
    if (file) {
        std::string content;
        file->seek(0, maverik::FileAsset::Seek::END);
        int fileSize = file->tell();
        file->seek(0, maverik::FileAsset::Seek::SET);
        content.resize(fileSize);
        file->read(&content[0], 1, fileSize);
        std::cout << content << std::endl;
    } else {
        std::cerr << "Failed to load asset." << std::endl;
    }
    return 0;
}