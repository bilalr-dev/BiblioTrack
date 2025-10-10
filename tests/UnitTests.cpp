#include "../src/datamodel/Book.hpp"
#include "../src/services/LibraryService.hpp"
#include "../src/services/BookRepository.hpp"
#include "../src/services/AuthenticationService.hpp"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>

using namespace DataModel;
using namespace Services;

// Test utilities
static void cleanTestData() {
    std::error_code ec;
    std::filesystem::remove("data/books.jsonl", ec);
    // Note: credentials.json is NOT removed to preserve authentication setup
}

static void cleanTestDataIncludingCredentials() {
    std::error_code ec;
    std::filesystem::remove("data/books.jsonl", ec);
    std::filesystem::remove("config/credentials.json", ec);
}

// ============================================================================
// BOOK DATA MODEL UNIT TESTS
// ============================================================================

static void test_book_default_constructor() {
    std::cout << "Testing Book default constructor...\n";
    
    Book book;
    assert(book.getIsbn().empty());
    assert(book.getTitle().empty());
    assert(book.getAuthor().empty());
    assert(book.getYear() == 0);
    assert(book.getQuantity() == 0);
    assert(book.getCategory() == "General");
    
    std::cout << "✓ Book default constructor test passed\n";
}

static void test_book_parameterized_constructor() {
    std::cout << "Testing Book parameterized constructor...\n";
    
    Book book("9780134685991", "Clean Code", "Robert Martin", 2008, 5, "Programming");
    assert(book.getIsbn() == "9780134685991");
    assert(book.getTitle() == "Clean Code");
    assert(book.getAuthor() == "Robert Martin");
    assert(book.getYear() == 2008);
    assert(book.getQuantity() == 5);
    assert(book.getCategory() == "Programming");
    
    std::cout << "✓ Book parameterized constructor test passed\n";
}

static void test_book_default_category() {
    std::cout << "Testing Book default category...\n";
    
    Book book("1234567890", "Test Book", "Test Author", 2020, 1);
    assert(book.getCategory() == "General");
    
    std::cout << "✓ Book default category test passed\n";
}

static void test_book_setters() {
    std::cout << "Testing Book setters...\n";
    
    Book book;
    book.setTitle("New Title");
    book.setAuthor("New Author");
    book.setYear(2023);
    book.setQuantity(10);
    book.setCategory("Fiction");
    
    assert(book.getTitle() == "New Title");
    assert(book.getAuthor() == "New Author");
    assert(book.getYear() == 2023);
    assert(book.getQuantity() == 10);
    assert(book.getCategory() == "Fiction");
    
    std::cout << "✓ Book setters test passed\n";
}

static void test_book_edge_cases() {
    std::cout << "Testing Book edge cases...\n";
    
    // Empty strings
    Book book1("", "", "", 0, 0, "");
    assert(book1.getIsbn().empty());
    assert(book1.getTitle().empty());
    assert(book1.getAuthor().empty());
    assert(book1.getYear() == 0);
    assert(book1.getQuantity() == 0);
    assert(book1.getCategory().empty());
    
    // Very long strings
    std::string longString(1000, 'A');
    Book book2(longString, longString, longString, 9999, 999999, longString);
    assert(book2.getIsbn() == longString);
    assert(book2.getTitle() == longString);
    assert(book2.getAuthor() == longString);
    assert(book2.getYear() == 9999);
    assert(book2.getQuantity() == 999999);
    assert(book2.getCategory() == longString);
    
    // Negative values
    Book book3("123", "Test", "Author", -100, -5, "Category");
    assert(book3.getYear() == -100);
    assert(book3.getQuantity() == -5);
    
    std::cout << "✓ Book edge cases test passed\n";
}

// ============================================================================
// LIBRARY SERVICE UNIT TESTS
// ============================================================================

