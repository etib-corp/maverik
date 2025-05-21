/*
** EPITECH PROJECT, 2025
** maverik
** File description:
** Version
*/

#include "Version.hpp"

/**
 * @brief Constructs a Version object with the specified major, minor, and patch numbers.
 *
 * @param major The major version number.
 * @param minor The minor version number.
 * @param patch The patch version number.
 */
maverik::Version::Version(unsigned int major, unsigned int minor, unsigned int patch)
{
    this->_major = major;
    this->_minor = minor;
    this->_patch = patch;
}

/**
 * @brief Destructor for the Version class.
 *
 * Cleans up any resources used by the Version instance.
 * Currently, this destructor does not perform any specific actions.
 */
maverik::Version::~Version()
{

}

/**
 * @brief Converts the version information to a 32-bit unsigned integer.
 *
 * This function encodes the major, minor, and patch version numbers
 * into a single 32-bit unsigned integer using the VK_MAKE_VERSION macro,
 * which is commonly used for Vulkan versioning.
 *
 * @return uint32_t The encoded version as a 32-bit unsigned integer.
 */
uint32_t maverik::Version::to_uint32_t()
{
    return VK_MAKE_VERSION(_major, _minor, _patch);
}
