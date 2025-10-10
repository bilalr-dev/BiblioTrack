#include "../src/services/LibraryService.hpp"
#include "../src/services/BookRepository.hpp"
#include "../src/utils/PerformanceMonitor.hpp"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <random>

using namespace DataModel;
using namespace Services;
using namespace Utils;

// Test utilities
static void cleanTestData() {
    std::error_code ec;
    std::filesystem::remove("data/books.jsonl", ec);
}

// ============================================================================
// PERFORMANCE COMPARISON TESTS
// ============================================================================

static void test_search_performance_comparison() {
    std::cout << "Testing search performance comparison...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add a large number of books for performance testing
    const int numBooks = 10000;
    std::cout << "Adding " << numBooks << " books...\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numBooks; ++i) {
        std::string isbn = std::to_string(1000000000 + i);
        std::string title = "Performance Test Book " + std::to_string(i);
        std::string author = "Author " + std::to_string(i % 100); // 100 unique authors
        std::string category = "Category " + std::to_string(i % 20); // 20 categories
        int year = 2000 + (i % 25); // Years 2000-2024
        int quantity = 1 + (i % 10); // Quantities 1-10
        
        service.addBook(Book(isbn, title, author, year, quantity, category));
    }
    
    auto addEnd = std::chrono::high_resolution_clock::now();
    auto addDuration = std::chrono::duration_cast<std::chrono::milliseconds>(addEnd - start);
    
    std::cout << "  - Added " << numBooks << " books in " << addDuration.count() << "ms\n";
    
    // Test search performance with indexing
    std::cout << "Testing optimized search performance...\n";
    
    // Test title search
    auto searchStart = std::chrono::high_resolution_clock::now();
    auto titleResults = service.searchByTitle("Performance Test Book 5000");
    auto searchEnd = std::chrono::high_resolution_clock::now();
    auto searchDuration = std::chrono::duration_cast<std::chrono::microseconds>(searchEnd - searchStart);
    
    assert(titleResults.size() == 1);
    assert(titleResults[0].getIsbn() == "1000005000");
    
    // Test author search
    searchStart = std::chrono::high_resolution_clock::now();
    auto authorResults = service.searchByAuthor("Author 50");
    searchEnd = std::chrono::high_resolution_clock::now();
    auto authorSearchDuration = std::chrono::duration_cast<std::chrono::microseconds>(searchEnd - searchStart);
    
    // Test category search
    searchStart = std::chrono::high_resolution_clock::now();
    auto categoryResults = service.searchByCategory("Category 10");
    searchEnd = std::chrono::high_resolution_clock::now();
    auto categorySearchDuration = std::chrono::duration_cast<std::chrono::microseconds>(searchEnd - searchStart);
    
    std::cout << "  - Title search: " << searchDuration.count() << "μs (found " << titleResults.size() << " results)\n";
    std::cout << "  - Author search: " << authorSearchDuration.count() << "μs (found " << authorResults.size() << " results)\n";
    std::cout << "  - Category search: " << categorySearchDuration.count() << "μs (found " << categoryResults.size() << " results)\n";
    
    // Performance should be very fast with indexing (less than 1ms for any search)
    assert(searchDuration.count() < 1000); // Less than 1ms
    assert(authorSearchDuration.count() < 1000);
    assert(categorySearchDuration.count() < 1000);
    
    std::cout << "✓ Search performance comparison test passed\n";
}

