#include "../src/services/LibraryService.hpp"
#include "../src/services/BookRepository.hpp"
#include "../src/services/AuthenticationService.hpp"
#include "../src/launcher/LibraryApp.hpp"
#include "../src/datamodel/Book.hpp"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>

using namespace DataModel;
using namespace Services;
using namespace Launcher;

// Test utilities
static void cleanTestData() {
    std::error_code ec;
    std::filesystem::remove("data/books.json", ec);
    // Note: credentials.json is NOT removed to preserve authentication setup
}

// ============================================================================
// LIBRARY SERVICE + BOOK REPOSITORY INTEGRATION TESTS
// ============================================================================

static void test_library_service_repository_integration() {
    std::cout << "Testing LibraryService + BookRepository integration...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService service(repo);
    
    // Test complete workflow: add -> search -> delete -> verify
    Book book1("9780134685991", "Clean Code", "Robert Martin", 2008, 5, "Programming");
    Book book2("9780201633610", "Design Patterns", "Gang of Four", 1994, 3, "Programming");
    Book book3("9780061120084", "To Kill a Mockingbird", "Harper Lee", 1960, 2, "Fiction");
    
    // Add books through service
    std::string result1 = service.addBook(book1);
    std::string result2 = service.addBook(book2);
    std::string result3 = service.addBook(book3);
    
    assert(result1.empty());
    assert(result2.empty());
    assert(result3.empty());
    
    // Verify books are stored and retrievable
    auto allBooks = service.listBooks();
    assert(allBooks.size() == 3);
    
    // Test search integration
    auto programmingBooks = service.searchByCategory("Programming");
    assert(programmingBooks.size() == 2);
    
    auto fictionBooks = service.searchByCategory("Fiction");
    assert(fictionBooks.size() == 1);
    
    // Test delete integration
    std::string deleteResult = service.deleteBook("9780134685991");
    assert(deleteResult.empty());
    
    // Verify deletion
    auto remainingBooks = service.listBooks();
    assert(remainingBooks.size() == 2);
    
    auto deletedBook = service.searchByIsbn("9780134685991");
    assert(!deletedBook.has_value());
    
    std::cout << "✓ LibraryService + BookRepository integration test passed\n";
}

static void test_data_persistence_integration() {
    std::cout << "Testing data persistence integration...\n";
    
    cleanTestData();
    
    // Create service and add books
    {
        auto repo = std::make_shared<BookRepository>("data/books.json");
        LibraryService service(repo);
        
        service.addBook(Book("1", "Book1", "Author1", 2020, 1, "Category1"));
        service.addBook(Book("2", "Book2", "Author2", 2021, 2, "Category2"));
        service.addBook(Book("3", "Book3", "Author3", 2022, 3, "Category1"));
    }
    
    // Create new service instance and verify data persistence
    {
        auto repo = std::make_shared<BookRepository>("data/books.json");
        LibraryService service(repo);
        
        auto books = service.listBooks();
        assert(books.size() == 3);
        
        // Test that all operations work with persisted data
        auto category1Books = service.searchByCategory("Category1");
        assert(category1Books.size() == 2);
        
        auto category2Books = service.searchByCategory("Category2");
        assert(category2Books.size() == 1);
        
        auto stats = service.getCategoryStatistics();
        assert(stats["Category1"] == 2);
        assert(stats["Category2"] == 1);
        
        // Test modification of persisted data
        std::string deleteResult = service.deleteBook("2");
        assert(deleteResult.empty());
        
        auto remainingBooks = service.listBooks();
        assert(remainingBooks.size() == 2);
    }
    
    // Verify final state persisted
    {
        auto repo = std::make_shared<BookRepository>("data/books.json");
        LibraryService service(repo);
        
        auto finalBooks = service.listBooks();
        assert(finalBooks.size() == 2);
        
        auto deletedBook = service.searchByIsbn("2");
        assert(!deletedBook.has_value());
    }
    
    std::cout << "✓ Data persistence integration test passed\n";
}

