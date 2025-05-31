/*
** ETIB PROJECT, 2025
** maverik
** File description:
** FileAsset
*/

#include "FileAsset.hpp"

maverik::FileAsset::FileAsset(const std::string& content)
    : _content(content)
{
}

maverik::FileAsset::~FileAsset()
{
}

size_t maverik::FileAsset::write(const void *ptr, size_t size, size_t nmemb)
{
    // TODO: write to file
    size_t _lenBefore = _content.size();
    _content.append(static_cast<const char *>(ptr), size * nmemb);
    size_t _lenAfter = _content.size();
    return (_lenAfter - _lenBefore) / size;
}

size_t maverik::FileAsset::read(void *ptr, size_t size, size_t count)
{
    size_t toRead = size * count;
    if (_pos + toRead > _content.size())
        toRead = _content.size() - _pos;
    std::memcpy(ptr, _content.c_str() + _pos, toRead);
    _pos += toRead;
    return toRead / size;
}

size_t maverik::FileAsset::read(std::string& str, size_t size, size_t count)
{
    return this->read(&str[0], size, count);
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
        _pos = _content.size() + offset;
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
