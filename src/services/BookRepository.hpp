#pragma once
#include "../datamodel/Book.hpp"
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <algorithm>

namespace Services {

// Constants for JSON operations and performance optimization
namespace JsonConstants {
    constexpr size_t ESTIMATED_JSON_LINE_SIZE = 200;
    constexpr size_t BUFFER_RESERVE_SIZE = 1000;
    constexpr size_t ESTIMATED_INDEX_SIZE = 100;
}

// Optimized BookRepository with advanced indexing for O(1) lookups
class BookRepository {
public:
    explicit BookRepository(const std::string& jsonPath);
    
    std::vector<DataModel::Book> getAllBooks();
    std::optional<DataModel::Book> findByIsbn(const std::string& isbn);
    bool addBook(const DataModel::Book& book);
    bool removeBook(const std::string& isbn);
    
    // Optimized search methods with indexing
    std::vector<DataModel::Book> findByTitle(const std::string& title);
    std::vector<DataModel::Book> findByAuthor(const std::string& author);
    std::vector<DataModel::Book> findByCategory(const std::string& category);
    
    // Cached statistics for O(1) access
    struct CachedStatistics {
        int totalBooks = 0;
        int totalQuantity = 0;
        std::unordered_set<std::string> uniqueAuthors;
        std::unordered_set<std::string> uniqueCategories;
        std::map<std::string, int> categoryCounts;
        std::map<std::string, int> authorCounts;
        std::map<int, int> yearCounts;
        bool isValid = false;
    };
    
    const CachedStatistics& getCachedStatistics();

private:
    std::string jsonPath_;
    std::vector<DataModel::Book> books_;
    
    // Advanced indexing for O(1) lookups
    std::unordered_map<std::string, size_t> isbnIndex_;
    std::unordered_map<std::string, std::unordered_set<size_t>> titleIndex_;
    std::unordered_map<std::string, std::unordered_set<size_t>> authorIndex_;
    std::unordered_map<std::string, std::unordered_set<size_t>> categoryIndex_;
    
    // Cached statistics
    mutable CachedStatistics cachedStats_;
    bool loaded_;
    
    void ensureFileExists();
    void loadBooksFromJson();
    bool saveBooksToJson();
    bool parseJsonLine(const std::string& line, std::string& isbn, std::string& title, std::string& author, int& year, int& quantity, std::string& category);
    std::string escapeJsonString(const std::string& input);
    
    // Index management methods
    void buildIndices();
    void updateIndicesForBook(size_t index, const DataModel::Book& book, bool isRemoval = false);
    void invalidateCachedStatistics();
    void rebuildCachedStatistics() const;
    
    // String normalization for case-insensitive indexing
    std::string normalizeString(const std::string& str) const;
};

} // namespace Services

