#pragma once
#include "UploadMeta.h"
#include "IngestConfig.h"
#include "Validator.h"
#include "FileByteSource.h"
#include "StreamProcessor.h"
#include "IngestSink.h"

class Ingestor {
public:
    void ingest(const UploadMeta& meta,
        const IngestConfig& config,
        ByteSource& source,
        IngestSink& sink) {
        StreamProcessor processor;
        processor.process(source);

        std::string detectedMime = processor.getDetectedMime();
        long size = processor.getSize();
        std::string sha256 = processor.getSha256();

        IngestResult result = Validator::validate(meta, config, detectedMime, size, sha256);

        FileByteSource replaySource(processor.getTempFilePath());
        sink.persist(meta, result, replaySource);
    }
};


