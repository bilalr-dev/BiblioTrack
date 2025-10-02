#include "BookRepository.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace Services {

BookRepository::BookRepository(const std::string& csvPath) : csvPath_(csvPath) {
    ensureFileExists();
}

void BookRepository::ensureFileExists() {
    std::ifstream file(csvPath_);
    if (!file.good()) {
        // Create directory if it doesn't exist
        size_t lastSlash = csvPath_.find_last_of('/');
        if (lastSlash != std::string::npos) {
            std::string dir = csvPath_.substr(0, lastSlash);
            system(("mkdir -p " + dir).c_str());
        }
        
        std::ofstream outFile(csvPath_);
        if (outFile.is_open()) {
            outFile << "ISBN,Title,Author,Year,Quantity,Category\n";
            outFile.close();
        }
    }
}

std::vector<DataModel::Book> BookRepository::getAllBooks() {
    std::vector<DataModel::Book> books;
    std::ifstream file(csvPath_);
    std::string line;
    
    // Skip header
    if (std::getline(file, line)) {
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            auto fields = parseLine(line);
            if (fields.size() >= 5) {
                try {
                    int year = std::stoi(fields[3]);
                    int quantity = std::stoi(fields[4]);
                    std::string category = (fields.size() > 5) ? fields[5] : "General";
                    books.emplace_back(fields[0], fields[1], fields[2], year, quantity, category);
                } catch (const std::exception&) {
                    // Skip invalid lines
                    continue;
                }
            }
        }
    }
    return books;
}

std::optional<DataModel::Book> BookRepository::findByIsbn(const std::string& isbn) {
    auto books = getAllBooks();
    for (const auto& book : books) {
        if (book.getIsbn() == isbn) {
            return book;
        }
    }
    return std::nullopt;
}

bool BookRepository::addBook(const DataModel::Book& book) {
    // Check if book already exists
    auto existingBooks = getAllBooks();
    for (const auto& existingBook : existingBooks) {
        if (existingBook.getIsbn() == book.getIsbn()) {
            return false;
        }
    }
    
    std::ofstream file(csvPath_, std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    
    std::vector<std::string> fields = {
        book.getIsbn(),
        book.getTitle(),
        book.getAuthor(),
        std::to_string(book.getYear()),
        std::to_string(book.getQuantity()),
        book.getCategory()
    };
    
    file << buildLine(fields) << "\n";
    file.close();
    return true;
}

bool BookRepository::removeBook(const std::string& isbn) {
    auto books = getAllBooks();
    bool found = false;
    
    // Check if book exists
    for (const auto& book : books) {
        if (book.getIsbn() == isbn) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        return false;
    }
    
    // Rewrite file without the book
    std::ofstream file(csvPath_);
    if (!file.is_open()) {
        return false;
    }
    
    file << "ISBN,Title,Author,Year,Quantity,Category\n";
    for (const auto& book : books) {
        if (book.getIsbn() != isbn) {
            std::vector<std::string> fields = {
                book.getIsbn(),
                book.getTitle(),
                book.getAuthor(),
                std::to_string(book.getYear()),
                std::to_string(book.getQuantity()),
                book.getCategory()
            };
            file << buildLine(fields) << "\n";
        }
    }
    
    return true;
}

std::vector<std::string> BookRepository::parseLine(const std::string& line) {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string field;
    
    while (std::getline(ss, field, ',')) {
        result.push_back(field);
    }
    
    return result;
}

std::string BookRepository::buildLine(const std::vector<std::string>& fields) {
    std::string result;
    for (size_t i = 0; i < fields.size(); ++i) {
        result += fields[i];
        if (i < fields.size() - 1) {
            result += ",";
        }
    }
    return result;
}

} // namespace Services
