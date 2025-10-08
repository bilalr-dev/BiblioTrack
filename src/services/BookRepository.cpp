#include "BookRepository.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

namespace Services {

BookRepository::BookRepository(const std::string& jsonPath) : jsonPath_(jsonPath), loaded_(false) {
    ensureFileExists();
}

void BookRepository::ensureFileExists() {
    std::ifstream file(jsonPath_);
    if (!file.good()) {
        // Create directory if it doesn't exist
        size_t lastSlash = jsonPath_.find_last_of('/');
        if (lastSlash != std::string::npos) {
            std::string dir = jsonPath_.substr(0, lastSlash);
            system(("mkdir -p " + dir).c_str());
        }
        
        std::ofstream outFile(jsonPath_);
        if (outFile.is_open()) {
            // Start with an empty file for line-delimited JSON
            // Each line is a JSON object representing one book
            outFile.close();
        }
    }
}

std::vector<DataModel::Book> BookRepository::getAllBooks() {
    if (!loaded_) {
        loadBooksFromJson();
    }
    return books_;
}

std::optional<DataModel::Book> BookRepository::findByIsbn(const std::string& isbn) {
    if (!loaded_) {
        loadBooksFromJson();
    }
    auto it = isbnIndex_.find(isbn);
    if (it == isbnIndex_.end()) return std::nullopt;
    return books_[it->second];
}

bool BookRepository::addBook(const DataModel::Book& book) {
    if (!loaded_) {
        loadBooksFromJson();
    }
    if (isbnIndex_.count(book.getIsbn())) return false;
    books_.push_back(book);
    isbnIndex_[book.getIsbn()] = books_.size() - 1;
    // Append to file for efficiency
    std::ofstream file(jsonPath_, std::ios::app);
    if (!file.is_open()) return false;
    file << "{"
         << "\"isbn\":\"" << escapeJsonString(book.getIsbn()) << "\"," 
         << "\"title\":\"" << escapeJsonString(book.getTitle()) << "\"," 
         << "\"author\":\"" << escapeJsonString(book.getAuthor()) << "\"," 
         << "\"year\":" << book.getYear() << ","
         << "\"quantity\":" << book.getQuantity() << ","
         << "\"category\":\"" << escapeJsonString(book.getCategory()) << "\"" 
         << "}"
         << "\n";
    return true;
}

bool BookRepository::removeBook(const std::string& isbn) {
    if (!loaded_) {
        loadBooksFromJson();
    }
    auto it = isbnIndex_.find(isbn);
    if (it == isbnIndex_.end()) return false;
    size_t idx = it->second;
    // Erase from books by swap-pop to keep O(1)
    size_t last = books_.size() - 1;
    if (idx != last) {
        books_[idx] = books_[last];
        isbnIndex_[books_[idx].getIsbn()] = idx;
    }
    books_.pop_back();
    isbnIndex_.erase(it);
    // Rewrite entire file from books to maintain correctness
    return saveBooksToJson();
}

// Escape a string for safe inclusion in JSON string value
std::string BookRepository::escapeJsonString(const std::string& input) {
    std::string out;
    out.reserve(input.size());
    for (char c : input) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    // Control characters -> escape as \u00XX
                    const char* hex = "0123456789abcdef";
                    out += "\\u00";
                    out += hex[(c >> 4) & 0x0F];
                    out += hex[c & 0x0F];
                } else {
                    out += c;
                }
        }
    }
    return out;
}