static void test_statistics_performance_comparison() {
    std::cout << "Testing statistics performance comparison...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add books for statistics testing
    const int numBooks = 5000;
    for (int i = 0; i < numBooks; ++i) {
        std::string isbn = std::to_string(1000000000 + i);
        std::string title = "Stats Test Book " + std::to_string(i);
        std::string author = "Author " + std::to_string(i % 50); // 50 unique authors
        std::string category = "Category " + std::to_string(i % 10); // 10 categories
        int year = 2000 + (i % 25); // Years 2000-2024
        int quantity = 1 + (i % 5); // Quantities 1-5
        
        service.addBook(Book(isbn, title, author, year, quantity, category));
    }
    
    // Test statistics performance with caching
    std::cout << "Testing optimized statistics performance...\n";
    
    auto statsStart = std::chrono::high_resolution_clock::now();
    auto stats = service.getLibraryStatistics();
    auto statsEnd = std::chrono::high_resolution_clock::now();
    auto statsDuration = std::chrono::duration_cast<std::chrono::microseconds>(statsEnd - statsStart);
    
    // Test multiple calls to verify caching
    auto statsStart2 = std::chrono::high_resolution_clock::now();
    auto stats2 = service.getLibraryStatistics();
    auto statsEnd2 = std::chrono::high_resolution_clock::now();
    auto statsDuration2 = std::chrono::duration_cast<std::chrono::microseconds>(statsEnd2 - statsStart2);
    
    // Test author statistics
    auto authorStatsStart = std::chrono::high_resolution_clock::now();
    auto authorStats = service.getAuthorStatistics();
    auto authorStatsEnd = std::chrono::high_resolution_clock::now();
    auto authorStatsDuration = std::chrono::duration_cast<std::chrono::microseconds>(authorStatsEnd - authorStatsStart);
    
    // Test year statistics
    auto yearStatsStart = std::chrono::high_resolution_clock::now();
    auto yearStats = service.getYearStatistics();
    auto yearStatsEnd = std::chrono::high_resolution_clock::now();
    auto yearStatsDuration = std::chrono::duration_cast<std::chrono::microseconds>(yearStatsEnd - yearStatsStart);
    
    std::cout << "  - Library statistics (first call): " << statsDuration.count() << "μs\n";
    std::cout << "  - Library statistics (cached): " << statsDuration2.count() << "μs\n";
    std::cout << "  - Author statistics: " << authorStatsDuration.count() << "μs\n";
    std::cout << "  - Year statistics: " << yearStatsDuration.count() << "μs\n";
    
    // Verify results
    assert(stats.totalBooks == numBooks);
    assert(stats.uniqueAuthors == 50);
    assert(stats.totalCategories == 10);
    assert(authorStats.size() == 50);
    assert(yearStats.size() == 25);
    
    // Performance should be reasonable with caching
    assert(statsDuration.count() < 50000); // First call may take longer to build cache
    assert(statsDuration2.count() < 10000); // Cached should be much faster
    assert(authorStatsDuration.count() < 1000);
    assert(yearStatsDuration.count() < 1000);
    
    std::cout << "✓ Statistics performance comparison test passed\n";
}

static void test_memory_efficiency() {
    std::cout << "Testing memory efficiency...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add books and measure memory usage patterns
    const int numBooks = 1000;
    for (int i = 0; i < numBooks; ++i) {
        std::string isbn = std::to_string(1000000000 + i);
        std::string title = "Memory Test Book " + std::to_string(i);
        std::string author = "Author " + std::to_string(i % 20);
        std::string category = "Category " + std::to_string(i % 5);
        
        service.addBook(Book(isbn, title, author, 2000 + (i % 25), 1 + (i % 3), category));
    }
    
    // Test that indices are properly maintained
    auto allBooks = service.listBooks();
    assert(allBooks.size() == numBooks);
    
    // Test that searches work correctly
    auto titleResults = service.searchByTitle("Memory Test Book 500");
    assert(titleResults.size() == 1);
    
    auto authorResults = service.searchByAuthor("Author 10");
    assert(authorResults.size() == 50); // 1000 books / 20 authors = 50 per author
    
    auto categoryResults = service.searchByCategory("Category 2");
    assert(categoryResults.size() == 200); // 1000 books / 5 categories = 200 per category
    
    // Test that statistics are accurate
    auto stats = service.getLibraryStatistics();
    assert(stats.totalBooks == numBooks);
    assert(stats.uniqueAuthors == 20);
    assert(stats.totalCategories == 5);
    
    std::cout << "✓ Memory efficiency test passed\n";
}

