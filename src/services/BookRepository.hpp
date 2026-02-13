#pragma once
#include "../datamodel/Book.hpp"
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>

namespace Services {

// Constants for JSON operations
namespace JsonConstants {
    constexpr size_t ESTIMATED_JSON_LINE_SIZE = 200;
    constexpr size_t BUFFER_RESERVE_SIZE = 1000;
}

class BookRepository {
public:
    explicit BookRepository(const std::string& jsonPath);
    
    std::vector<DataModel::Book> getAllBooks();
    std::optional<DataModel::Book> findByIsbn(const std::string& isbn);
    bool addBook(const DataModel::Book& book);
    bool removeBook(const std::string& isbn);

private:
    std::string jsonPath_;
    std::vector<DataModel::Book> books_;
    std::unordered_map<std::string, size_t> isbnIndex_;
    bool loaded_;
    
    void ensureFileExists();
    void loadBooksFromJson();
    bool saveBooksToJson();
    bool parseJsonLine(const std::string& line, std::string& isbn, std::string& title, std::string& author, int& year, int& quantity, std::string& category);
    std::string escapeJsonString(const std::string& input);
};

} // namespace Services

