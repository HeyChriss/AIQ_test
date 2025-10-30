#pragma once
#include <string>
#include <vector>

struct IngestResult {
    std::string detectedMime;
    long size;
    std::string sha256;
    bool ok;
    std::vector<std::string> errors;
};

