/*
** ETIB PROJECT, 2025
** maverik
** File description:
** AssetsManager
*/

#pragma once

#include "AAssetManager.hpp"
#include <iostream>

/**
 * @namespace maverik
 * @brief The maverik namespace contains classes and functions for the maverik project.
 */
namespace maverik {
    /**
     * @namespace vk
     * @brief The vk namespace contains classes and functions for the Vulkan graphics API.
     */
    namespace vk {
        /**
         * @class AssetsManager
         * @brief The AssetsManager class is responsible for managing file assets.
         *
         * This class inherits from AAssetManager and implements the addAsset method
         * to load assets from files.
         */
        class AssetsManager : public AAssetManager {
            public:
                /**
                 * @brief Default constructor for AssetsManager.
                 */
                AssetsManager() = default;

                /**
                 * @brief Default destructor for AssetsManager.
                 */
                ~AssetsManager() override = default;

                /**
                 * @brief Adds an asset to the manager.
                 * @param path The path to the asset.
                 * @return A shared pointer to the FileAsset object. If the file can't be opened,
                 *         it returns a nullptr and logs an error message.
                 */
                std::shared_ptr<maverik::FileAsset> addAsset(const std::string &path) override;
        };
    }
}