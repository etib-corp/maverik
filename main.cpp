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
        content.resize(100);
        file->read(&content[0], 1, 100);
        std::cout << "File content: " << content << std::endl;
    } else {
        std::cerr << "Failed to load asset." << std::endl;
    }
    return 0;
}