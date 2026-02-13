#include "../src/services/LibraryService.hpp"
#include "../src/services/BookRepository.hpp"
#include "../src/datamodel/Book.hpp"
#include <iostream>
#include <filesystem>
#include <cassert>

using Services::LibraryService;
using Services::BookRepository;

static void cleanTestData() {
    std::error_code ec;
    std::filesystem::remove("data/books.json", ec);
}

static void test_add_valid_then_duplicate() {
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService svc(repo);
    std::string err;

    DataModel::Book book1("978-0134685991", "Effective Modern C++", "Scott Meyers", 2014, 2, "Programming");
    err = svc.addBook(book1);
    assert(err.empty());
    DataModel::Book book2("9780134685991", "Duplicate", "X", 2015, 1, "Programming");
    err = svc.addBook(book2);
    assert(err == "Book with this ISBN already exists");
}

static void test_invalid_quantity_first() {
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService svc(repo);
    DataModel::Book book("0-395-19395-4", "Test 3", "Tester", 2000, -10, "");
    std::string err = svc.addBook(book);
    assert(err == "Quantity must be greater than 0");
}

static void test_delete_and_search() {
    cleanTestData();
    auto repo = std::make_shared<BookRepository>("data/books.json");
    LibraryService svc(repo);
    std::string err;
    DataModel::Book book("9780134685991", "EMC++", "Meyers", 2014, 1, "Programming");
    err = svc.addBook(book);
    assert(err.empty());
    auto found = svc.searchByIsbn("978-0-13-468599-1");
    assert(found.has_value());
    err = svc.deleteBook("978-0134685991");
    assert(err.empty());
    auto notFound = svc.searchByIsbn("9780134685991");
    assert(!notFound.has_value());
}

int main() {
    try {
        test_add_valid_then_duplicate();
        test_invalid_quantity_first();
        test_delete_and_search();
        std::cout << "All smoke tests passed.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << "\n";
        return 1;
    }
}