static void test_library_service_validation() {
    std::cout << "Testing LibraryService validation methods...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Test valid book
    Book validBook("9780134685991", "Clean Code", "Robert Martin", 2008, 5, "Programming");
    std::string result = service.addBook(validBook);
    assert(result.empty()); // Should succeed
    
    // Test empty ISBN
    Book emptyIsbn("", "Title", "Author", 2020, 1);
    result = service.addBook(emptyIsbn);
    assert(!result.empty());
    assert(result.find("ISBN cannot be empty") != std::string::npos);
    
    // Test non-numeric ISBN
    Book nonNumericIsbn("ABC123", "Title", "Author", 2020, 1);
    result = service.addBook(nonNumericIsbn);
    assert(!result.empty());
    assert(result.find("ISBN must contain digits only") != std::string::npos);
    
    // Test empty title
    Book emptyTitle("1234567890", "", "Author", 2020, 1);
    result = service.addBook(emptyTitle);
    assert(!result.empty());
    assert(result.find("Title cannot be empty") != std::string::npos);
    
    // Test empty author
    Book emptyAuthor("1234567890", "Title", "", 2020, 1);
    result = service.addBook(emptyAuthor);
    assert(!result.empty());
    assert(result.find("Author cannot be empty") != std::string::npos);
    
    // Test invalid year (too old)
    Book oldYear("1234567890", "Title", "Author", 500, 1);
    result = service.addBook(oldYear);
    assert(!result.empty());
    assert(result.find("Invalid year") != std::string::npos);
    
    // Test invalid year (future)
    Book futureYear("1234567890", "Title", "Author", 2030, 1);
    result = service.addBook(futureYear);
    assert(!result.empty());
    assert(result.find("Invalid year") != std::string::npos);
    
    // Test zero quantity
    Book zeroQuantity("1234567890", "Title", "Author", 2020, 0);
    result = service.addBook(zeroQuantity);
    assert(!result.empty());
    assert(result.find("Quantity must be greater than 0") != std::string::npos);
    
    // Test negative quantity
    Book negativeQuantity("1234567890", "Title", "Author", 2020, -1);
    result = service.addBook(negativeQuantity);
    assert(!result.empty());
    assert(result.find("Quantity must be greater than 0") != std::string::npos);
    
    std::cout << "✓ LibraryService validation test passed\n";
}

static void test_library_service_search() {
    std::cout << "Testing LibraryService search methods...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add test books
    service.addBook(Book("9780134685991", "Clean Code", "Robert Martin", 2008, 5, "Programming"));
    service.addBook(Book("9780201633610", "Design Patterns", "Gang of Four", 1994, 3, "Programming"));
    service.addBook(Book("9780061120084", "To Kill a Mockingbird", "Harper Lee", 1960, 2, "Fiction"));
    service.addBook(Book("9780743273565", "The Great Gatsby", "F. Scott Fitzgerald", 1925, 1, "Fiction"));
    
    // Test ISBN search (exact match)
    auto book = service.searchByIsbn("9780134685991");
    assert(book.has_value());
    assert(book->getTitle() == "Clean Code");
    
    // Test ISBN search (not found)
    auto notFound = service.searchByIsbn("9999999999");
    assert(!notFound.has_value());
    
    // Test title search (partial match)
    auto titleResults = service.searchByTitle("Clean");
    assert(titleResults.size() == 1);
    assert(titleResults[0].getTitle() == "Clean Code");
    
    // Test title search (case insensitive)
    auto titleResults2 = service.searchByTitle("clean");
    assert(titleResults2.size() == 1);
    assert(titleResults2[0].getTitle() == "Clean Code");
    
    // Test author search (partial match)
    auto authorResults = service.searchByAuthor("Martin");
    assert(authorResults.size() == 1);
    assert(authorResults[0].getAuthor() == "Robert Martin");
    
    // Test category search
    auto categoryResults = service.searchByCategory("Programming");
    assert(categoryResults.size() == 2);
    
    // Test empty search terms
    auto emptyResults = service.searchByTitle("");
    assert(emptyResults.empty());
    
    auto emptyResults2 = service.searchByAuthor("");
    assert(emptyResults2.empty());
    
    auto emptyResults3 = service.searchByCategory("");
    assert(emptyResults3.empty());
    
    std::cout << "✓ LibraryService search test passed\n";
}

static void test_library_service_category_management() {
    std::cout << "Testing LibraryService category management...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add books with different categories
    service.addBook(Book("1", "Book1", "Author1", 2020, 1, "Programming"));
    service.addBook(Book("2", "Book2", "Author2", 2020, 2, "Programming"));
    service.addBook(Book("3", "Book3", "Author3", 2020, 1, "Fiction"));
    service.addBook(Book("4", "Book4", "Author4", 2020, 1, "General"));
    service.addBook(Book("5", "Book5", "Author5", 2020, 1, "Fiction"));
    
    // Test getAllCategories
    auto categories = service.getAllCategories();
    assert(categories.size() == 3); // Programming, Fiction, General
    assert(std::find(categories.begin(), categories.end(), "Programming") != categories.end());
    assert(std::find(categories.begin(), categories.end(), "Fiction") != categories.end());
    assert(std::find(categories.begin(), categories.end(), "General") != categories.end());
    
    // Test getCategoryStatistics
    auto stats = service.getCategoryStatistics();
    assert(stats.size() == 3);
    assert(stats["Programming"] == 2);
    assert(stats["Fiction"] == 2);
    assert(stats["General"] == 1);
    
    std::cout << "✓ LibraryService category management test passed\n";
}

