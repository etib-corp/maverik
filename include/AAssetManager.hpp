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
#include <pair>
#include <memory>

namespace maverik {
    class AAssetManager {
        public:
            virtual ~AAssetManager() = default;

            virtual void addAsset(const std::string &path) = 0;
            void addAsset(const std::string &path, const std::string& content);

            void removeAsset(const std::string &path);

            bool assetExists(const std::string &path) const;
            std::shared_ptr<maverik::fileAsset> getAsset(const std::string &path) const;

        protected:
            std::map<std::string, std::pair<std::string, size_t>> _assets;

        private:
    };
} // namespace maverik
