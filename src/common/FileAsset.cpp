/*
** ETIB PROJECT, 2025
** Visual Studio Live Share (Workspace)
** File description:
** FileAsset
*/

#include "FileAsset.hpp"

maverik::FileAsset::FileAsset(const std::string& content, size_t size)
    : _content(content), _size(size)
{
}

maverik::FileAsset::~FileAsset()
{
}

size_t maverik::FileAsset::write(const void *ptr, size_t size, size_t nmemb)
{
    _content.append(static_cast<const char *>(ptr), size * nmemb);
    return size * nmemb;
}

size_t maverik::FileAsset::read(void *ptr, size_t size, size_t count)
{
    size_t toRead = size * count;
    if (_pos + toRead > _size)
        toRead = _size - _pos;
    std::memcpy(ptr, _content.c_str() + _pos, toRead);
    _pos += toRead;
    return toRead / size;
}

int maverik::FileAsset::seek(long offset, Seek whence)
{
    switch (whence)
    {
    case FileAsset::Seek::SET:
        _pos = offset;
        break;
    case FileAsset::Seek::CUR:
        _pos += offset;
        break;
    case FileAsset::Seek::END:
        _pos = _size + offset;
        break;
    default:
        return -1;
    }
    return 0;
}

size_t maverik::FileAsset::tell()
{
    return _pos;
}
