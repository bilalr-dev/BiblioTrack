#include "LibraryService.hpp"
#include <algorithm>
#include <cctype>
#include <map>

namespace Services {

LibraryService::LibraryService(std::shared_ptr<BookRepository> repository) 
    : repository_(repository) {}

std::string LibraryService::addBook(const DataModel::Book& book) {
    // Basic validation
    if (book.getIsbn().empty()) {
        return "ISBN cannot be empty";
    }
    if (!isNumericString(book.getIsbn())) {
        return "ISBN must contain digits only";
    }
    if (book.getTitle().empty()) {
        return "Title cannot be empty";
    }
    if (book.getAuthor().empty()) {
        return "Author cannot be empty";
    }
    if (!isValidYear(book.getYear())) {
        return "Invalid year (must be between 1000 and current year)";
    }
    if (book.getQuantity() <= 0) {
        return "Quantity must be greater than 0";
    }
    
    // Create sanitized book with cleaned input
    DataModel::Book sanitizedBook(
        sanitizeInput(book.getIsbn()),
        sanitizeInput(book.getTitle()),
        sanitizeInput(book.getAuthor()),
        book.getYear(),
        book.getQuantity(),
        sanitizeInput(book.getCategory())
    );
    
    if (repository_->addBook(sanitizedBook)) {
        return ""; // Success
    } else {
        return "Book with this ISBN already exists";
    }
}

std::string LibraryService::deleteBook(const std::string& isbn) {
    if (isbn.empty()) {
        return "ISBN cannot be empty";
    }
    if (!isNumericString(isbn)) {
        return "ISBN must contain digits only";
    }
    
    if (repository_->removeBook(isbn)) {
        return ""; // Success
    } else {
        return "Book not found";
    }
}

std::vector<DataModel::Book> LibraryService::listBooks() {
    return repository_->getAllBooks();
}

std::vector<DataModel::Book> LibraryService::searchByTitle(const std::string& title) {
    std::vector<DataModel::Book> results;
    auto books = repository_->getAllBooks();
    std::string lowerTitle = toLowerCase(title);
    
    for (const auto& book : books) {
        std::string bookTitle = toLowerCase(book.getTitle());
        if (bookTitle.find(lowerTitle) != std::string::npos) {
            results.push_back(book);
        }
    }
    
    return results;
}

std::vector<DataModel::Book> LibraryService::searchByAuthor(const std::string& author) {
    std::vector<DataModel::Book> results;
    auto books = repository_->getAllBooks();
    std::string lowerAuthor = toLowerCase(author);
    
    for (const auto& book : books) {
        std::string bookAuthor = toLowerCase(book.getAuthor());
        if (bookAuthor.find(lowerAuthor) != std::string::npos) {
            results.push_back(book);
        }
    }
    
    return results;
}

std::optional<DataModel::Book> LibraryService::searchByIsbn(const std::string& isbn) {
    if (!isNumericString(isbn)) {
        return std::nullopt;
    }
    return repository_->findByIsbn(isbn);
}

std::vector<DataModel::Book> LibraryService::searchByCategory(const std::string& category) {
    std::vector<DataModel::Book> results;
    auto books = repository_->getAllBooks();
    std::string lowerCategory = toLowerCase(category);
    
    for (const auto& book : books) {
        std::string bookCategory = toLowerCase(book.getCategory());
        if (bookCategory.find(lowerCategory) != std::string::npos) {
            results.push_back(book);
        }
    }
    
    return results;
}

std::vector<std::string> LibraryService::getAllCategories() {
    std::vector<std::string> categories;
    auto books = repository_->getAllBooks();
    
    for (const auto& book : books) {
        std::string category = book.getCategory();
        bool exists = false;
        for (const auto& existingCategory : categories) {
            if (existingCategory == category) {
                exists = true;
                break;
            }
        }
        if (!exists && !category.empty()) {
            categories.push_back(category);
        }
    }
    
    return categories;
}

std::map<std::string, int> LibraryService::getCategoryStatistics() {
    std::map<std::string, int> stats;
    auto books = repository_->getAllBooks();
    
    for (const auto& book : books) {
        std::string category = book.getCategory();
        if (category.empty()) {
            category = "General";
        }
        stats[category]++;
    }
    
    return stats;
}

bool LibraryService::isValidYear(int year) {
    return year >= 1000 && year <= 2030; // Simple range check
}

bool LibraryService::isNumericString(const std::string& value) {
    if (value.empty()) return false;
    for (char c : value) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

std::string LibraryService::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string LibraryService::sanitizeInput(const std::string& input) {
    std::string result = input;
    
    // Trim leading and trailing whitespace
    size_t start = result.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = result.find_last_not_of(" \t\n\r");
    result = result.substr(start, end - start + 1);
    
    // Remove excessive whitespace (replace multiple spaces with single space)
    std::string cleaned;
    bool inSpace = false;
    for (char c : result) {
        if (std::isspace(c)) {
            if (!inSpace) {
                cleaned += ' ';
                inSpace = true;
            }
        } else {
            cleaned += c;
            inSpace = false;
        }
    }
    
    return cleaned;
}

} // namespace Services

