#pragma once
#include "../datamodel/Book.hpp"
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>

namespace Services {

class BookRepository {
public:
    explicit BookRepository(const std::string& jsonPath);
    
    std::vector<DataModel::Book> getAllBooks();
    std::optional<DataModel::Book> findByIsbn(const std::string& isbn);
    bool addBook(const DataModel::Book& book);
    bool removeBook(const std::string& isbn);

private:
    std::string jsonPath_;
    bool loaded_ = false;
    std::vector<DataModel::Book> cache_;
    std::unordered_map<std::string, size_t> isbnToIndex_;
    
    // JSON helpers (line-delimited JSON objects for simplicity and robustness without external deps)
    static std::string escapeJsonString(const std::string& input);
    static bool parseJsonLine(const std::string& line,
                              std::string& isbn,
                              std::string& title,
                              std::string& author,
                              int& year,
                              int& quantity,
                              std::string& category);
    void ensureFileExists();
    void migrateCsvIfPresent();
    void loadAllIntoCache();
    bool writeAllFromCache();
};

} // namespace Services

