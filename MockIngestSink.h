#pragma once
#include "IngestSink.h"
#include "ByteSource.h"
#include "IngestResult.h"
#include <array>

class MockIngestSink : public IngestSink {
private:
    long bytesConsumed = 0;
    IngestResult lastResult;

public:
    void persist(const UploadMeta& meta,
        const IngestResult& result,
        ByteSource& data) override {

        lastResult = result;
        bytesConsumed = 0;

        // Consume all bytes from the source
        std::array<char, 8192> buffer;
        while (!data.isEof()) {
            size_t bytesRead = data.readChunk(buffer.data(), buffer.size());
            if (bytesRead == 0) break;
            bytesConsumed += bytesRead;
        }
    }

    long getBytesConsumed() const { return bytesConsumed; }
    const IngestResult& getLastResult() const { return lastResult; }
};
