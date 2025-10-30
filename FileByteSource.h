#pragma once
#include "ByteSource.h"
#include <fstream>
#include <stdexcept>

class FileByteSource : public ByteSource {
private:
    std::ifstream stream;
    bool eof;

public:
    explicit FileByteSource(const std::string& filepath)
        : stream(filepath, std::ios::binary), eof(false) {
        if (!stream.is_open()) {
            throw std::runtime_error("Failed to open file: " + filepath);
        }
    }

    size_t readChunk(char* buffer, size_t bufferSize) override {
        if (eof) return 0;
        stream.read(buffer, bufferSize);
        size_t bytesRead = stream.gcount();
        if (bytesRead == 0 || stream.eof()) eof = true;
        return bytesRead;
    }

    bool isEof() const override { return eof; }
};


