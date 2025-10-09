#include "../src/services/LibraryService.hpp"
#include "../src/services/BookRepository.hpp"
#include "../src/services/AuthenticationService.hpp"
#include "../src/datamodel/Book.hpp"
#include <iostream>
#include <filesystem>
#include <cassert>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>

using Services::LibraryService;
using Services::BookRepository;
using Services::AuthenticationService;
using DataModel::Book;

// Test utilities
static void cleanTestData() {
    std::error_code ec;
    std::filesystem::remove("data/books.json", ec);
    // Note: credentials.json is NOT removed to preserve authentication setup
}

static void createTestCredentials() {
    std::ofstream credFile("config/credentials.json");
    credFile << R"({"username": "admin", "password": "admin123"})";
    credFile.close();
}

// ============================================================================
// BASIC FUNCTIONALITY SMOKE TESTS
// ============================================================================

static void test_basic_book_operations() {
    std::cout << "Testing basic book operations...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService svc(repo);
    
    // Test add book
    DataModel::Book book("9780134685991", "Clean Code", "Robert Martin", 2008, 5, "Programming");
    std::string err = svc.addBook(book);
    assert(err.empty());
    
    // Test list books
    auto books = svc.listBooks();
    assert(books.size() == 1);
    assert(books[0].getTitle() == "Clean Code");
    
    // Test search by ISBN
    auto found = svc.searchByIsbn("9780134685991");
    assert(found.has_value());
    assert(found->getAuthor() == "Robert Martin");
    
    // Test delete book
    err = svc.deleteBook("9780134685991");
    assert(err.empty());
    
    // Verify deletion
    auto remainingBooks = svc.listBooks();
    assert(remainingBooks.empty());
    
    std::cout << "✓ Basic book operations test passed\n";
}

static void test_duplicate_isbn_handling() {
    std::cout << "Testing duplicate ISBN handling...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService svc(repo);
    
    DataModel::Book book1("9780134685991", "Clean Code", "Robert Martin", 2008, 5, "Programming");
    std::string err = svc.addBook(book1);
    assert(err.empty());
    
    DataModel::Book book2("9780134685991", "Duplicate Title", "Different Author", 2020, 1, "Different Category");
    err = svc.addBook(book2);
    assert(!err.empty());
    assert(err.find("already exists") != std::string::npos);
    
    // Verify original book unchanged
    auto originalBook = svc.searchByIsbn("9780134685991");
    assert(originalBook.has_value());
    assert(originalBook->getTitle() == "Clean Code");
    
    std::cout << "✓ Duplicate ISBN handling test passed\n";
}

static void test_input_validation() {
    std::cout << "Testing input validation...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService svc(repo);
    
    // Test empty ISBN
    DataModel::Book emptyIsbn("", "Title", "Author", 2020, 1, "Category");
    std::string err = svc.addBook(emptyIsbn);
    assert(!err.empty());
    
    // Test non-numeric ISBN
    DataModel::Book nonNumericIsbn("ABC123", "Title", "Author", 2020, 1, "Category");
    err = svc.addBook(nonNumericIsbn);
    assert(!err.empty());
    
    // Test empty title
    DataModel::Book emptyTitle("1234567890", "", "Author", 2020, 1, "Category");
    err = svc.addBook(emptyTitle);
    assert(!err.empty());
    
    // Test empty author
    DataModel::Book emptyAuthor("1234567890", "Title", "", 2020, 1, "Category");
    err = svc.addBook(emptyAuthor);
    assert(!err.empty());
    
    // Test invalid year (too old)
    DataModel::Book oldYear("1234567890", "Title", "Author", 500, 1, "Category");
    err = svc.addBook(oldYear);
    assert(!err.empty());
    
    // Test invalid year (future)
    DataModel::Book futureYear("1234567890", "Title", "Author", 2030, 1, "Category");
    err = svc.addBook(futureYear);
    assert(!err.empty());
    
    // Test zero quantity
    DataModel::Book zeroQuantity("1234567890", "Title", "Author", 2020, 0, "Category");
    err = svc.addBook(zeroQuantity);
    assert(!err.empty());
    
    // Test negative quantity
    DataModel::Book negativeQuantity("1234567890", "Title", "Author", 2020, -1, "Category");
    err = svc.addBook(negativeQuantity);
    assert(!err.empty());
    
    std::cout << "✓ Input validation test passed\n";
}

