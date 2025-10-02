#include "LibraryApp.hpp"
#include <iostream>
#include <iomanip>
#include <limits>

namespace Launcher {

LibraryApp::LibraryApp() {
    repository_ = std::make_shared<Services::BookRepository>("data/books.csv");
    service_ = std::make_shared<Services::LibraryService>(repository_);
}

void LibraryApp::run() {
    std::cout << "=== BiblioTrack - Library Management System 2.01 ===\n";
    
    while (true) {
        showMenu();
        int choice = getChoice();
        
        switch (choice) {
            case 1: addBook(); break;
            case 2: deleteBook(); break;
            case 3: listBooks(); break;
            case 4: searchBooks(); break;
            case 5: 
                std::cout << "Goodbye!\n";
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

void LibraryApp::showMenu() {
    std::cout << "\n--- Main Menu ---\n";
    std::cout << "1. Add Book\n";
    std::cout << "2. Delete Book\n";
    std::cout << "3. List All Books\n";
    std::cout << "4. Search Books\n";
    std::cout << "5. Exit\n";
    std::cout << "Choose an option (1-5): ";
}

int LibraryApp::getChoice() {
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
}

void LibraryApp::addBook() {
    std::cout << "\n--- Add New Book ---\n";
    
    std::string isbn, title, author;
    int year, quantity;
    
    std::cout << "ISBN: ";
    std::getline(std::cin, isbn);
    std::cout << "Title: ";
    std::getline(std::cin, title);
    std::cout << "Author: ";
    std::getline(std::cin, author);
    std::cout << "Year: ";
    std::cin >> year;
    std::cout << "Quantity: ";
    std::cin >> quantity;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    DataModel::Book book(isbn, title, author, year, quantity);
    std::string result = service_->addBook(book);
    
    if (result.empty()) {
        std::cout << "✓ Book added successfully!\n";
    } else {
        std::cout << "✗ Error: " << result << "\n";
    }
}

void LibraryApp::deleteBook() {
    std::cout << "\n--- Delete Book ---\n";
    
    std::string isbn;
    std::cout << "Enter ISBN to delete: ";
    std::getline(std::cin, isbn);
    
    std::string confirm;
    std::cout << "Are you sure? (y/N): ";
    std::getline(std::cin, confirm);
    
    if (confirm == "y" || confirm == "Y") {
        std::string result = service_->deleteBook(isbn);
        if (result.empty()) {
            std::cout << "✓ Book deleted successfully!\n";
        } else {
            std::cout << "✗ Error: " << result << "\n";
        }
    } else {
        std::cout << "Delete cancelled.\n";
    }
}

void LibraryApp::listBooks() {
    std::cout << "\n--- All Books ---\n";
    auto books = service_->listBooks();
    displayBooks(books);
}

void LibraryApp::searchBooks() {
    std::cout << "\n--- Search Books ---\n";
    std::cout << "1. Search by ISBN\n";
    std::cout << "2. Search by Title\n";
    std::cout << "3. Search by Author\n";
    std::cout << "Choose search type (1-3): ";
    
    int choice = getChoice();
    std::string searchTerm;
    
    switch (choice) {
        case 1: {
            std::cout << "Enter ISBN: ";
            std::getline(std::cin, searchTerm);
            auto book = service_->searchByIsbn(searchTerm);
            if (book) {
                std::vector<DataModel::Book> results = {*book};
                displayBooks(results);
            } else {
                std::cout << "No book found with ISBN: " << searchTerm << "\n";
            }
            break;
        }
        case 2: {
            std::cout << "Enter title (or part of it): ";
            std::getline(std::cin, searchTerm);
            auto results = service_->searchByTitle(searchTerm);
            displayBooks(results);
            break;
        }
        case 3: {
            std::cout << "Enter author name (or part of it): ";
            std::getline(std::cin, searchTerm);
            auto results = service_->searchByAuthor(searchTerm);
            displayBooks(results);
            break;
        }
        default:
            std::cout << "Invalid search option.\n";
    }
}

void LibraryApp::displayBooks(const std::vector<DataModel::Book>& books) {
    if (books.empty()) {
        std::cout << "No books found.\n";
        return;
    }
    
    std::cout << "\n";
    std::cout << std::left << std::setw(15) << "ISBN"
              << std::setw(25) << "Title"
              << std::setw(20) << "Author"
              << std::setw(6) << "Year"
              << std::setw(8) << "Qty" << "\n";
    std::cout << std::string(74, '-') << "\n";
    
    for (const auto& book : books) {
        std::cout << std::left << std::setw(15) << book.getIsbn().substr(0, 14)
                  << std::setw(25) << book.getTitle().substr(0, 24)
                  << std::setw(20) << book.getAuthor().substr(0, 19)
                  << std::setw(6) << book.getYear()
                  << std::setw(8) << book.getQuantity() << "\n";
    }
    std::cout << "\nTotal: " << books.size() << " book(s)\n";
}

} // namespace Launcher
