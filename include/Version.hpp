/*
** EPITECH PROJECT, 2025
** maverik
** File description:
** Version
*/

#ifndef VERSION_HPP_
#define VERSION_HPP_

    #include "vulkan.hpp"

namespace maverik {
    class Version {
        public:
            Version(unsigned int major, unsigned int minor, unsigned int patch);
            ~Version();

            uint32_t to_uint32_t();

            unsigned int _major;
            unsigned int _minor;
            unsigned int _patch;
    };
}

#endif /* !VERSION_HPP_ */
