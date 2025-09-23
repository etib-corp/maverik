#include "AssetsManager.hpp"

int main(int ac, char **av)
{
    maverik::vk::AssetsManager assetsManager;

    if (ac < 2) {
        std::cerr << "Usage: " << av[0] << " <path_to_asset>" << std::endl;
        return 1;
    }
    auto file = assetsManager.add(av[1]);
    if (file) {
        std::string content;
        file->seek(0, maverik::FileAsset::Seek::END);
        int fileSize = file->tell();
        file->seek(0, maverik::FileAsset::Seek::SET);
        content.resize(fileSize);
        file->read(&content[0], 1, fileSize);
        std::cout << content << std::endl;
        file->seek(0, maverik::FileAsset::Seek::END);
        file->write("Hello World!", 1, 12);
    } else {
        std::cerr << "Failed to load asset." << std::endl;
    }

    assetsManager.remove(av[1]);
    if (assetsManager.exists(av[1])) {
        std::cerr << "Asset still exists after removal." << std::endl;
    } else {
        std::cout << "Asset successfully removed." << std::endl;
    }
    std::cout << "End of program." << std::endl;
    return 0;
}