// ============================================================================
// BOOK REPOSITORY UNIT TESTS
// ============================================================================

static void test_book_repository_basic_operations() {
    std::cout << "Testing BookRepository basic operations...\n";
    
    cleanTestData();
    BookRepository repo("data/books.jsonl");
    
    // Test initial state
    auto books = repo.getAllBooks();
    assert(books.empty());
    
    // Test add book
    Book book("9780134685991", "Clean Code", "Robert Martin", 2008, 5, "Programming");
    bool added = repo.addBook(book);
    assert(added);
    
    // Test duplicate ISBN
    Book duplicate("9780134685991", "Duplicate", "Author", 2020, 1);
    bool duplicateAdded = repo.addBook(duplicate);
    assert(!duplicateAdded);
    
    // Test findByIsbn
    auto found = repo.findByIsbn("9780134685991");
    assert(found.has_value());
    assert(found->getTitle() == "Clean Code");
    
    // Test findByIsbn (not found)
    auto notFound = repo.findByIsbn("9999999999");
    assert(!notFound.has_value());
    
    // Test remove book
    bool removed = repo.removeBook("9780134685991");
    assert(removed);
    
    // Test remove non-existent book
    bool notRemoved = repo.removeBook("9999999999");
    assert(!notRemoved);
    
    std::cout << "✓ BookRepository basic operations test passed\n";
}

static void test_book_repository_persistence() {
    std::cout << "Testing BookRepository persistence...\n";
    
    cleanTestData();
    
    // Create repository and add books
    {
        BookRepository repo("data/books.jsonl");
        repo.addBook(Book("1", "Book1", "Author1", 2020, 1, "Category1"));
        repo.addBook(Book("2", "Book2", "Author2", 2021, 2, "Category2"));
    }
    
    // Create new repository and verify data persisted
    BookRepository repo2("data/books.jsonl");
    auto books = repo2.getAllBooks();
    assert(books.size() == 2);
    
    // Verify book data
    auto book1 = repo2.findByIsbn("1");
    assert(book1.has_value());
    assert(book1->getTitle() == "Book1");
    assert(book1->getAuthor() == "Author1");
    assert(book1->getYear() == 2020);
    assert(book1->getQuantity() == 1);
    assert(book1->getCategory() == "Category1");
    
    std::cout << "✓ BookRepository persistence test passed\n";
}

// ============================================================================
// AUTHENTICATION SERVICE UNIT TESTS
// ============================================================================

static void test_authentication_service_credentials() {
    std::cout << "Testing AuthenticationService credentials...\n";
    
    cleanTestData();
    
    // Create credentials file
    std::ofstream credFile("config/credentials.json");
    credFile << R"({"username": "testuser", "password": "testpass"})";
    credFile.close();
    
    AuthenticationService auth;
    assert(auth.isInitialized());
    
    // Test valid credentials
    bool valid = auth.authenticate("testuser", "testpass");
    assert(valid);
    
    // Test invalid username
    bool invalidUser = auth.authenticate("wronguser", "testpass");
    assert(!invalidUser);
    
    // Test invalid password
    bool invalidPass = auth.authenticate("testuser", "wrongpass");
    assert(!invalidPass);
    
    // Test both invalid
    bool bothInvalid = auth.authenticate("wronguser", "wrongpass");
    assert(!bothInvalid);
    
    // Test empty credentials
    bool emptyUser = auth.authenticate("", "testpass");
    assert(!emptyUser);
    
    bool emptyPass = auth.authenticate("testuser", "");
    assert(!emptyPass);
    
    std::cout << "✓ AuthenticationService credentials test passed\n";
}

static void test_authentication_service_missing_file() {
    std::cout << "Testing AuthenticationService missing file...\n";
    
    cleanTestDataIncludingCredentials();
    
    // Ensure credentials file is removed for this test
    std::filesystem::remove("config/credentials.json");
    
    AuthenticationService auth;
    assert(!auth.isInitialized());
    
    // Should fail authentication
    bool result = auth.authenticate("admin", "admin123");
    assert(!result);
    
    // Restore credentials file for other tests
    std::ofstream credFile("config/credentials.json");
    credFile << R"({"username": "admin", "password": "admin123"})";
    credFile.close();
    
    std::cout << "✓ AuthenticationService missing file test passed\n";
}

