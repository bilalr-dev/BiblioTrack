#include "LibraryService.hpp"
#include <algorithm>
#include <cctype>
#include <map>
#include <ctime>
#include <unordered_set>
#include <limits>

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
    
    // Use optimized repository search with indexing - O(1) lookup
    return repository_->findByTitle(title);
}

std::vector<DataModel::Book> LibraryService::searchByAuthor(const std::string& author) {
    if (author.empty()) return {};
    
    // Use optimized repository search with indexing - O(1) lookup
    return repository_->findByAuthor(author);
}

std::optional<DataModel::Book> LibraryService::searchByIsbn(const std::string& isbn) {
    if (!isNumericString(isbn)) {
        return std::nullopt;
    }
    return repository_->findByIsbn(isbn);
}

std::vector<DataModel::Book> LibraryService::searchByCategory(const std::string& category) {
    if (category.empty()) return {};
    
    // Use optimized repository search with indexing - O(1) lookup
    return repository_->findByCategory(category);
}

std::vector<std::string> LibraryService::getAllCategories() {
    // Use cached statistics for O(1) access
    const auto& cachedStats = repository_->getCachedStatistics();
    
    std::vector<std::string> categories;
    categories.reserve(cachedStats.uniqueCategories.size());
    categories.assign(cachedStats.uniqueCategories.begin(), cachedStats.uniqueCategories.end());
    
    return categories;
}

std::map<std::string, int> LibraryService::getCategoryStatistics() {
    // Use cached statistics for O(1) access
    const auto& cachedStats = repository_->getCachedStatistics();
    return cachedStats.categoryCounts;
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

// ============================================================================
// STATISTICS AND ANALYTICS METHODS (v2.08)
// ============================================================================

LibraryService::LibraryStatistics LibraryService::getLibraryStatistics() {
    LibraryStatistics stats;
    
    // Use cached statistics for O(1) access instead of O(n) iteration
    const auto& cachedStats = repository_->getCachedStatistics();
    
    if (cachedStats.totalBooks == 0) {
        return stats; // Return default values for empty library
    }
    
    // Basic counts from cache
    stats.totalBooks = cachedStats.totalBooks;
    stats.totalQuantity = cachedStats.totalQuantity;
    stats.uniqueAuthors = static_cast<int>(cachedStats.uniqueAuthors.size());
    stats.totalCategories = static_cast<int>(cachedStats.uniqueCategories.size());
    
    // Calculate averages
    if (stats.totalCategories > 0) {
        stats.averageBooksPerCategory = static_cast<double>(stats.totalBooks) / stats.totalCategories;
    }
    if (stats.totalBooks > 0) {
        stats.averageQuantityPerBook = static_cast<double>(stats.totalQuantity) / stats.totalBooks;
    }
    
    // Find most popular category (by book count)
    if (!cachedStats.categoryCounts.empty()) {
        auto maxCategory = std::max_element(cachedStats.categoryCounts.begin(), cachedStats.categoryCounts.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        stats.mostPopularCategory = maxCategory->first;
    }
    
    // Find most prolific author
    if (!cachedStats.authorCounts.empty()) {
        auto maxAuthor = std::max_element(cachedStats.authorCounts.begin(), cachedStats.authorCounts.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        stats.mostProlificAuthor = maxAuthor->first;
    }
    
    // Find year range - still need to iterate for oldest/newest titles
    if (!cachedStats.yearCounts.empty()) {
        auto minYear = std::min_element(cachedStats.yearCounts.begin(), cachedStats.yearCounts.end(),
            [](const auto& a, const auto& b) { return a.first < b.first; });
        auto maxYear = std::max_element(cachedStats.yearCounts.begin(), cachedStats.yearCounts.end(),
            [](const auto& a, const auto& b) { return a.first < b.first; });
        
        stats.oldestYear = minYear->first;
        stats.newestYear = maxYear->first;
        
        // Find titles for oldest and newest years (minimal iteration)
        auto books = repository_->getAllBooks();
        for (const auto& book : books) {
            if (book.getYear() == stats.oldestYear && stats.oldestBookTitle.empty()) {
                stats.oldestBookTitle = book.getTitle();
            }
            if (book.getYear() == stats.newestYear && stats.newestBookTitle.empty()) {
                stats.newestBookTitle = book.getTitle();
            }
            if (!stats.oldestBookTitle.empty() && !stats.newestBookTitle.empty()) {
                break; // Found both, can exit early
            }
        }
    }
    
    // Find category with highest quantity - need to calculate from books
    if (!cachedStats.categoryCounts.empty()) {
        std::map<std::string, int> categoryQuantityCounts;
        auto books = repository_->getAllBooks();
        for (const auto& book : books) {
            categoryQuantityCounts[book.getCategory()] += book.getQuantity();
        }
        
        auto maxQuantityCategory = std::max_element(categoryQuantityCounts.begin(), categoryQuantityCounts.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        stats.categoryWithHighestQuantity = maxQuantityCategory->first;
    }
    
    return stats;
}

std::map<std::string, int> LibraryService::getAuthorStatistics() {
    // Use cached statistics for O(1) access
    const auto& cachedStats = repository_->getCachedStatistics();
    return cachedStats.authorCounts;
}

std::map<int, int> LibraryService::getYearStatistics() {
    // Use cached statistics for O(1) access
    const auto& cachedStats = repository_->getCachedStatistics();
    return cachedStats.yearCounts;
}

} // namespace Services