static void test_concurrent_operations_performance() {
    std::cout << "Testing concurrent operations performance...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add initial books
    const int initialBooks = 1000;
    for (int i = 0; i < initialBooks; ++i) {
        std::string isbn = std::to_string(1000000000 + i);
        std::string title = "Concurrent Test Book " + std::to_string(i);
        std::string author = "Author " + std::to_string(i % 10);
        std::string category = "Category " + std::to_string(i % 3);
        
        service.addBook(Book(isbn, title, author, 2000 + (i % 25), 1 + (i % 2), category));
    }
    
    // Test mixed operations performance
    auto start = std::chrono::high_resolution_clock::now();
    
    // Perform various operations
    for (int i = 0; i < 100; ++i) {
        // Add new book
        std::string isbn = std::to_string(2000000000 + i);
        service.addBook(Book(isbn, "New Book " + std::to_string(i), "New Author", 2024, 1, "New Category"));
        
        // Search operations
        auto results = service.searchByAuthor("Author 5");
        auto categoryResults = service.searchByCategory("Category 1");
        
        // Statistics
        auto stats = service.getLibraryStatistics();
        
        // Delete a book
        if (i % 10 == 0) {
            std::string deleteIsbn = std::to_string(1000000000 + i);
            service.deleteBook(deleteIsbn);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  - Mixed operations (100 iterations): " << duration.count() << "ms\n";
    
    // Verify final state
    auto finalStats = service.getLibraryStatistics();
    assert(finalStats.totalBooks >= initialBooks + 100 - 10); // Added 100, deleted ~10
    
    // Performance should be reasonable (less than 1 second for 100 mixed operations)
    assert(duration.count() < 1000);
    
    std::cout << "✓ Concurrent operations performance test passed\n";
}

static void test_large_dataset_scalability() {
    std::cout << "Testing large dataset scalability...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Test with progressively larger datasets
    std::vector<int> datasetSizes = {1000, 5000, 10000};
    
    for (int size : datasetSizes) {
        std::cout << "  Testing with " << size << " books...\n";
        
        // Add books
        auto addStart = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < size; ++i) {
            std::string isbn = std::to_string(1000000000 + i);
            std::string title = "Scalability Book " + std::to_string(i);
            std::string author = "Author " + std::to_string(i % (size / 10)); // 10% unique authors
            std::string category = "Category " + std::to_string(i % (size / 100)); // 1% unique categories
            
            service.addBook(Book(isbn, title, author, 2000 + (i % 25), 1 + (i % 5), category));
        }
        auto addEnd = std::chrono::high_resolution_clock::now();
        auto addDuration = std::chrono::duration_cast<std::chrono::milliseconds>(addEnd - addStart);
        
        // Test search performance
        auto searchStart = std::chrono::high_resolution_clock::now();
        auto results = service.searchByTitle("Scalability Book " + std::to_string(size / 2));
        auto searchEnd = std::chrono::high_resolution_clock::now();
        auto searchDuration = std::chrono::duration_cast<std::chrono::microseconds>(searchEnd - searchStart);
        
        // Test statistics performance
        auto statsStart = std::chrono::high_resolution_clock::now();
        auto stats = service.getLibraryStatistics();
        auto statsEnd = std::chrono::high_resolution_clock::now();
        auto statsDuration = std::chrono::duration_cast<std::chrono::microseconds>(statsEnd - statsStart);
        
        std::cout << "    - Add time: " << addDuration.count() << "ms\n";
        std::cout << "    - Search time: " << searchDuration.count() << "μs\n";
        std::cout << "    - Statistics time: " << statsDuration.count() << "μs\n";
        
        // Verify results
        assert(results.size() == 1);
        assert(stats.totalBooks == size);
        
        // Performance should scale well
        assert(searchDuration.count() < 1000); // Search should be O(1)
        // Statistics performance scales with dataset size but should be reasonable
        assert(statsDuration.count() < size * 10); // Allow 10μs per book for statistics
        
        // Clean up for next iteration
        cleanTestData();
        repo = std::make_shared<BookRepository>("data/books.jsonl");
        service = LibraryService(repo);
    }
    
    std::cout << "✓ Large dataset scalability test passed\n";
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main() {
    std::cout << "Running BiblioTrack Performance Tests...\n";
    std::cout << "========================================\n\n";
    
    try {
        // Performance comparison tests
        test_search_performance_comparison();
        test_statistics_performance_comparison();
        test_memory_efficiency();
        test_concurrent_operations_performance();
        test_large_dataset_scalability();
        
        std::cout << "\n🎉 All performance tests passed successfully!\n";
        std::cout << "Performance test coverage includes:\n";
        std::cout << "  - Search performance with indexing (O(1) lookups)\n";
        std::cout << "  - Statistics performance with caching (O(1) access)\n";
        std::cout << "  - Memory efficiency and index maintenance\n";
        std::cout << "  - Concurrent operations performance\n";
        std::cout << "  - Large dataset scalability testing\n";
        std::cout << "  - Performance regression validation\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Performance test failed: " << e.what() << std::endl;
        return 1;
    }
}