// ============================================================================
// EDGE CASE TESTS
// ============================================================================

static void test_edge_cases_large_dataset() {
    std::cout << "Testing edge cases with large dataset...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add many books
    for (int i = 0; i < 1000; ++i) {
        std::string isbn = std::to_string(1000000000 + i);
        std::string title = "Book " + std::to_string(i);
        std::string author = "Author " + std::to_string(i);
        std::string category = "Category " + std::to_string(i % 10);
        
        Book book(isbn, title, author, 2000 + (i % 25), 1 + (i % 10), category);
        std::string result = service.addBook(book);
        assert(result.empty());
    }
    
    // Test search performance
    auto results = service.searchByTitle("Book 500");
    assert(results.size() == 1);
    assert(results[0].getIsbn() == "1000000500");
    
    // Test category statistics
    auto stats = service.getCategoryStatistics();
    assert(stats.size() == 10);
    
    std::cout << "✓ Large dataset edge case test passed\n";
}

static void test_edge_cases_special_characters() {
    std::cout << "Testing edge cases with special characters...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Test books with special characters
    Book book1("1234567890", "Book with \"quotes\"", "Author with 'apostrophes'", 2020, 1, "Category with, commas");
    std::string result = service.addBook(book1);
    assert(result.empty());
    
    Book book2("0987654321", "Book with\nnewlines", "Author with\ttabs", 2020, 1, "Category with; semicolons");
    result = service.addBook(book2);
    assert(result.empty());
    
    // Test search with special characters
    auto results = service.searchByTitle("quotes");
    assert(results.size() == 1);
    
    auto results2 = service.searchByAuthor("apostrophes");
    assert(results2.size() == 1);
    
    std::cout << "✓ Special characters edge case test passed\n";
}

static void test_edge_cases_boundary_values() {
    std::cout << "Testing edge cases with boundary values...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Test minimum valid year
    Book book1("1", "Title", "Author", 1000, 1);
    std::string result = service.addBook(book1);
    assert(result.empty());
    
    // Test maximum valid year (current year)
    time_t now = time(nullptr);
    struct tm* tm_info = localtime(&now);
    int currentYear = tm_info->tm_year + 1900;
    
    Book book2("2", "Title", "Author", currentYear, 1);
    result = service.addBook(book2);
    assert(result.empty());
    
    // Test minimum quantity
    Book book3("3", "Title", "Author", 2020, 1);
    result = service.addBook(book3);
    assert(result.empty());
    
    // Test large quantity
    Book book4("4", "Title", "Author", 2020, 999999);
    result = service.addBook(book4);
    assert(result.empty());
    
    std::cout << "✓ Boundary values edge case test passed\n";
}

// ============================================================================
// STATISTICS FUNCTIONALITY TESTS (v2.08)
// ============================================================================

static void test_library_statistics_basic() {
    std::cout << "Testing LibraryService statistics basic functionality...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add test books
    service.addBook(Book("1", "Book1", "Author1", 2020, 5, "Programming"));
    service.addBook(Book("2", "Book2", "Author2", 2021, 3, "Programming"));
    service.addBook(Book("3", "Book3", "Author1", 2019, 2, "Fiction"));
    service.addBook(Book("4", "Book4", "Author3", 2022, 4, "Fiction"));
    service.addBook(Book("5", "Book5", "Author4", 2018, 1, "General"));
    
    auto stats = service.getLibraryStatistics();
    
    // Test basic counts
    assert(stats.totalBooks == 5);
    assert(stats.totalQuantity == 15); // 5+3+2+4+1
    assert(stats.uniqueAuthors == 4);
    assert(stats.totalCategories == 3);
    
    // Test year range
    assert(stats.oldestYear == 2018);
    assert(stats.newestYear == 2022);
    assert(stats.oldestBookTitle == "Book5");
    assert(stats.newestBookTitle == "Book4");
    
    // Test averages
    assert(std::abs(stats.averageBooksPerCategory - 1.67) < 0.1); // 5/3
    assert(std::abs(stats.averageQuantityPerBook - 3.0) < 0.1); // 15/5
    
    std::cout << "✓ LibraryService statistics basic test passed\n";
}