// ============================================================================
// SEARCH FUNCTIONALITY SMOKE TESTS
// ============================================================================

static void test_search_functionality() {
    std::cout << "Testing search functionality...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService svc(repo);
    
    // Add test books
    std::vector<DataModel::Book> testBooks = {
        DataModel::Book("9780134685991", "Clean Code", "Robert Martin", 2008, 5, "Programming"),
        DataModel::Book("9780201633610", "Design Patterns", "Gang of Four", 1994, 3, "Programming"),
        DataModel::Book("9780061120084", "To Kill a Mockingbird", "Harper Lee", 1960, 2, "Fiction"),
        DataModel::Book("9780743273565", "The Great Gatsby", "F. Scott Fitzgerald", 1925, 1, "Fiction"),
        DataModel::Book("9780141439518", "Pride and Prejudice", "Jane Austen", 1813, 4, "Fiction")
    };
    
    for (const auto& book : testBooks) {
        std::string err = svc.addBook(book);
        assert(err.empty());
    }
    
    // Test ISBN search (exact match)
    auto isbnResult = svc.searchByIsbn("9780134685991");
    assert(isbnResult.has_value());
    assert(isbnResult->getTitle() == "Clean Code");
    
    // Test ISBN search (not found)
    auto notFound = svc.searchByIsbn("9999999999");
    assert(!notFound.has_value());
    
    // Test title search (partial match)
    auto titleResults = svc.searchByTitle("Clean");
    assert(titleResults.size() == 1);
    assert(titleResults[0].getTitle() == "Clean Code");
    
    // Test title search (case insensitive)
    auto titleResults2 = svc.searchByTitle("clean");
    assert(titleResults2.size() == 1);
    assert(titleResults2[0].getTitle() == "Clean Code");
    
    // Test author search (partial match)
    auto authorResults = svc.searchByAuthor("Martin");
    assert(authorResults.size() == 1);
    assert(authorResults[0].getAuthor() == "Robert Martin");
    
    // Test category search
    auto programmingBooks = svc.searchByCategory("Programming");
    assert(programmingBooks.size() == 2);
    
    auto fictionBooks = svc.searchByCategory("Fiction");
    assert(fictionBooks.size() == 3);
    
    // Test empty search terms
    auto emptyResults = svc.searchByTitle("");
    assert(emptyResults.empty());
    
    std::cout << "✓ Search functionality test passed\n";
}

// ============================================================================
// CATEGORY MANAGEMENT SMOKE TESTS
// ============================================================================

static void test_category_management() {
    std::cout << "Testing category management...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService svc(repo);
    
    // Add books with different categories
    std::vector<DataModel::Book> testBooks = {
        DataModel::Book("1", "Book1", "Author1", 2020, 1, "Programming"),
        DataModel::Book("2", "Book2", "Author2", 2020, 2, "Programming"),
        DataModel::Book("3", "Book3", "Author3", 2020, 1, "Fiction"),
        DataModel::Book("4", "Book4", "Author4", 2020, 1, "General"),
        DataModel::Book("5", "Book5", "Author5", 2020, 1, "Fiction")
    };
    
    for (const auto& book : testBooks) {
        std::string err = svc.addBook(book);
        assert(err.empty());
    }
    
    // Test getAllCategories
    auto categories = svc.getAllCategories();
    assert(categories.size() == 3); // Programming, Fiction, General
    assert(std::find(categories.begin(), categories.end(), "Programming") != categories.end());
    assert(std::find(categories.begin(), categories.end(), "Fiction") != categories.end());
    assert(std::find(categories.begin(), categories.end(), "General") != categories.end());
    
    // Test getCategoryStatistics
    auto stats = svc.getCategoryStatistics();
    assert(stats.size() == 3);
    assert(stats["Programming"] == 2);
    assert(stats["Fiction"] == 2);
    assert(stats["General"] == 1);
    
    std::cout << "✓ Category management test passed\n";
}

