#pragma once
#include "UploadMeta.h"
#include "IngestResult.h"


class IngestSink {
public:
    virtual ~IngestSink() = default;

    virtual void persist(const UploadMeta& meta,
        const IngestResult& result,
        ByteSource& data) = 0;
};