static void test_library_statistics_analytics() {
    std::cout << "Testing LibraryService statistics analytics...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add books with specific patterns for analytics
    service.addBook(Book("1", "Book1", "Author1", 2020, 5, "Programming"));
    service.addBook(Book("2", "Book2", "Author1", 2021, 3, "Programming"));
    service.addBook(Book("3", "Book3", "Author1", 2019, 2, "Programming"));
    service.addBook(Book("4", "Book4", "Author2", 2022, 10, "Fiction")); // Highest quantity
    service.addBook(Book("5", "Book5", "Author3", 2018, 1, "General"));
    
    auto stats = service.getLibraryStatistics();
    
    // Test category analytics
    assert(stats.mostPopularCategory == "Programming"); // 3 books
    assert(stats.categoryWithHighestQuantity == "Fiction"); // 10 quantity
    
    // Test author analytics
    assert(stats.mostProlificAuthor == "Author1"); // 3 books
    
    std::cout << "✓ LibraryService statistics analytics test passed\n";
}

static void test_author_statistics() {
    std::cout << "Testing LibraryService author statistics...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add books with different authors
    service.addBook(Book("1", "Book1", "Author1", 2020, 1, "Category1"));
    service.addBook(Book("2", "Book2", "Author1", 2021, 1, "Category1"));
    service.addBook(Book("3", "Book3", "Author2", 2019, 1, "Category2"));
    service.addBook(Book("4", "Book4", "Author1", 2022, 1, "Category1"));
    service.addBook(Book("5", "Book5", "Author3", 2018, 1, "Category3"));
    
    auto authorStats = service.getAuthorStatistics();
    
    // Test author counts
    assert(authorStats["Author1"] == 3);
    assert(authorStats["Author2"] == 1);
    assert(authorStats["Author3"] == 1);
    assert(authorStats.size() == 3);
    
    std::cout << "✓ LibraryService author statistics test passed\n";
}

static void test_year_statistics() {
    std::cout << "Testing LibraryService year statistics...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add books with different years
    service.addBook(Book("1", "Book1", "Author1", 2020, 1, "Category1"));
    service.addBook(Book("2", "Book2", "Author2", 2020, 1, "Category1"));
    service.addBook(Book("3", "Book3", "Author3", 2019, 1, "Category2"));
    service.addBook(Book("4", "Book4", "Author4", 2021, 1, "Category1"));
    service.addBook(Book("5", "Book5", "Author5", 2018, 1, "Category3"));
    
    auto yearStats = service.getYearStatistics();
    
    // Test year counts
    assert(yearStats[2020] == 2);
    assert(yearStats[2019] == 1);
    assert(yearStats[2021] == 1);
    assert(yearStats[2018] == 1);
    assert(yearStats.size() == 4);
    
    std::cout << "✓ LibraryService year statistics test passed\n";
}

static void test_statistics_empty_library() {
    std::cout << "Testing LibraryService statistics with empty library...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    auto stats = service.getLibraryStatistics();
    
    // Test empty library statistics
    assert(stats.totalBooks == 0);
    assert(stats.totalQuantity == 0);
    assert(stats.uniqueAuthors == 0);
    assert(stats.totalCategories == 0);
    assert(stats.oldestYear == 0);
    assert(stats.newestYear == 0);
    assert(stats.averageBooksPerCategory == 0.0);
    assert(stats.averageQuantityPerBook == 0.0);
    assert(stats.mostPopularCategory.empty());
    assert(stats.categoryWithHighestQuantity.empty());
    assert(stats.mostProlificAuthor.empty());
    assert(stats.oldestBookTitle.empty());
    assert(stats.newestBookTitle.empty());
    
    auto authorStats = service.getAuthorStatistics();
    assert(authorStats.empty());
    
    auto yearStats = service.getYearStatistics();
    assert(yearStats.empty());
    
    std::cout << "✓ LibraryService statistics empty library test passed\n";
}