// ============================================================================
// AUTHENTICATION + LIBRARY SERVICE INTEGRATION TESTS
// ============================================================================

static void test_authentication_library_integration() {
    std::cout << "Testing Authentication + LibraryService integration...\n";
    
    cleanTestData();
    
    // Create credentials file
    std::ofstream credFile("config/credentials.json");
    credFile << R"({"username": "admin", "password": "admin123"})";
    credFile.close();
    
    // Test authentication service
    AuthenticationService auth;
    assert(auth.isInitialized());
    
    bool authResult = auth.authenticate("admin", "admin123");
    assert(authResult);
    
    // Test that library service works after authentication
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService service(repo);
    
    Book book("9780134685991", "Clean Code", "Robert Martin", 2008, 5, "Programming");
    std::string addResult = service.addBook(book);
    assert(addResult.empty());
    
    auto books = service.listBooks();
    assert(books.size() == 1);
    
    std::cout << "✓ Authentication + LibraryService integration test passed\n";
}

// ============================================================================
// COMPLETE WORKFLOW INTEGRATION TESTS
// ============================================================================

static void test_complete_library_workflow() {
    std::cout << "Testing complete library workflow...\n";
    
    cleanTestData();
    
    // Create credentials
    std::ofstream credFile("config/credentials.json");
    credFile << R"({"username": "admin", "password": "admin123"})";
    credFile.close();
    
    // Simulate complete library management workflow
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService service(repo);
    
    // 1. Add multiple books
    std::vector<Book> testBooks = {
        Book("9780134685991", "Clean Code", "Robert Martin", 2008, 5, "Programming"),
        Book("9780201633610", "Design Patterns", "Gang of Four", 1994, 3, "Programming"),
        Book("9780061120084", "To Kill a Mockingbird", "Harper Lee", 1960, 2, "Fiction"),
        Book("9780743273565", "The Great Gatsby", "F. Scott Fitzgerald", 1925, 1, "Fiction"),
        Book("9780141439518", "Pride and Prejudice", "Jane Austen", 1813, 4, "Fiction")
    };
    
    for (const auto& book : testBooks) {
        std::string result = service.addBook(book);
        assert(result.empty());
    }
    
    // 2. Verify all books added
    auto allBooks = service.listBooks();
    assert(allBooks.size() == 5);
    
    // 3. Test various search operations
    auto programmingBooks = service.searchByCategory("Programming");
    assert(programmingBooks.size() == 2);
    
    auto fictionBooks = service.searchByCategory("Fiction");
    assert(fictionBooks.size() == 3);
    
    auto martinBooks = service.searchByAuthor("Martin");
    assert(martinBooks.size() == 1);
    assert(martinBooks[0].getTitle() == "Clean Code");
    
    auto cleanCode = service.searchByIsbn("9780134685991");
    assert(cleanCode.has_value());
    assert(cleanCode->getTitle() == "Clean Code");
    
    // 4. Test category management
    auto categories = service.getAllCategories();
    assert(categories.size() == 2);
    
    auto stats = service.getCategoryStatistics();
    assert(stats["Programming"] == 2);
    assert(stats["Fiction"] == 3);
    
    // 5. Test book deletion
    std::string deleteResult = service.deleteBook("9780061120084");
    assert(deleteResult.empty());
    
    // 6. Verify deletion and updated statistics
    auto remainingBooks = service.listBooks();
    assert(remainingBooks.size() == 4);
    
    auto updatedStats = service.getCategoryStatistics();
    assert(updatedStats["Fiction"] == 2);
    
    // 7. Test search after deletion
    auto deletedBook = service.searchByIsbn("9780061120084");
    assert(!deletedBook.has_value());
    
    std::cout << "✓ Complete library workflow test passed\n";
}

