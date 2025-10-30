#pragma once
#include <string>
#include <vector>

class MimeDetector {
public:
    static std::string detect(const std::vector<char>& initialBytes) {
        if (initialBytes.size() < 4) return "application/octet-stream";

        // PDF
        if (initialBytes.size() >= 4 &&
            initialBytes[0] == 0x25 && initialBytes[1] == 0x50 &&
            initialBytes[2] == 0x44 && initialBytes[3] == 0x46)
            return "application/pdf";

        // PNG
        if (initialBytes.size() >= 8 &&
            (unsigned char)initialBytes[0] == 0x89 && initialBytes[1] == 0x50 &&
            initialBytes[2] == 0x4E && initialBytes[3] == 0x47 &&
            (unsigned char)initialBytes[4] == 0x0D && (unsigned char)initialBytes[5] == 0x0A &&
            (unsigned char)initialBytes[6] == 0x1A && (unsigned char)initialBytes[7] == 0x0A)
            return "image/png";

        // DOCX
        if (initialBytes.size() >= 4 &&
            initialBytes[0] == 0x50 && initialBytes[1] == 0x4B &&
            initialBytes[2] == 0x03 && initialBytes[3] == 0x04)
            return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";

        return "application/octet-stream";
    }
};

