#pragma once
#define OPENSSL_SUPPRESS_DEPRECATED
#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <array>
#include <vector>
#include <stdexcept>
#include "MimeDetector.h"
#include "ByteSource.h"

namespace fs = std::filesystem;

class StreamProcessor {
private:
    static constexpr size_t CHUNK_SIZE = 8192;
    SHA256_CTX sha256Ctx;
    long totalSize;
    std::vector<char> initialBytes;
    std::ofstream tempFile;
    fs::path tempFilePath;

public:
    StreamProcessor() : totalSize(0) {
        SHA256_Init(&sha256Ctx);
        tempFilePath = fs::temp_directory_path() / ("ingest_" + std::to_string(rand()) + ".tmp");
        tempFile.open(tempFilePath, std::ios::binary);
        if (!tempFile.is_open()) throw std::runtime_error("Failed to create temporary file");
    }

    ~StreamProcessor() {
        tempFile.close();
        if (fs::exists(tempFilePath)) fs::remove(tempFilePath);
    }

    void process(ByteSource& source) {
        std::array<char, CHUNK_SIZE> buffer;
        bool firstChunk = true;
        while (!source.isEof()) {
            size_t bytesRead = source.readChunk(buffer.data(), CHUNK_SIZE);
            if (bytesRead == 0) break;

            if (firstChunk) {
                size_t captureSize = std::min(bytesRead, (size_t)8);
                initialBytes.insert(initialBytes.end(), buffer.begin(), buffer.begin() + captureSize);
                firstChunk = false;
            }

            SHA256_Update(&sha256Ctx, buffer.data(), bytesRead);
            tempFile.write(buffer.data(), bytesRead);
            totalSize += bytesRead;
        }
        tempFile.flush();
    }

    std::string getDetectedMime() const { return MimeDetector::detect(initialBytes); }
    long getSize() const { return totalSize; }

    std::string getSha256() const {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX ctxCopy = sha256Ctx;
        SHA256_Final(hash, &ctxCopy);
        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        return ss.str();
    }

    std::string getTempFilePath() const { return tempFilePath.string(); }
};


