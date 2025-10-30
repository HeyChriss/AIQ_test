#pragma once
class ByteSource {
public:
    virtual ~ByteSource() = default;

    // Read up to bufferSize bytes into buffer
    // Returns number of bytes actually read (0 = EOF)
    virtual size_t readChunk(char* buffer, size_t bufferSize) = 0;

    // Check if stream has ended
    virtual bool isEof() const = 0;
};

