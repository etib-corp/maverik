/*
** ETIB PROJECT, 2025
** maverik
** File description:
** FileAsset
*/

#pragma once

#include <string>
#include <memory>
#include <cstring>

namespace maverik {
    class FileAsset {
        public:

            enum class Seek {
                SET,
                CUR,
                END
            };

            FileAsset(const std::string& content, size_t size);
            ~FileAsset();

            size_t write(const void *ptr, size_t size, size_t nmemb);

            size_t read(void *ptr, size_t size, size_t count);

            int seek(long offset, Seek );

            size_t tell();

        protected:
            std::string _content;
            size_t _size;
            size_t _pos;
        private:
    };
}
