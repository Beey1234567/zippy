#pragma once
#include <string>
#include <cstdint>

struct FileMetadata {
    std::string name;
    uint32_t crc;
    uint32_t size;
    uint32_t byteOffset;
};