// ============================================================================
// AUTHENTICATION SMOKE TESTS
// ============================================================================

static void test_authentication_system() {
    std::cout << "Testing authentication system...\n";
    
    cleanTestData();
    createTestCredentials();
    
    AuthenticationService auth;
    assert(auth.isInitialized());
    
    // Test valid credentials
    bool valid = auth.authenticate("admin", "admin123");
    assert(valid);
    
    // Test invalid credentials
    bool invalid = auth.authenticate("admin", "wrongpassword");
    assert(!invalid);
    
    // Test non-existent user
    bool nonExistent = auth.authenticate("nonexistent", "password");
    assert(!nonExistent);
    
    // Test empty credentials
    bool emptyUser = auth.authenticate("", "admin123");
    assert(!emptyUser);
    
    bool emptyPass = auth.authenticate("admin", "");
    assert(!emptyPass);
    
    std::cout << "✓ Authentication system test passed\n";
}

static void test_authentication_missing_file() {
    std::cout << "Testing authentication with missing file...\n";
    
    // Remove credentials file for this specific test
    std::filesystem::remove("config/credentials.json");
    
    AuthenticationService auth;
    assert(!auth.isInitialized());
    
    // Should fail authentication
    bool result = auth.authenticate("admin", "admin123");
    assert(!result);
    
    // Restore credentials file for other tests
    createTestCredentials();
    
    std::cout << "✓ Authentication missing file test passed\n";
}

// ============================================================================
// PERFORMANCE SMOKE TESTS
// ============================================================================

static void test_performance_large_dataset() {
    std::cout << "Testing performance with large dataset...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService svc(repo);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Add many books
    for (int i = 0; i < 1000; ++i) {
        std::string isbn = std::to_string(1000000000 + i);
        std::string title = "Book " + std::to_string(i);
        std::string author = "Author " + std::to_string(i);
        std::string category = "Category " + std::to_string(i % 10);
        
        DataModel::Book book(isbn, title, author, 2000 + (i % 25), 1 + (i % 10), category);
        std::string result = svc.addBook(book);
        assert(result.empty());
    }
    
    auto addEnd = std::chrono::high_resolution_clock::now();
    auto addDuration = std::chrono::duration_cast<std::chrono::milliseconds>(addEnd - start);
    
    // Test search performance
    auto searchStart = std::chrono::high_resolution_clock::now();
    auto results = svc.searchByTitle("Book 500");
    auto searchEnd = std::chrono::high_resolution_clock::now();
    auto searchDuration = std::chrono::duration_cast<std::chrono::milliseconds>(searchEnd - searchStart);
    
    assert(results.size() == 1);
    assert(results[0].getIsbn() == "1000000500");
    
    // Test category statistics performance
    auto statsStart = std::chrono::high_resolution_clock::now();
    auto stats = svc.getCategoryStatistics();
    auto statsEnd = std::chrono::high_resolution_clock::now();
    auto statsDuration = std::chrono::duration_cast<std::chrono::milliseconds>(statsEnd - statsStart);
    
    assert(stats.size() == 10);
    
    std::cout << "  - Added 1000 books in " << addDuration.count() << "ms\n";
    std::cout << "  - Search completed in " << searchDuration.count() << "ms\n";
    std::cout << "  - Category statistics in " << statsDuration.count() << "ms\n";
    
    std::cout << "✓ Performance large dataset test passed\n";
}

// ============================================================================
// EDGE CASE SMOKE TESTS
// ============================================================================

