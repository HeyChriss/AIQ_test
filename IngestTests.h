#pragma once
#include <iostream>
#include "Ingestor.h"
#include "FileByteSource.h"
#include "MockIngestSink.h"

class IngestTests
{
public:
	void runTests();
};



void IngestTests::runTests()
{
    std::cout << "Running tests...\n\n";

    IngestConfig config;
    config.maxContentLength = 10 * 1024 * 1024; // 10MB
    config.acceptedMimes = {
        "application/pdf",
        "image/png",
        "application/vnd.openxmlformats-officedocument.wordprocessingml.document"
    };

    Ingestor ingestor;
    MockIngestSink sink;

    // Test 1: Happy path - PDF
    std::cout << "Test 1: Valid PDF upload\n";
    try {
        FileByteSource pdfSource("../test/resources/sample.pdf");
        UploadMeta pdfMeta{ "sample.pdf", "application/pdf", std::nullopt };

        ingestor.ingest(pdfMeta, config, pdfSource, sink);

        auto result = sink.getLastResult();
        std::cout << "  Detected MIME: " << result.detectedMime << "\n";
        std::cout << "  Size: " << result.size << "\n";
        std::cout << "  SHA-256: " << result.sha256 << "\n";
        std::cout << "  OK: " << (result.ok ? "true" : "false") << "\n";
        std::cout << "  Bytes consumed by sink: " << sink.getBytesConsumed() << "\n";
        std::cout << "  ✓ PASSED\n\n";
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ FAILED: " << e.what() << "\n\n";

    }

    // Test 2: Content length mismatch
    std::cout << "Test 2: Content length mismatch\n";
    try {
        FileByteSource pdfSource("../test/resources/sample.pdf");
        UploadMeta pdfMeta{ "sample.pdf", "application/pdf", 999999L };

        ingestor.ingest(pdfMeta, config, pdfSource, sink);

        auto result = sink.getLastResult();
        std::cout << "  OK: " << (result.ok ? "true" : "false") << "\n";
        std::cout << "  Errors:\n";
        for (const auto& err : result.errors) {
            std::cout << "    - " << err << "\n";
        }
        std::cout << "  ✓ PASSED\n\n";
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ FAILED: " << e.what() << "\n\n";
    }

    // Test 3: Verify bytes consumed equals size
    std::cout << "Test 3: Verify sink consumes exact bytes\n";
    try {
        FileByteSource pngSource("../test/resources/sample.png");
        UploadMeta pngMeta{ "sample.png", "image/png", std::nullopt };

        ingestor.ingest(pngMeta, config, pngSource, sink);

        auto result = sink.getLastResult();
        bool match = (result.size == sink.getBytesConsumed());
        std::cout << "  Result size: " << result.size << "\n";
        std::cout << "  Sink consumed: " << sink.getBytesConsumed() << "\n";
        std::cout << "  Match: " << (match ? "true" : "false") << "\n";
        std::cout << "  ✓ PASSED\n\n";
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ FAILED: " << e.what() << "\n\n";
    }

    // Test 4: DOCX file
    std::cout << "Test 4: Valid DOCX upload\n";
    try {
        FileByteSource docxSource("../test/resources/sample.docx");
        UploadMeta docxMeta{ "sample.docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document", std::nullopt };

        ingestor.ingest(docxMeta, config, docxSource, sink);

        auto result = sink.getLastResult();
        std::cout << "  Detected MIME: " << result.detectedMime << "\n";
        std::cout << "  Size: " << result.size << "\n";
        std::cout << "  OK: " << (result.ok ? "true" : "false") << "\n";
        std::cout << "  ✓ PASSED\n\n";
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ FAILED: " << e.what() << "\n\n";
    }

    // Test 5: Claimed MIME mismatch (but detected is accepted)
    std::cout << "Test 5: Claimed MIME mismatch\n";
    try {
        FileByteSource pdfSource("../test/resources/sample.pdf");
        UploadMeta pdfMeta{ "sample.pdf", "image/png", std::nullopt }; // Wrong claim

        ingestor.ingest(pdfMeta, config, pdfSource, sink);

        auto result = sink.getLastResult();
        std::cout << "  Claimed: " << pdfMeta.claimedMime << "\n";
        std::cout << "  Detected: " << result.detectedMime << "\n";
        std::cout << "  OK: " << (result.ok ? "true" : "false") << "\n";
        std::cout << "  Errors:\n";
        for (const auto& err : result.errors) {
            std::cout << "    - " << err << "\n";
        }
        std::cout << "  ✓ PASSED\n\n";
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ FAILED: " << e.what() << "\n\n";
    }

    // Test 6: File exceeds max content length
    std::cout << "Test 6: File exceeds max content length\n";
    try {
        IngestConfig restrictiveConfig;
        restrictiveConfig.maxContentLength = 100; // Very small
        restrictiveConfig.acceptedMimes = config.acceptedMimes;

        FileByteSource pdfSource("../test/resources/sample.pdf");
        UploadMeta pdfMeta{ "sample.pdf", "application/pdf", std::nullopt };

        ingestor.ingest(pdfMeta, restrictiveConfig, pdfSource, sink);

        auto result = sink.getLastResult();
        std::cout << "  OK: " << (result.ok ? "true" : "false") << "\n";
        std::cout << "  Errors:\n";
        for (const auto& err : result.errors) {
            std::cout << "    - " << err << "\n";
        }
        std::cout << "  ✓ PASSED\n\n";
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ FAILED: " << e.what() << "\n\n";
    }

    // Test 7: Unaccepted MIME type
    std::cout << "Test 7: Unaccepted MIME type\n";
    try {
        IngestConfig pdfOnlyConfig;
        pdfOnlyConfig.maxContentLength = 10 * 1024 * 1024;
        pdfOnlyConfig.acceptedMimes = { "application/pdf" }; // Only PDFs

        FileByteSource pngSource("../test/resources/sample.png");
        UploadMeta pngMeta{ "sample.png", "image/png", std::nullopt };

        ingestor.ingest(pngMeta, pdfOnlyConfig, pngSource, sink);

        auto result = sink.getLastResult();
        std::cout << "  OK: " << (result.ok ? "true" : "false") << "\n";
        std::cout << "  Errors:\n";
        for (const auto& err : result.errors) {
            std::cout << "    - " << err << "\n";
        }
        std::cout << "  ✓ PASSED\n\n";
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ FAILED: " << e.what() << "\n\n";
    }

    // Test 8: Multiple errors at once
    std::cout << "Test 8: Multiple validation errors\n";
    try {
        IngestConfig restrictiveConfig;
        restrictiveConfig.maxContentLength = 100; // Too small
        restrictiveConfig.acceptedMimes = { "application/pdf" }; // PNG not allowed

        FileByteSource pngSource("../test/resources/sample.png");
        UploadMeta pngMeta{ "sample.png", "image/jpeg", 50L }; // Wrong claim, wrong size

        ingestor.ingest(pngMeta, restrictiveConfig, pngSource, sink);

        auto result = sink.getLastResult();
        std::cout << "  OK: " << (result.ok ? "true" : "false") << "\n";
        std::cout << "  Number of errors: " << result.errors.size() << "\n";
        std::cout << "  Errors:\n";
        for (const auto& err : result.errors) {
            std::cout << "    - " << err << "\n";
        }
        std::cout << "  ✓ PASSED\n\n";
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ FAILED: " << e.what() << "\n\n";
	}
	std::cout << "All tests completed.\n";
}