// Very simple JSON line parser for flat objects with known keys
static std::string extractJsonString(const std::string& src, const std::string& key) {
    std::string needle = "\"" + key + "\"";
    size_t pos = src.find(needle);
    if (pos == std::string::npos) return {};
    pos = src.find(':', pos);
    if (pos == std::string::npos) return {};
    pos = src.find('"', pos);
    if (pos == std::string::npos) return {};
    size_t end = pos + 1;
    std::string value;
    bool escape = false;
    for (; end < src.size(); ++end) {
        char c = src[end];
        if (escape) {
            switch (c) {
                case '"': value += '"'; break;
                case '\\': value += '\\'; break;
                case 'b': value += '\b'; break;
                case 'f': value += '\f'; break;
                case 'n': value += '\n'; break;
                case 'r': value += '\r'; break;
                case 't': value += '\t'; break;
                case 'u': {
                    // naive: skip unicode sequences
                    // ensure not to read beyond bounds
                    if (end + 4 < src.size()) end += 4;
                    break;
                }
                default: value += c; break;
            }
            escape = false;
        } else if (c == '\\') {
            escape = true;
        } else if (c == '"') {
            break;
        } else {
            value += c;
        }
    }
    return value;
}

static bool extractJsonInt(const std::string& src, const std::string& key, int& out) {
    std::string needle = "\"" + key + "\"";
    size_t pos = src.find(needle);
    if (pos == std::string::npos) return false;
    pos = src.find(':', pos);
    if (pos == std::string::npos) return false;
    // skip whitespace
    ++pos;
    while (pos < src.size() && std::isspace(static_cast<unsigned char>(src[pos]))) ++pos;
    size_t end = pos;
    while (end < src.size() && (std::isdigit(static_cast<unsigned char>(src[end])) || src[end] == '-')) ++end;
    if (end == pos) return false;
    try {
        out = std::stoi(src.substr(pos, end - pos));
        return true;
    } catch (...) {
        return false;
    }
}

bool BookRepository::parseJsonLine(const std::string& line,
                                   std::string& isbn,
                                   std::string& title,
                                   std::string& author,
                                   int& year,
                                   int& quantity,
                                   std::string& category) {
    isbn = extractJsonString(line, "isbn");
    title = extractJsonString(line, "title");
    author = extractJsonString(line, "author");
    category = extractJsonString(line, "category");
    bool okYear = extractJsonInt(line, "year", year);
    bool okQty = extractJsonInt(line, "quantity", quantity);
    if (category.empty()) category = "General";
    return !isbn.empty() && !title.empty() && !author.empty() && okYear && okQty;
}

void BookRepository::loadBooksFromJson() {
    books_.clear();
    isbnIndex_.clear();
    std::ifstream file(jsonPath_);
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::string isbn, title, author, category;
        int year = 0;
        int quantity = 0;
        if (parseJsonLine(line, isbn, title, author, year, quantity, category)) {
            isbnIndex_[isbn] = books_.size();
            books_.emplace_back(isbn, title, author, year, quantity, category);
        }
    }
    loaded_ = true;
}

bool BookRepository::saveBooksToJson() {
    std::ofstream file(jsonPath_);
    if (!file.is_open()) return false;
    for (const auto& book : books_) {
        file << "{"
             << "\"isbn\":\"" << escapeJsonString(book.getIsbn()) << "\"," 
             << "\"title\":\"" << escapeJsonString(book.getTitle()) << "\"," 
             << "\"author\":\"" << escapeJsonString(book.getAuthor()) << "\"," 
             << "\"year\":" << book.getYear() << ","
             << "\"quantity\":" << book.getQuantity() << ","
             << "\"category\":\"" << escapeJsonString(book.getCategory()) << "\"" 
             << "}"
             << "\n";
    }
    return true;
}

std::string BookRepository::buildJsonLine(const DataModel::Book& book) {
    std::stringstream json;
    json << "{"
         << "\"isbn\":\"" << escapeJsonString(book.getIsbn()) << "\","
         << "\"title\":\"" << escapeJsonString(book.getTitle()) << "\","
         << "\"author\":\"" << escapeJsonString(book.getAuthor()) << "\","
         << "\"year\":" << book.getYear() << ","
         << "\"quantity\":" << book.getQuantity() << ","
         << "\"category\":\"" << escapeJsonString(book.getCategory()) << "\""
         << "}";
    return json.str();
}

} // namespace Services

