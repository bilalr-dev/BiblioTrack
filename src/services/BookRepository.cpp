#include "BookRepository.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

namespace Services {

BookRepository::BookRepository(const std::string& jsonPath) : jsonPath_(jsonPath) {
    ensureFileExists();
    migrateCsvIfPresent();
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
        loadAllIntoCache();
    }
    return cache_;
}

std::optional<DataModel::Book> BookRepository::findByIsbn(const std::string& isbn) {
    if (!loaded_) {
        loadAllIntoCache();
    }
    auto it = isbnToIndex_.find(isbn);
    if (it == isbnToIndex_.end()) return std::nullopt;
    return cache_[it->second];
}

bool BookRepository::addBook(const DataModel::Book& book) {
    if (!loaded_) {
        loadAllIntoCache();
    }
    if (isbnToIndex_.count(book.getIsbn())) return false;
    cache_.push_back(book);
    isbnToIndex_[book.getIsbn()] = cache_.size() - 1;
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
        loadAllIntoCache();
    }
    auto it = isbnToIndex_.find(isbn);
    if (it == isbnToIndex_.end()) return false;
    size_t idx = it->second;
    // Erase from cache by swap-pop to keep O(1)
    size_t last = cache_.size() - 1;
    if (idx != last) {
        cache_[idx] = cache_[last];
        isbnToIndex_[cache_[idx].getIsbn()] = idx;
    }
    cache_.pop_back();
    isbnToIndex_.erase(it);
    // Rewrite entire file from cache to maintain correctness
    return writeAllFromCache();
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

void BookRepository::migrateCsvIfPresent() {
    // If legacy CSV exists and JSON file is empty, migrate
    std::string csvPath;
    // derive csv path by replacing extension if possible
    size_t dot = jsonPath_.rfind('.');
    if (dot != std::string::npos) {
        csvPath = jsonPath_.substr(0, dot) + ".csv";
    } else {
        csvPath = jsonPath_ + ".csv";
    }
    std::ifstream csv(csvPath);
    if (!csv.good()) return;

    // Check if JSON already has content
    std::ifstream jsonIn(jsonPath_);
    std::string existingLine;
    if (std::getline(jsonIn, existingLine) && !existingLine.empty()) {
        return; // do not overwrite existing JSON data
    }

    // Read CSV and append to JSON
    std::ofstream jsonOut(jsonPath_, std::ios::app);
    if (!jsonOut.is_open()) return;

    std::string line;
    // skip header if present
    if (std::getline(csv, line)) {
        if (line.find("ISBN") == std::string::npos) {
            // First line is a record; process it
            csv.seekg(0);
        }
    }

    while (std::getline(csv, line)) {
        if (line.empty()) continue;
        // simple CSV split (compatible with our previous buildLine/parseLine would be better, but avoid duplication)
        std::vector<std::string> fields;
        std::string cur;
        bool inQuotes = false;
        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (c == '"') {
                if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                    cur += '"';
                    ++i;
                } else {
                    inQuotes = !inQuotes;
                }
            } else if (c == ',' && !inQuotes) {
                fields.push_back(cur);
                cur.clear();
            } else {
                cur += c;
            }
        }
        fields.push_back(cur);
        if (fields.size() < 5) continue;

        std::string isbn = fields[0];
        std::string title = fields[1];
        std::string author = fields[2];
        int year = 0;
        int quantity = 0;
        try { year = std::stoi(fields[3]); } catch (...) { continue; }
        try { quantity = std::stoi(fields[4]); } catch (...) { continue; }
        std::string category = (fields.size() > 5 && !fields[5].empty()) ? fields[5] : "General";

        jsonOut << "{"
                << "\"isbn\":\"" << escapeJsonString(isbn) << "\","
                << "\"title\":\"" << escapeJsonString(title) << "\","
                << "\"author\":\"" << escapeJsonString(author) << "\","
                << "\"year\":" << year << ","
                << "\"quantity\":" << quantity << ","
                << "\"category\":\"" << escapeJsonString(category) << "\"" 
                << "}"
                << "\n";
    }
}

void BookRepository::loadAllIntoCache() {
    cache_.clear();
    isbnToIndex_.clear();
    std::ifstream file(jsonPath_);
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::string isbn, title, author, category;
        int year = 0;
        int quantity = 0;
        if (parseJsonLine(line, isbn, title, author, year, quantity, category)) {
            isbnToIndex_[isbn] = cache_.size();
            cache_.emplace_back(isbn, title, author, year, quantity, category);
        }
    }
    loaded_ = true;
}

bool BookRepository::writeAllFromCache() {
    std::ofstream file(jsonPath_);
    if (!file.is_open()) return false;
    for (const auto& book : cache_) {
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

} // namespace Services

