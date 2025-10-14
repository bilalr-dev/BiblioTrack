#pragma once
#include "../services/LibraryService.hpp"
#include "../services/BookRepository.hpp"
#include "../services/AuthenticationService.hpp"
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
    std::shared_ptr<Services::AuthenticationService> authService_;
    
    // Menu and UI methods
    void showMenu();
    int getChoice();
    void addBook();
    void deleteBook();
    void listBooks();
    void searchBooks();
    void browseByCategory();
    
    // Display helpers
    void displayBooks(const std::vector<DataModel::Book>& books);
    void displayCategoryStatistics();
    void displayLibraryStatistics(); // v2.09
};

} // namespace Launcher