static void test_edge_cases_special_characters() {
    std::cout << "Testing edge cases with special characters...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService svc(repo);
    
    // Test books with special characters
    DataModel::Book book1("1234567890", "Book with \"quotes\"", "Author with 'apostrophes'", 2020, 1, "Category with, commas");
    std::string result = svc.addBook(book1);
    assert(result.empty());
    
    DataModel::Book book2("0987654321", "Book with\nnewlines", "Author with\ttabs", 2020, 1, "Category with; semicolons");
    result = svc.addBook(book2);
    assert(result.empty());
    
    // Test search with special characters
    auto results = svc.searchByTitle("quotes");
    assert(results.size() == 1);
    
    auto results2 = svc.searchByAuthor("apostrophes");
    assert(results2.size() == 1);
    
    std::cout << "✓ Edge cases special characters test passed\n";
}

static void test_edge_cases_boundary_values() {
    std::cout << "Testing edge cases with boundary values...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService svc(repo);
    
    // Test minimum valid year
    DataModel::Book book1("1", "Title", "Author", 1000, 1);
    std::string result = svc.addBook(book1);
    assert(result.empty());
    
    // Test maximum valid year (current year)
    time_t now = time(nullptr);
    struct tm* tm_info = localtime(&now);
    int currentYear = tm_info->tm_year + 1900;
    
    DataModel::Book book2("2", "Title", "Author", currentYear, 1);
    result = svc.addBook(book2);
    assert(result.empty());
    
    // Test minimum quantity
    DataModel::Book book3("3", "Title", "Author", 2020, 1);
    result = svc.addBook(book3);
    assert(result.empty());
    
    // Test large quantity
    DataModel::Book book4("4", "Title", "Author", 2020, 999999);
    result = svc.addBook(book4);
    assert(result.empty());
    
    std::cout << "✓ Edge cases boundary values test passed\n";
}

// ============================================================================
// DATA PERSISTENCE SMOKE TESTS
// ============================================================================

static void test_data_persistence() {
    std::cout << "Testing data persistence...\n";
    
    cleanTestData();
    
    // Create service and add books
    {
        auto repo = std::make_shared<BookRepository>("data/books.json");
        LibraryService svc(repo);
        
        std::vector<DataModel::Book> testBooks = {
            DataModel::Book("1", "Book1", "Author1", 2020, 1, "Category1"),
            DataModel::Book("2", "Book2", "Author2", 2021, 2, "Category2"),
            DataModel::Book("3", "Book3", "Author3", 2022, 3, "Category1")
        };
        
        for (const auto& book : testBooks) {
            std::string result = svc.addBook(book);
            assert(result.empty());
        }
    }
    
    // Create new service instance and verify data persistence
    {
        auto repo = std::make_shared<BookRepository>("data/books.json");
        LibraryService svc(repo);
        
        auto books = svc.listBooks();
        assert(books.size() == 3);
        
        // Test that all operations work with persisted data
        auto category1Books = svc.searchByCategory("Category1");
        assert(category1Books.size() == 2);
        
        auto category2Books = svc.searchByCategory("Category2");
        assert(category2Books.size() == 1);
        
        auto stats = svc.getCategoryStatistics();
        assert(stats["Category1"] == 2);
        assert(stats["Category2"] == 1);
        
        // Test modification of persisted data
        std::string deleteResult = svc.deleteBook("2");
        assert(deleteResult.empty());
        
        auto remainingBooks = svc.listBooks();
        assert(remainingBooks.size() == 2);
    }
    
    // Verify final state persisted
    {
        auto repo = std::make_shared<BookRepository>("data/books.json");
        LibraryService svc(repo);
        
        auto finalBooks = svc.listBooks();
        assert(finalBooks.size() == 2);
        
        auto deletedBook = svc.searchByIsbn("2");
        assert(!deletedBook.has_value());
    }
    
    std::cout << "✓ Data persistence test passed\n";
}

