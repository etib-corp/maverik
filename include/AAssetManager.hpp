/*
** ETIB PROJECT, 2025
** maverik
** File description:
** AAssetManager
*/

#pragma once

#include "FileAsset.hpp"

#include <string>
#include <map>
#include <utility>
#include <memory>

/**
 * @namespace maverik
 * @brief The maverik namespace contains classes and functions for the maverik project.
 */
namespace maverik {
    /**
     * @class AAssetManager
     * @brief The AAssetManager class is an abstract base class for managing file assets.
     */
    class AAssetManager {
        public:
            /**
             * @brief Defautl destructor for AAssetManager.
             */
            virtual ~AAssetManager() = default;

            /**
             * @brief Adds an asset to the manager.
             * @param path The path to the asset.
             * @return A shared pointer to the FileAsset object.
             *
             * This method is pure virtual and must be implemented by derived classes.
             * This method is responsible for loading the asset from the specified path
             * by reading the file content and storing it in the _assets map.
             */
            virtual std::shared_ptr<maverik::FileAsset> addAsset(const std::string &path) = 0;

            /**
             * @brief Adds an asset to the manager with the specified content.
             * @param path The path to the asset.
             * @param content The content of the asset.
             * @return A shared pointer to the FileAsset object.
             *
             * This method is responsible for creating a new FileAsset object with the
             * specified content and storing it in the _assets map.
             * This method should be used when the file can't be opened by the AssetManager itself.
             * In this case, the content, obtained in some way, is passed as a parameter.
             */
            std::shared_ptr<maverik::FileAsset> addAsset(const std::string &path, const std::ustring& content);

            /**
             * @brief Removes an asset from the manager.
             * @param path The path to the asset.
             *
             * This method removes the asset from the _assets map.
             * Using a deleted asset will result in undefined behavior.
             */
            void removeAsset(const std::string &path);

            /**
             * @brief Checks if an asset exists in the manager.
             * @param path The path to the asset.
             * @return True if the asset exists, false otherwise.
             *
             * This method checks if the asset is present in the _assets map.
             */
            bool assetExists(const std::string &path) const;

            /**
             * @brief Gets an asset from the manager.
             * @param path The path to the asset.
             * @return A shared pointer to the FileAsset object.
             *
             * This method retrieves the asset from the _assets map.
             * If the asset does not exist, it returns a nullptr.
             */
            std::shared_ptr<maverik::FileAsset> getAsset(const std::string &path);

        protected:
            std::map<std::string, std::ustring> _assets;     ///> The map of assets, where the key is the path and the value is the content
    };
} // namespace maverik