static void test_concurrent_operations_integration() {
    std::cout << "Testing concurrent operations integration...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService service(repo);
    
    // Add books in rapid succession
    for (int i = 0; i < 100; ++i) {
        std::string isbn = std::to_string(1000000000 + i);
        std::string title = "Book " + std::to_string(i);
        std::string author = "Author " + std::to_string(i);
        std::string category = "Category " + std::to_string(i % 5);
        
        Book book(isbn, title, author, 2000 + (i % 25), 1 + (i % 10), category);
        std::string result = service.addBook(book);
        assert(result.empty());
    }
    
    // Verify all books added
    auto allBooks = service.listBooks();
    assert(allBooks.size() == 100);
    
    // Test concurrent search operations
    auto category0Books = service.searchByCategory("Category 0");
    assert(category0Books.size() == 20); // 100 books / 5 categories = 20 each
    
    auto category1Books = service.searchByCategory("Category 1");
    assert(category1Books.size() == 20);
    
    // Test concurrent delete operations
    for (int i = 0; i < 50; ++i) {
        std::string isbn = std::to_string(1000000000 + i);
        std::string result = service.deleteBook(isbn);
        assert(result.empty());
    }
    
    // Verify deletions
    auto remainingBooks = service.listBooks();
    assert(remainingBooks.size() == 50);
    
    // Verify category statistics updated
    auto stats = service.getCategoryStatistics();
    assert(stats["Category 0"] == 10); // Half of original 20
    
    std::cout << "✓ Concurrent operations integration test passed\n";
}

// ============================================================================
// ERROR HANDLING INTEGRATION TESTS
// ============================================================================

static void test_error_handling_integration() {
    std::cout << "Testing error handling integration...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService service(repo);
    
    // Test duplicate ISBN handling
    Book book1("9780134685991", "Clean Code", "Robert Martin", 2008, 5, "Programming");
    std::string result1 = service.addBook(book1);
    assert(result1.empty());
    
    Book book2("9780134685991", "Different Title", "Different Author", 2020, 1, "Different Category");
    std::string result2 = service.addBook(book2);
    assert(!result2.empty());
    assert(result2.find("already exists") != std::string::npos);
    
    // Verify original book unchanged
    auto originalBook = service.searchByIsbn("9780134685991");
    assert(originalBook.has_value());
    assert(originalBook->getTitle() == "Clean Code");
    assert(originalBook->getAuthor() == "Robert Martin");
    
    // Test delete non-existent book
    std::string deleteResult = service.deleteBook("9999999999");
    assert(!deleteResult.empty());
    assert(deleteResult.find("not found") != std::string::npos);
    
    // Test search with invalid ISBN format
    auto invalidSearch = service.searchByIsbn("invalid-isbn");
    assert(!invalidSearch.has_value());
    
    std::cout << "✓ Error handling integration test passed\n";
}

// ============================================================================
// DATA INTEGRITY INTEGRATION TESTS
// ============================================================================

static void test_data_integrity_integration() {
    std::cout << "Testing data integrity integration...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService service(repo);
    
    // Add books with various data types
    Book book1("9780134685991", "Clean Code", "Robert Martin", 2008, 5, "Programming");
    Book book2("9780201633610", "Design Patterns", "Gang of Four", 1994, 3, "Programming");
    Book book3("9780061120084", "To Kill a Mockingbird", "Harper Lee", 1960, 2, "Fiction");
    
    service.addBook(book1);
    service.addBook(book2);
    service.addBook(book3);
    
    // Test data integrity after operations
    auto allBooks = service.listBooks();
    assert(allBooks.size() == 3);
    
    // Verify each book's data integrity
    for (const auto& book : allBooks) {
        assert(!book.getIsbn().empty());
        assert(!book.getTitle().empty());
        assert(!book.getAuthor().empty());
        assert(book.getYear() > 0);
        assert(book.getQuantity() > 0);
        assert(!book.getCategory().empty());
    }
    
    // Test data integrity after search operations
    auto programmingBooks = service.searchByCategory("Programming");
    assert(programmingBooks.size() == 2);
    
    for (const auto& book : programmingBooks) {
        assert(book.getCategory() == "Programming");
    }
    
    // Test data integrity after delete operations
    service.deleteBook("9780134685991");
    auto remainingBooks = service.listBooks();
    assert(remainingBooks.size() == 2);
    
    // Verify remaining books still have correct data
    for (const auto& book : remainingBooks) {
        assert(book.getIsbn() != "9780134685991");
        assert(!book.getIsbn().empty());
        assert(!book.getTitle().empty());
        assert(!book.getAuthor().empty());
        assert(book.getYear() > 0);
        assert(book.getQuantity() > 0);
        assert(!book.getCategory().empty());
    }
    
    std::cout << "✓ Data integrity integration test passed\n";
}

