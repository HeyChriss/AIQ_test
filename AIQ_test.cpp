#include <iostream>
#include "IngestTests.h"   

int main() {
    try {
        IngestTests tests;   
        tests.runTests();    
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}