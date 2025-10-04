#pragma once
#include "BookRepository.hpp"
#include "../datamodel/Book.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace Services {

class LibraryService {
public:
    explicit LibraryService(std::shared_ptr<BookRepository> repository);
    
    // Book management operations
    std::string addBook(const DataModel::Book& book);
    std::string deleteBook(const std::string& isbn);
    std::vector<DataModel::Book> listBooks();
    
    // Search operations
    std::vector<DataModel::Book> searchByTitle(const std::string& title);
    std::vector<DataModel::Book> searchByAuthor(const std::string& author);
    std::optional<DataModel::Book> searchByIsbn(const std::string& isbn);
    std::vector<DataModel::Book> searchByCategory(const std::string& category);
    
    // Category management
    std::vector<std::string> getAllCategories();
    std::map<std::string, int> getCategoryStatistics();

private:
    std::shared_ptr<BookRepository> repository_;
    
    // Validation methods
    bool isValidYear(int year);
    std::string toLowerCase(const std::string& str);
    std::string sanitizeInput(const std::string& input);
};

} // namespace Services

