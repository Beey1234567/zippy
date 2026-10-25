#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <string>
#include <cstdint>

#pragma pack(push, 1)

struct LocalFileHeader {
    uint32_t signature = 0x04034b50;
    uint16_t versionneeded = 10;
    uint16_t flags = 0;
    uint16_t compressionMethod = 0;
    uint16_t lastModTime = 0;
    uint16_t lastModDate = 0;
    uint32_t crc32 = 0;
    uint32_t compressedSize = 0;
    uint32_t uncompressedSize = 0;
    uint16_t fileNameLength = 0;
    uint16_t extraFieldLength = 0;
};

struct CentralDirectoryHeader {
    uint32_t signature = 0x02014b50;
    uint16_t versionMadeBy = 10;
    uint16_t flags = 0;
    uint16_t compressionMethod = 0;
    uint16_t lastModTime = 0;
    uint16_t lastModDate = 0;
    uint32_t crc32 = 0;
    uint32_t compressedSize = 0;
    uint32_t uncompressedSize = 0;
    uint16_t fileNameLength = 0;
    uint16_t extraFieldLength = 0;
    uint16_t fileCommentLength = 0;
    uint16_t diskNumberStart = 0;
    uint16_t internalFileAttr = 0;
    uint32_t externalFileAttr = 0;
    uint32_t localHeaderOffset = 0;
};

struct EndOfCentralDirectory {
    uint32_t signature = 0x06054b50;
    uint16_t diskNumber = 0;
    uint16_t startDiskNumber = 0;
    uint16_t numberRecordsOnDisk = 0;
    uint16_t totalRecords = 0;
    uint32_t centralDirectorySize = 0;
    uint32_t centralDirectoryOffset = 0;
    uint16_t commentLength = 0;
};

#pragma pack(pop)