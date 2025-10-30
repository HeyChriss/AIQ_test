#pragma once

#include <set>
#include <string>

struct IngestConfig {
    long maxContentLength;
    std::set<std::string> acceptedMimes;
};