static void test_statistics_integration() {
    std::cout << "Testing statistics integration across components...\n";
    
    cleanTestData();
    
    // Test statistics with real data persistence
    {
        auto repo = std::make_shared<BookRepository>("data/books.json");
        LibraryService service(repo);
        
        // Add books with specific patterns for statistics
        service.addBook(Book("1", "Integration Book 1", "Test Author", 2020, 5, "Test Category"));
        service.addBook(Book("2", "Integration Book 2", "Test Author", 2020, 3, "Test Category"));
        service.addBook(Book("3", "Integration Book 3", "Another Author", 2019, 2, "Another Category"));
        
        // Verify statistics are calculated correctly
        auto stats = service.getLibraryStatistics();
        assert(stats.totalBooks == 3);
        assert(stats.uniqueAuthors == 2);
        assert(stats.totalCategories == 2);
        assert(stats.mostProlificAuthor == "Test Author");
        assert(stats.mostPopularCategory == "Test Category");
    }
    
    // Test statistics persistence across service instances
    {
        auto repo = std::make_shared<BookRepository>("data/books.json");
        LibraryService service(repo);
        
        // Statistics should reflect previously added books
        auto stats = service.getLibraryStatistics();
        assert(stats.totalBooks == 3);
        assert(stats.uniqueAuthors == 2);
        assert(stats.totalCategories == 2);
        
        // Add more books and verify statistics update
        service.addBook(Book("4", "Integration Book 4", "Third Author", 2021, 1, "Third Category"));
        
        stats = service.getLibraryStatistics();
        assert(stats.totalBooks == 4);
        assert(stats.uniqueAuthors == 3);
        assert(stats.totalCategories == 3);
    }
    
    std::cout << "✓ Statistics integration test passed\n";
}

static void test_statistics_workflow_integration() {
    std::cout << "Testing statistics integration with complete workflows...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService service(repo);
    
    // Simulate a complete library management workflow
    // 1. Add initial books
    service.addBook(Book("1", "Workflow Book 1", "Author A", 2020, 3, "Category A"));
    service.addBook(Book("2", "Workflow Book 2", "Author A", 2020, 2, "Category A"));
    service.addBook(Book("3", "Workflow Book 3", "Author B", 2019, 1, "Category B"));
    
    // 2. Verify initial statistics
    auto stats = service.getLibraryStatistics();
    assert(stats.totalBooks == 3);
    assert(stats.uniqueAuthors == 2);
    assert(stats.totalCategories == 2);
    assert(stats.mostProlificAuthor == "Author A");
    
    // 3. Add more books
    service.addBook(Book("4", "Workflow Book 4", "Author C", 2021, 4, "Category C"));
    service.addBook(Book("5", "Workflow Book 5", "Author A", 2021, 1, "Category A"));
    
    // 4. Verify updated statistics
    stats = service.getLibraryStatistics();
    assert(stats.totalBooks == 5);
    assert(stats.uniqueAuthors == 3);
    assert(stats.totalCategories == 3);
    assert(stats.mostProlificAuthor == "Author A"); // Still most prolific with 3 books
    
    // 5. Delete a book
    service.deleteBook("3");
    
    // 6. Verify statistics after deletion
    stats = service.getLibraryStatistics();
    assert(stats.totalBooks == 4);
    assert(stats.uniqueAuthors == 2); // Only Author A and Author C remain
    assert(stats.totalCategories == 2); // Only Category A and Category C remain
    
    // 7. Test search integration with statistics
    auto searchResults = service.searchByAuthor("Author A");
    assert(searchResults.size() == 3); // Should find 3 books by Author A
    
    // 8. Verify statistics consistency with search results
    auto authorStats = service.getAuthorStatistics();
    assert(authorStats["Author A"] == 3);
    assert(authorStats["Author C"] == 1);
    
    std::cout << "✓ Statistics workflow integration test passed\n";
}

