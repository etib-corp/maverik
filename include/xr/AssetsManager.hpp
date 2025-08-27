/*
** EPITECH PROJECT, 2025
** maverik
** File description:
** AssetsManager
*/

#pragma once

#include "AAssetsManager.hpp"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

/**
 * @namespace maverik
 * @brief The maverik namespace contains classes and functions for the maverik project.
 */
namespace maverik {
    /**
     * @namespace xr
     * @brief The xr namespace contains classes and functions for the XR (Extended Reality) project.
     */
    namespace xr {
        /**
         * @class AssetsManager
         * @brief The AssetsManager class is responsible for managing file assets.
         *
         * This class inherits from AAssetsManager and implements the addAsset method
         * to load assets from files.
         */
        class AssetsManager : public maverik::AAssetsManager {
            public:
                /**
                 * @brief Default constructor for AssetsManager.
                 */
                AssetsManager() = default;

                /**
                 * @brief Constructs an AssetsManager with a given Android asset manager.
                 * @param assetManager Pointer to the Android asset manager.
                 */
                explicit AssetsManager(AAssetManager* assetManager);

                /**
                 * @brief Default destructor for AssetsManager.
                 */
                ~AssetsManager() override = default;

                /**
                 * @brief Initializes the AssetsManager with an Android asset manager.
                 * @param assetManager Pointer to the Android asset manager.
                 *
                 * This method sets the _assetManager member variable to the provided asset manager.
                 */
                void init(AAssetManager* assetManager);

                /**
                 * @brief Adds an asset to the manager.
                 * @param path The path to the asset.
                 * @return A shared pointer to the FileAsset object. If the file can't be opened,
                 *         it returns a nullptr and logs an error message.
                 * @note This method first tries to load from APK assets, then falls back to regular file system.
                 */
                std::shared_ptr<maverik::FileAsset> add(const std::string &path) override;

                /**
                 * @brief Removes an asset from the manager.
                 * @param path The path to the asset.
                 * @param save Whether to save the asset before removing it (default is true).
                 *
                 * This method removes the asset from the _assets map.
                 * Using a deleted asset will result in undefined behavior.
                 * @note APK assets cannot be saved/modified, only regular files can be saved, so putting true for save will not do anything.
                 */
                void remove(const std::string &path, bool save = true) override;

                /**
                 * @brief Saves an asset to the disk.
                 * @param path The path to the asset.
                 * @param newPath The new path to save the asset to (optional).
                 * @return True if the asset was saved successfully, false otherwise.
                 *
                 * This method is responsible for saving the content of the asset to the specified path.
                 * If newPath is provided, it will save the asset to that path and will not update the original path.
                 * If newPath is not provided, it will save the asset to the original path.
                 *
                 * @note This only works for assets that represent regular files, not APK assets.
                 * APK assets are read-only and cannot be modified.
                 */
                bool save(const std::string &path, const std::string& newPath = "") override;

            private:
                AAssetManager* _assetManager = nullptr;  ///< Pointer to the Android asset manager

                /**
                 * @brief Checks if a path refers to an APK asset.
                 * @param path The path to check.
                 * @return True if the path is an APK asset, false otherwise.
                 */
                bool isApkAsset(const std::string& path) const;

                /**
                 * @brief Loads an asset from the APK.
                 * @param path The asset path within the APK.
                 * @return A shared pointer to the FileAsset object, or nullptr on failure.
                 */
                std::shared_ptr<maverik::FileAsset> loadApkAsset(const std::string& path);

                /**
                 * @brief Loads a regular file from the file system.
                 * @param path The file path.
                 * @return A shared pointer to the FileAsset object, or nullptr on failure.
                 */
                std::shared_ptr<maverik::FileAsset> loadRegularFile(const std::string& path);

                /**
                 * @brief Saves data to a regular file.
                 * @param path The file path to save to.
                 * @param data The data to save.
                 * @return True if saved successfully, false otherwise.
                 */
                bool saveRegularFile(const std::string& path, const std::string& content);

                /**
                 * @brief Gets the Android external storage path (like Downloads, Documents).
                 * @param folderType The type of folder ("Download", "Documents", etc.).
                 * @return The full path to the folder, or empty string if not available.
                 */
                std::string getExternalStoragePath(const std::string& folderType = "") const;
        };
    }
}