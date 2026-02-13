#include "LibraryService.hpp"
#include <algorithm>
#include <cctype>
#include <map>
#include <ctime>
#include <unordered_set>

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
    if (book.getQuantity() < Constants::MIN_QUANTITY) {
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
    if (title.empty()) return {};
    
    std::vector<DataModel::Book> results;
    auto books = repository_->getAllBooks();
    
    // Pre-allocate results vector for better performance
    results.reserve(books.size() / Constants::ESTIMATED_SEARCH_MATCH_RATE);
    
    const std::string lowerTitle = toLowerCase(title);
    
    for (const auto& book : books) {
        const std::string& bookTitle = book.getTitle();
        if (bookTitle.size() >= title.size()) { // Quick size check
            std::string lowerBookTitle = toLowerCase(bookTitle);
            if (lowerBookTitle.find(lowerTitle) != std::string::npos) {
                results.push_back(book);
            }
        }
    }
    
    return results;
}

std::vector<DataModel::Book> LibraryService::searchByAuthor(const std::string& author) {
    if (author.empty()) return {};
    
    std::vector<DataModel::Book> results;
    auto books = repository_->getAllBooks();
    
    // Pre-allocate results vector for better performance
    results.reserve(books.size() / Constants::ESTIMATED_SEARCH_MATCH_RATE);
    
    const std::string lowerAuthor = toLowerCase(author);
    
    for (const auto& book : books) {
        const std::string& bookAuthor = book.getAuthor();
        if (bookAuthor.size() >= author.size()) { // Quick size check
            std::string lowerBookAuthor = toLowerCase(bookAuthor);
            if (lowerBookAuthor.find(lowerAuthor) != std::string::npos) {
                results.push_back(book);
            }
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
    if (category.empty()) return {};
    
    std::vector<DataModel::Book> results;
    auto books = repository_->getAllBooks();
    
    // Pre-allocate results vector for better performance
    results.reserve(books.size() / Constants::ESTIMATED_CATEGORY_MATCH_RATE);
    
    const std::string lowerCategory = toLowerCase(category);
    
    for (const auto& book : books) {
        const std::string& bookCategory = book.getCategory();
        if (bookCategory.size() >= category.size()) { // Quick size check
            std::string lowerBookCategory = toLowerCase(bookCategory);
            if (lowerBookCategory.find(lowerCategory) != std::string::npos) {
                results.push_back(book);
            }
        }
    }
    
    return results;
}

std::vector<std::string> LibraryService::getAllCategories() {
    std::unordered_set<std::string> categorySet;
    auto books = repository_->getAllBooks();
    
    // Reserve space for better performance
    categorySet.reserve(books.size() / Constants::ESTIMATED_CATEGORIES_PER_BOOK);
    
    for (const auto& book : books) {
        const std::string& category = book.getCategory();
        if (!category.empty()) {
            categorySet.insert(category);
        }
    }
    
    // Convert set to vector for return
    std::vector<std::string> categories;
    categories.reserve(categorySet.size());
    categories.assign(categorySet.begin(), categorySet.end());
    
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
    // Cache current year calculation for better performance
    static int currentYear = 0;
    if (currentYear == 0) {
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        currentYear = 1900 + tm.tm_year;
    }
    return year >= Constants::MIN_YEAR && year <= currentYear;
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
    if (input.empty()) return input;
    
    // Find first and last non-whitespace characters
    size_t start = input.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = input.find_last_not_of(" \t\n\r");
    
    // Pre-allocate result string with estimated size
    std::string result;
    result.reserve(end - start + 1);
    
    // Process characters in one pass
    bool inSpace = false;
    for (size_t i = start; i <= end; ++i) {
        char c = input[i];
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!inSpace) {
                result += ' ';
                inSpace = true;
            }
        } else {
            result += c;
            inSpace = false;
        }
    }
    
    return result;
}

} // namespace Services

