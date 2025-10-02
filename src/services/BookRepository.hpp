#pragma once
#include "../datamodel/Book.hpp"
#include <vector>
#include <string>
#include <optional>

namespace Services {

class BookRepository {
public:
    explicit BookRepository(const std::string& csvPath);
    
    std::vector<DataModel::Book> getAllBooks();
    std::optional<DataModel::Book> findByIsbn(const std::string& isbn);
    bool addBook(const DataModel::Book& book);
    bool removeBook(const std::string& isbn);

private:
    std::string csvPath_;
    
    std::vector<std::string> parseLine(const std::string& line);
    std::string buildLine(const std::vector<std::string>& fields);
    void ensureFileExists();
};

} // namespace Services