static void test_statistics_smoke() {
    std::cout << "Testing statistics functionality smoke test...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService service(repo);
    
    // Add a diverse set of books for comprehensive statistics testing
    service.addBook(Book("1", "Statistics Test Book 1", "Author A", 2020, 5, "Fiction"));
    service.addBook(Book("2", "Statistics Test Book 2", "Author A", 2020, 3, "Fiction"));
    service.addBook(Book("3", "Statistics Test Book 3", "Author B", 2019, 2, "Science"));
    service.addBook(Book("4", "Statistics Test Book 4", "Author C", 2021, 1, "History"));
    service.addBook(Book("5", "Statistics Test Book 5", "Author A", 2021, 4, "Fiction"));
    service.addBook(Book("6", "Statistics Test Book 6", "Author D", 1990, 10, "Art"));
    
    // Test basic statistics
    auto stats = service.getLibraryStatistics();
    assert(stats.totalBooks == 6);
    assert(stats.totalQuantity == 25); // 5+3+2+1+4+10
    assert(stats.uniqueAuthors == 4);
    assert(stats.totalCategories == 4);
    assert(stats.oldestYear == 1990);
    assert(stats.newestYear == 2021);
    assert(stats.oldestBookTitle == "Statistics Test Book 6");
    assert(stats.newestBookTitle == "Statistics Test Book 4");
    
    // Test category analytics
    assert(stats.mostPopularCategory == "Fiction"); // 3 books
    assert(stats.categoryWithHighestQuantity == "Fiction"); // 12 quantity (5+3+4)
    assert(std::abs(stats.averageBooksPerCategory - 1.5) < 0.1); // 6/4
    
    // Test author analytics
    assert(stats.mostProlificAuthor == "Author A"); // 3 books
    assert(std::abs(stats.averageQuantityPerBook - 4.17) < 0.1); // 25/6
    
    // Test author statistics
    auto authorStats = service.getAuthorStatistics();
    assert(authorStats["Author A"] == 3);
    assert(authorStats["Author B"] == 1);
    assert(authorStats["Author C"] == 1);
    assert(authorStats["Author D"] == 1);
    assert(authorStats.size() == 4);
    
    // Test year statistics
    auto yearStats = service.getYearStatistics();
    assert(yearStats[2020] == 2);
    assert(yearStats[2019] == 1);
    assert(yearStats[2021] == 2);
    assert(yearStats[1990] == 1);
    assert(yearStats.size() == 4);
    
    std::cout << "✓ Statistics functionality smoke test passed\n";
}

static void test_statistics_edge_cases_smoke() {
    std::cout << "Testing statistics edge cases smoke test...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService service(repo);
    
    // Test with single book
    service.addBook(Book("1", "Single Book", "Single Author", 2020, 1, "Single Category"));
    
    auto stats = service.getLibraryStatistics();
    assert(stats.totalBooks == 1);
    assert(stats.uniqueAuthors == 1);
    assert(stats.totalCategories == 1);
    assert(stats.mostPopularCategory == "Single Category");
    assert(stats.categoryWithHighestQuantity == "Single Category");
    assert(stats.mostProlificAuthor == "Single Author");
    assert(stats.averageBooksPerCategory == 1.0);
    assert(stats.averageQuantityPerBook == 1.0);
    
    // Test with empty library
    service.deleteBook("1");
    stats = service.getLibraryStatistics();
    assert(stats.totalBooks == 0);
    assert(stats.totalQuantity == 0);
    assert(stats.uniqueAuthors == 0);
    assert(stats.totalCategories == 0);
    assert(stats.averageBooksPerCategory == 0.0);
    assert(stats.averageQuantityPerBook == 0.0);
    
    // Test with books having same values
    service.addBook(Book("1", "Book1", "Same Author", 2020, 1, "Same Category"));
    service.addBook(Book("2", "Book2", "Same Author", 2020, 1, "Same Category"));
    service.addBook(Book("3", "Book3", "Same Author", 2020, 1, "Same Category"));
    
    stats = service.getLibraryStatistics();
    assert(stats.totalBooks == 3);
    assert(stats.uniqueAuthors == 1);
    assert(stats.totalCategories == 1);
    assert(stats.oldestYear == 2020);
    assert(stats.newestYear == 2020);
    assert(stats.mostPopularCategory == "Same Category");
    assert(stats.categoryWithHighestQuantity == "Same Category");
    assert(stats.mostProlificAuthor == "Same Author");
    
    std::cout << "✓ Statistics edge cases smoke test passed\n";
}

