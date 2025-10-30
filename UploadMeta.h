#pragma once
#include <string>
#include <optional>

struct UploadMeta {
    std::string filename;
    std::string claimedMime;
    std::optional<long> contentLength;
};

