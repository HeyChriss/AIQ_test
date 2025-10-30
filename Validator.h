#pragma once
#include "UploadMeta.h"
#include "IngestConfig.h"
#include "IngestResult.h"

class Validator {
public:
    static IngestResult validate(const UploadMeta& meta,
        const IngestConfig& config,
        const std::string& detectedMime,
        long size,
        const std::string& sha256) {
        IngestResult result;
        result.detectedMime = detectedMime;
        result.size = size;
        result.sha256 = sha256;

        // Validate content length if provided
        if (meta.contentLength.has_value()) {
            if (size != meta.contentLength.value()) {
                result.errors.push_back(
                    "Content length mismatch: expected " +
                    std::to_string(meta.contentLength.value()) +
                    ", got " + std::to_string(size));
            }
        }

        // Validate max content length
        if (size > config.maxContentLength) {
            result.errors.push_back(
                "File size " + std::to_string(size) +
                " exceeds maximum " + std::to_string(config.maxContentLength));
        }

        // Validate MIME type (strip parameters for comparison)
        std::string mimeBase = stripMimeParameters(detectedMime);
        if (config.acceptedMimes.find(mimeBase) == config.acceptedMimes.end()) {
            result.errors.push_back(
                "MIME type " + mimeBase + " not in accepted list");
        }

        // Check if claimed MIME matches detected (warning, not error)
        std::string claimedBase = stripMimeParameters(meta.claimedMime);
        if (claimedBase != mimeBase && !claimedBase.empty()) {
            result.errors.push_back(
                "Claimed MIME " + claimedBase +
                " does not match detected " + mimeBase);
        }

        result.ok = result.errors.empty();
        return result;
    }

private:
    static std::string stripMimeParameters(const std::string& mime) {
        size_t semicolon = mime.find(';');
        if (semicolon != std::string::npos) {
            return mime.substr(0, semicolon);
        }
        return mime;
    }
};