static void test_statistics_large_dataset() {
    std::cout << "Testing LibraryService statistics with large dataset...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add many books with patterns
    for (int i = 0; i < 100; ++i) {
        std::string isbn = std::to_string(1000000000 + i);
        std::string title = "Book " + std::to_string(i);
        std::string author = "Author " + std::to_string(i % 10); // 10 unique authors
        std::string category = "Category " + std::to_string(i % 5); // 5 categories
        int year = 2000 + (i % 25); // Years 2000-2024
        int quantity = 1 + (i % 10); // Quantities 1-10
        
        service.addBook(Book(isbn, title, author, year, quantity, category));
    }
    
    auto stats = service.getLibraryStatistics();
    
    // Test large dataset statistics
    assert(stats.totalBooks == 100);
    assert(stats.uniqueAuthors == 10);
    assert(stats.totalCategories == 5);
    assert(stats.oldestYear == 2000);
    assert(stats.newestYear == 2024);
    assert(std::abs(stats.averageBooksPerCategory - 20.0) < 0.1); // 100/5
    assert(stats.averageQuantityPerBook > 0);
    
    auto authorStats = service.getAuthorStatistics();
    assert(authorStats.size() == 10);
    
    auto yearStats = service.getYearStatistics();
    assert(yearStats.size() == 25); // 2000-2024
    
    std::cout << "✓ LibraryService statistics large dataset test passed\n";
}

static void test_statistics_edge_cases() {
    std::cout << "Testing LibraryService statistics edge cases...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
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
    assert(stats.oldestYear == 2020);
    assert(stats.newestYear == 2020);
    assert(stats.averageBooksPerCategory == 1.0);
    assert(stats.averageQuantityPerBook == 1.0);
    
    // Test with books having same year
    service.addBook(Book("2", "Book2", "Author2", 2020, 5, "Category2"));
    service.addBook(Book("3", "Book3", "Author3", 2020, 3, "Category2"));
    
    stats = service.getLibraryStatistics();
    assert(stats.totalBooks == 3);
    assert(stats.oldestYear == 2020);
    assert(stats.newestYear == 2020);
    assert(stats.totalCategories == 2);
    
    // Test with books having same author
    service.addBook(Book("4", "Book4", "Single Author", 2019, 2, "Category1"));
    service.addBook(Book("5", "Book5", "Single Author", 2021, 4, "Category3"));
    
    stats = service.getLibraryStatistics();
    assert(stats.uniqueAuthors == 3); // Single Author, Author2, Author3
    assert(stats.mostProlificAuthor == "Single Author"); // 3 books
    
    std::cout << "✓ LibraryService statistics edge cases test passed\n";
}

static void test_statistics_category_analytics() {
    std::cout << "Testing LibraryService category analytics...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add books with specific category patterns
    service.addBook(Book("1", "Book1", "Author1", 2020, 1, "Fiction"));
    service.addBook(Book("2", "Book2", "Author2", 2020, 1, "Fiction"));
    service.addBook(Book("3", "Book3", "Author3", 2020, 1, "Fiction"));
    service.addBook(Book("4", "Book4", "Author4", 2020, 1, "Science"));
    service.addBook(Book("5", "Book5", "Author5", 2020, 1, "Science"));
    service.addBook(Book("6", "Book6", "Author6", 2020, 10, "History")); // High quantity
    
    auto stats = service.getLibraryStatistics();
    
    // Test category analytics
    assert(stats.mostPopularCategory == "Fiction"); // 3 books
    assert(stats.categoryWithHighestQuantity == "History"); // 10 quantity
    assert(stats.totalCategories == 3);
    assert(std::abs(stats.averageBooksPerCategory - 2.0) < 0.1); // 6/3
    
    std::cout << "✓ LibraryService category analytics test passed\n";
}

static void test_statistics_author_analytics() {
    std::cout << "Testing LibraryService author analytics...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add books with specific author patterns
    service.addBook(Book("1", "Book1", "Prolific Author", 2020, 1, "Category1"));
    service.addBook(Book("2", "Book2", "Prolific Author", 2020, 1, "Category2"));
    service.addBook(Book("3", "Book3", "Prolific Author", 2020, 1, "Category3"));
    service.addBook(Book("4", "Book4", "Prolific Author", 2020, 1, "Category1"));
    service.addBook(Book("5", "Book5", "Single Author", 2020, 1, "Category1"));
    service.addBook(Book("6", "Book6", "Another Author", 2020, 1, "Category2"));
    
    auto stats = service.getLibraryStatistics();
    auto authorStats = service.getAuthorStatistics();
    
    // Test author analytics
    assert(stats.mostProlificAuthor == "Prolific Author"); // 4 books
    assert(stats.uniqueAuthors == 3);
    assert(authorStats["Prolific Author"] == 4);
    assert(authorStats["Single Author"] == 1);
    assert(authorStats["Another Author"] == 1);
    assert(authorStats.size() == 3);
    
    std::cout << "✓ LibraryService author analytics test passed\n";
}

