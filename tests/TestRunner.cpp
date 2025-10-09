#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <filesystem>

// Test suite results structure
struct TestResult {
    std::string testName;
    bool passed;
    std::string errorMessage;
    std::chrono::milliseconds duration;
};

// Test suite function pointer
using TestSuiteFunction = int(*)();

// Test suite information
struct TestSuite {
    std::string name;
    std::string description;
    TestSuiteFunction function;
    std::string executable;
};

// Available test suites
std::vector<TestSuite> testSuites = {
    {
        "UnitTests",
        "Unit tests for individual components (Book, LibraryService, BookRepository, AuthenticationService)",
        nullptr, // Will be set dynamically
        "UnitTests"
    },
    {
        "IntegrationTests", 
        "Integration tests for component interactions and workflows",
        nullptr, // Will be set dynamically
        "IntegrationTests"
    },
    {
        "SmokeTests",
        "Comprehensive smoke tests for end-to-end functionality and edge cases",
        nullptr, // Will be set dynamically
        "SmokeTests"
    }
};

// Run a single test suite
TestResult runTestSuite(const TestSuite& suite) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Running " << suite.name << "\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << "Description: " << suite.description << "\n\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Check if executable exists
    std::string executablePath = "cmake-build-debug/" + suite.executable;
    if (!std::filesystem::exists(executablePath)) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        return {
            suite.name,
            false,
            "Executable not found: " + executablePath + ". Please build the tests first.",
            duration
        };
    }
    
    // Run the test executable
    std::string command = "./" + executablePath;
    int result = std::system(command.c_str());
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    return {
        suite.name,
        result == 0,
        result == 0 ? "" : "Test suite returned non-zero exit code: " + std::to_string(result),
        duration
    };
}

// Print test results summary
void printSummary(const std::vector<TestResult>& results) {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "TEST RESULTS SUMMARY\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    int totalTests = results.size();
    int passedTests = 0;
    int failedTests = 0;
    std::chrono::milliseconds totalDuration(0);
    
    for (const auto& result : results) {
        totalDuration += result.duration;
        
        if (result.passed) {
            passedTests++;
            std::cout << "✅ " << result.testName << " - PASSED (" << result.duration.count() << "ms)\n";
        } else {
            failedTests++;
            std::cout << "❌ " << result.testName << " - FAILED (" << result.duration.count() << "ms)\n";
            if (!result.errorMessage.empty()) {
                std::cout << "   Error: " << result.errorMessage << "\n";
            }
        }
    }
    
    std::cout << "\n" << std::string(80, '-') << "\n";
    std::cout << "TOTAL: " << totalTests << " test suites\n";
    std::cout << "PASSED: " << passedTests << "\n";
    std::cout << "FAILED: " << failedTests << "\n";
    std::cout << "TOTAL TIME: " << totalDuration.count() << "ms\n";
    std::cout << std::string(80, '-') << "\n";
    
    if (failedTests == 0) {
        std::cout << "\n🎉 ALL TESTS PASSED! 🎉\n";
        std::cout << "BiblioTrack v2.08 is working correctly.\n";
    } else {
        std::cout << "\n⚠️  SOME TESTS FAILED!\n";
        std::cout << "Please check the failed test suites above.\n";
    }
}

// Print usage information
void printUsage(const char* programName) {
    std::cout << "BiblioTrack v2.08 Test Runner\n";
    std::cout << "=============================\n\n";
    std::cout << "Usage: " << programName << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help, -h          Show this help message\n";
    std::cout << "  --list, -l          List available test suites\n";
    std::cout << "  --suite <name>      Run specific test suite\n";
    std::cout << "  --all, -a           Run all test suites (default)\n";
    std::cout << "  --build, -b         Build all test suites before running\n\n";
    std::cout << "Available test suites:\n";
    for (const auto& suite : testSuites) {
        std::cout << "  " << suite.name << " - " << suite.description << "\n";
    }
    std::cout << "\nExamples:\n";
    std::cout << "  " << programName << "                    # Run all tests\n";
    std::cout << "  " << programName << " --suite UnitTests  # Run only unit tests\n";
    std::cout << "  " << programName << " --build            # Build and run all tests\n";
}

// List available test suites
void listTestSuites() {
    std::cout << "Available Test Suites:\n";
    std::cout << "======================\n\n";
    
    for (const auto& suite : testSuites) {
        std::cout << "📋 " << suite.name << "\n";
        std::cout << "   " << suite.description << "\n\n";
    }
}

// Build all test suites
bool buildTestSuites() {
    std::cout << "Building all test suites...\n";
    std::cout << "===========================\n\n";
    
    // Run cmake build command
    int result = std::system("cmake --build cmake-build-debug --target UnitTests IntegrationTests SmokeTests");
    
    if (result == 0) {
        std::cout << "\n✅ All test suites built successfully!\n";
        return true;
    } else {
        std::cout << "\n❌ Failed to build test suites!\n";
        return false;
    }
}

// Find test suite by name
TestSuite* findTestSuite(const std::string& name) {
    for (auto& suite : testSuites) {
        if (suite.name == name) {
            return &suite;
        }
    }
    return nullptr;
}

int main(int argc, char* argv[]) {
    std::cout << "BiblioTrack v2.08 Test Runner\n";
    std::cout << "=============================\n\n";
    
    // Parse command line arguments
    bool runAll = true;
    bool buildFirst = false;
    std::string specificSuite;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--list" || arg == "-l") {
            listTestSuites();
            return 0;
        } else if (arg == "--build" || arg == "-b") {
            buildFirst = true;
        } else if (arg == "--suite") {
            if (i + 1 < argc) {
                specificSuite = argv[++i];
                runAll = false;
            } else {
                std::cerr << "Error: --suite requires a test suite name\n";
                return 1;
            }
        } else if (arg == "--all" || arg == "-a") {
            runAll = true;
        } else {
            std::cerr << "Error: Unknown option " << arg << "\n";
            std::cerr << "Use --help for usage information\n";
            return 1;
        }
    }
    
    // Build test suites if requested
    if (buildFirst) {
        if (!buildTestSuites()) {
            return 1;
        }
    }
    
    // Determine which test suites to run
    std::vector<TestSuite> suitesToRun;
    
    if (runAll) {
        suitesToRun = testSuites;
    } else {
        TestSuite* suite = findTestSuite(specificSuite);
        if (suite) {
            suitesToRun.push_back(*suite);
        } else {
            std::cerr << "Error: Test suite '" << specificSuite << "' not found\n";
            std::cerr << "Use --list to see available test suites\n";
            return 1;
        }
    }
    
    // Run test suites
    std::vector<TestResult> results;
    
    for (const auto& suite : suitesToRun) {
        TestResult result = runTestSuite(suite);
        results.push_back(result);
    }
    
    // Print summary
    printSummary(results);
    
    // Return appropriate exit code
    bool allPassed = true;
    for (const auto& result : results) {
        if (!result.passed) {
            allPassed = false;
            break;
        }
    }
    
    return allPassed ? 0 : 1;
}
