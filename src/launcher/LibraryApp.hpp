#pragma once
#include "../services/LibraryService.hpp"
#include "../services/BookRepository.hpp"
#include "../datamodel/Book.hpp"
#include <memory>
#include <vector>

namespace Launcher {

class LibraryApp {
public:
    LibraryApp();
    void run();

private:
    std::shared_ptr<Services::BookRepository> repository_;
    std::shared_ptr<Services::LibraryService> service_;
    
    // Menu and UI methods
    void showMenu();
    int getChoice();
    void addBook();
    void deleteBook();
    void listBooks();
    void searchBooks();
    
    // Display helper
    void displayBooks(const std::vector<DataModel::Book>& books);
};

} // namespace Launcher