static void test_statistics_year_analytics() {
    std::cout << "Testing LibraryService year analytics...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add books with specific year patterns
    service.addBook(Book("1", "Old Book", "Author1", 1990, 1, "Category1"));
    service.addBook(Book("2", "Recent Book", "Author2", 2024, 1, "Category1"));
    service.addBook(Book("3", "Middle Book", "Author3", 2010, 1, "Category1"));
    service.addBook(Book("4", "Another 2010", "Author4", 2010, 1, "Category2"));
    service.addBook(Book("5", "Yet Another 2010", "Author5", 2010, 1, "Category3"));
    
    auto stats = service.getLibraryStatistics();
    auto yearStats = service.getYearStatistics();
    
    // Test year analytics
    assert(stats.oldestYear == 1990);
    assert(stats.newestYear == 2024);
    assert(stats.oldestBookTitle == "Old Book");
    assert(stats.newestBookTitle == "Recent Book");
    assert(yearStats[2010] == 3); // Most common year
    assert(yearStats[1990] == 1);
    assert(yearStats[2024] == 1);
    assert(yearStats.size() == 3);
    
    std::cout << "✓ LibraryService year analytics test passed\n";
}

static void test_statistics_performance() {
    std::cout << "Testing LibraryService statistics performance...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add a large number of books for performance testing
    const int numBooks = 1000;
    for (int i = 0; i < numBooks; ++i) {
        std::string isbn = std::to_string(1000000000 + i);
        std::string title = "Performance Test Book " + std::to_string(i);
        std::string author = "Author " + std::to_string(i % 50); // 50 unique authors
        std::string category = "Category " + std::to_string(i % 20); // 20 categories
        int year = 1900 + (i % 125); // Years 1900-2024
        int quantity = 1 + (i % 20); // Quantities 1-20
        
        service.addBook(Book(isbn, title, author, year, quantity, category));
    }
    
    // Measure performance of statistics calculation
    auto start = std::chrono::high_resolution_clock::now();
    
    auto stats = service.getLibraryStatistics();
    auto authorStats = service.getAuthorStatistics();
    auto yearStats = service.getYearStatistics();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Verify results
    assert(stats.totalBooks == numBooks);
    assert(stats.uniqueAuthors == 50);
    assert(stats.totalCategories == 20);
    assert(stats.oldestYear == 1900);
    assert(stats.newestYear == 2024);
    assert(authorStats.size() == 50);
    assert(yearStats.size() == 125);
    
    // Performance should be reasonable (less than 1 second for 1000 books)
    assert(duration.count() < 1000);
    
    std::cout << "✓ LibraryService statistics performance test passed ("
              << duration.count() << "ms for " << numBooks << " books)\n";
}

static void test_statistics_boundary_values() {
    std::cout << "Testing LibraryService statistics boundary values...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Test with extreme values (within valid ranges)
    service.addBook(Book("1", "A", "A", 1000, 1, "A")); // Minimum valid year
    service.addBook(Book("2", std::string(100, 'B'), std::string(100, 'B'), 2024, 9999, std::string(100, 'B'))); // Maximum values
    
    auto stats = service.getLibraryStatistics();
    
    // Test boundary handling
    assert(stats.totalBooks == 2);
    assert(stats.uniqueAuthors == 2);
    assert(stats.totalCategories == 2);
    assert(stats.oldestYear == 1000);
    assert(stats.newestYear == 2024);
    assert(stats.totalQuantity == 10000); // 1 + 9999
    assert(stats.averageQuantityPerBook == 5000.0); // 10000/2
    
    std::cout << "✓ LibraryService statistics boundary values test passed\n";
}

static void test_statistics_special_characters() {
    std::cout << "Testing LibraryService statistics with special characters...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add books with special characters
    service.addBook(Book("1", "Book with émojis 🚀", "Author with ñ", 2020, 1, "Category with ü"));
    service.addBook(Book("2", "Book with symbols !@#$%", "Author with spaces", 2020, 1, "Category with numbers 123"));
    service.addBook(Book("3", "Book with \"quotes\"", "Author with 'apostrophes'", 2020, 1, "Category with (parentheses)"));
    
    auto stats = service.getLibraryStatistics();
    auto authorStats = service.getAuthorStatistics();
    
    // Test special character handling
    assert(stats.totalBooks == 3);
    assert(stats.uniqueAuthors == 3);
    assert(stats.totalCategories == 3);
    assert(authorStats["Author with ñ"] == 1);
    assert(authorStats["Author with spaces"] == 1);
    assert(authorStats["Author with 'apostrophes'"] == 1);
    
    std::cout << "✓ LibraryService statistics special characters test passed\n";
}