static void test_statistics_performance_smoke() {
    std::cout << "Testing statistics performance smoke test...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService service(repo);
    
    // Add a reasonable number of books for performance testing
    const int numBooks = 200;
    for (int i = 0; i < numBooks; ++i) {
        std::string isbn = std::to_string(1000000000 + i);
        std::string title = "Performance Smoke Book " + std::to_string(i);
        std::string author = "Author " + std::to_string(i % 20); // 20 unique authors
        std::string category = "Category " + std::to_string(i % 8); // 8 categories
        int year = 2000 + (i % 25); // Years 2000-2024
        int quantity = 1 + (i % 5); // Quantities 1-5
        
        service.addBook(Book(isbn, title, author, year, quantity, category));
    }
    
    // Measure performance
    auto start = std::chrono::high_resolution_clock::now();
    
    auto stats = service.getLibraryStatistics();
    auto authorStats = service.getAuthorStatistics();
    auto yearStats = service.getYearStatistics();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Verify results
    assert(stats.totalBooks == numBooks);
    assert(stats.uniqueAuthors == 20);
    assert(stats.totalCategories == 8);
    assert(authorStats.size() == 20);
    assert(yearStats.size() == 25);
    
    // Performance should be reasonable (less than 200ms for 200 books)
    assert(duration.count() < 200);
    
    std::cout << "✓ Statistics performance smoke test passed ("
              << duration.count() << "ms for " << numBooks << " books)\n";
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main() {
    std::cout << "Running BiblioTrack v2.08 Comprehensive Smoke Tests...\n";
    std::cout << "======================================================\n\n";
    
    try {
        // Basic Functionality Tests
        test_basic_book_operations();
        test_duplicate_isbn_handling();
        test_input_validation();
        
        // Search Functionality Tests
        test_search_functionality();
        test_category_management();
        
        // Authentication Tests
        test_authentication_system();
        test_authentication_missing_file();
        
        // Performance Tests
        test_performance_large_dataset();
        
        // Edge Case Tests
        test_edge_cases_special_characters();
        test_edge_cases_boundary_values();
        
        // Data Persistence Tests
        test_data_persistence();
        
        // Statistics Tests (v2.08)
        test_statistics_smoke();
        test_statistics_edge_cases_smoke();
        test_statistics_performance_smoke();
        
        std::cout << "\n🎉 All smoke tests passed successfully!\n";
        std::cout << "Comprehensive smoke test coverage includes:\n";
        std::cout << "  - Basic book operations (add, list, search, delete)\n";
        std::cout << "  - Duplicate ISBN handling and validation\n";
        std::cout << "  - Input validation and error handling\n";
        std::cout << "  - Search functionality (ISBN, title, author, category)\n";
        std::cout << "  - Category management and statistics\n";
        std::cout << "  - Authentication system and security\n";
        std::cout << "  - Performance with large datasets\n";
        std::cout << "  - Edge cases and boundary conditions\n";
        std::cout << "  - Data persistence across sessions\n";
        std::cout << "  - Special characters and data integrity\n";
        std::cout << "  - Statistics functionality and analytics (v2.08)\n";
        std::cout << "  - Statistics edge cases and performance validation\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Smoke test failed: " << e.what() << std::endl;
        return 1;
    }
}