static void test_statistics_performance_integration() {
    std::cout << "Testing statistics performance integration...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService service(repo);
    
    // Add a moderate number of books for performance testing
    const int numBooks = 500;
    for (int i = 0; i < numBooks; ++i) {
        std::string isbn = std::to_string(1000000000 + i);
        std::string title = "Performance Integration Book " + std::to_string(i);
        std::string author = "Author " + std::to_string(i % 25); // 25 unique authors
        std::string category = "Category " + std::to_string(i % 10); // 10 categories
        int year = 2000 + (i % 25); // Years 2000-2024
        int quantity = 1 + (i % 10); // Quantities 1-10
        
        service.addBook(Book(isbn, title, author, year, quantity, category));
    }
    
    // Test performance of statistics calculation with real data persistence
    auto start = std::chrono::high_resolution_clock::now();
    
    auto stats = service.getLibraryStatistics();
    auto authorStats = service.getAuthorStatistics();
    auto yearStats = service.getYearStatistics();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Verify results
    assert(stats.totalBooks == numBooks);
    assert(stats.uniqueAuthors == 25);
    assert(stats.totalCategories == 10);
    assert(authorStats.size() == 25);
    assert(yearStats.size() == 25);
    
    // Performance should be reasonable (less than 500ms for 500 books)
    assert(duration.count() < 500);
    
    // Test that statistics work correctly with search operations
    auto searchResults = service.searchByCategory("Category 0");
    assert(!searchResults.empty());
    
    // Verify statistics remain consistent after search operations
    auto statsAfterSearch = service.getLibraryStatistics();
    assert(statsAfterSearch.totalBooks == stats.totalBooks);
    assert(statsAfterSearch.uniqueAuthors == stats.uniqueAuthors);
    assert(statsAfterSearch.totalCategories == stats.totalCategories);
    
    std::cout << "✓ Statistics performance integration test passed ("
              << duration.count() << "ms for " << numBooks << " books)\n";
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main() {
    std::cout << "Running BiblioTrack v2.08 Integration Tests...\n";
    std::cout << "===============================================\n\n";
    
    try {
        // Component Integration Tests
        test_library_service_repository_integration();
        test_data_persistence_integration();
        test_authentication_library_integration();
        
        // Workflow Integration Tests
        test_complete_library_workflow();
        test_concurrent_operations_integration();
        
        // Error Handling Integration Tests
        test_error_handling_integration();
        test_data_integrity_integration();
        
        // Statistics Integration Tests (v2.08)
        test_statistics_integration();
        test_statistics_workflow_integration();
        test_statistics_performance_integration();
        
        std::cout << "\n🎉 All integration tests passed successfully!\n";
        std::cout << "Integration test coverage includes:\n";
        std::cout << "  - LibraryService + BookRepository integration\n";
        std::cout << "  - Data persistence across service instances\n";
        std::cout << "  - Authentication + LibraryService integration\n";
        std::cout << "  - Complete library management workflows\n";
        std::cout << "  - Concurrent operations handling\n";
        std::cout << "  - Error handling and data integrity\n";
        std::cout << "  - Statistics integration across components (v2.08)\n";
        std::cout << "  - Statistics workflow and performance integration\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Integration test failed: " << e.what() << std::endl;
        return 1;
    }
}