static void test_statistics_consistency() {
    std::cout << "Testing LibraryService statistics consistency...\n";
    
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.jsonl");
    LibraryService service(repo);
    
    // Add books and verify statistics remain consistent
    service.addBook(Book("1", "Book1", "Author1", 2020, 5, "Category1"));
    service.addBook(Book("2", "Book2", "Author1", 2020, 3, "Category1"));
    service.addBook(Book("3", "Book3", "Author2", 2019, 2, "Category2"));
    
    // Get statistics multiple times and verify consistency
    auto stats1 = service.getLibraryStatistics();
    auto stats2 = service.getLibraryStatistics();
    
    assert(stats1.totalBooks == stats2.totalBooks);
    assert(stats1.totalQuantity == stats2.totalQuantity);
    assert(stats1.uniqueAuthors == stats2.uniqueAuthors);
    assert(stats1.totalCategories == stats2.totalCategories);
    assert(stats1.mostPopularCategory == stats2.mostPopularCategory);
    assert(stats1.categoryWithHighestQuantity == stats2.categoryWithHighestQuantity);
    assert(stats1.mostProlificAuthor == stats2.mostProlificAuthor);
    assert(stats1.oldestYear == stats2.oldestYear);
    assert(stats1.newestYear == stats2.newestYear);
    assert(std::abs(stats1.averageBooksPerCategory - stats2.averageBooksPerCategory) < 0.001);
    assert(std::abs(stats1.averageQuantityPerBook - stats2.averageQuantityPerBook) < 0.001);
    
    // Verify author and year statistics consistency
    auto authorStats1 = service.getAuthorStatistics();
    auto authorStats2 = service.getAuthorStatistics();
    auto yearStats1 = service.getYearStatistics();
    auto yearStats2 = service.getYearStatistics();
    
    assert(authorStats1 == authorStats2);
    assert(yearStats1 == yearStats2);
    
    std::cout << "✓ LibraryService statistics consistency test passed\n";
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main() {
    std::cout << "Running BiblioTrack v2.09 Unit Tests...\n";
    std::cout << "==========================================\n\n";
    
    try {
        // Book Data Model Tests
        test_book_default_constructor();
        test_book_parameterized_constructor();
        test_book_default_category();
        test_book_setters();
        test_book_edge_cases();
        
        // Library Service Tests
        test_library_service_validation();
        test_library_service_search();
        test_library_service_category_management();
        
        // Book Repository Tests
        test_book_repository_basic_operations();
        test_book_repository_persistence();
        
        // Authentication Service Tests
        test_authentication_service_credentials();
        test_authentication_service_missing_file();
        
        // Edge Case Tests
        test_edge_cases_large_dataset();
        test_edge_cases_special_characters();
        test_edge_cases_boundary_values();
        
        // Statistics Tests (v2.08)
        test_library_statistics_basic();
        test_library_statistics_analytics();
        test_author_statistics();
        test_year_statistics();
        test_statistics_empty_library();
        test_statistics_large_dataset();
        
        // Advanced Statistics Tests (v2.08)
        test_statistics_edge_cases();
        test_statistics_category_analytics();
        test_statistics_author_analytics();
        test_statistics_year_analytics();
        test_statistics_performance();
        test_statistics_boundary_values();
        test_statistics_special_characters();
        test_statistics_consistency();
        
        std::cout << "\n🎉 All unit tests passed successfully!\n";
        std::cout << "Unit test coverage includes:\n";
        std::cout << "  - Book data model (constructors, getters, setters)\n";
        std::cout << "  - LibraryService validation and business logic\n";
        std::cout << "  - BookRepository persistence and operations\n";
        std::cout << "  - AuthenticationService credential management\n";
        std::cout << "  - Edge cases and boundary conditions\n";
        std::cout << "  - Special characters and large datasets\n";
        std::cout << "  - Statistics and analytics functionality (v2.08)\n";
        std::cout << "  - Advanced statistics edge cases and performance tests\n";
        std::cout << "  - Category, author, and year analytics validation\n";
        std::cout << "  - Boundary values and special character handling\n";
        std::cout << "  - Statistics consistency and reliability tests\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Unit test failed: " << e.what() << std::endl;
        return 1;
    }
}
