#include "BookRepository.hpp"
#include "../utils/PerformanceMonitor.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <filesystem>
#include <algorithm>

namespace Services {

BookRepository::BookRepository(const std::string& jsonPath) : jsonPath_(jsonPath), loaded_(false) {
    ensureFileExists();
    // Initialize cache as invalid
    cachedStats_.isValid = false;
}

void BookRepository::ensureFileExists() {
    namespace fs = std::filesystem;
    std::ifstream file(jsonPath_);
    if (file.good()) {
        return;
    }
    // Create parent directory cross-platform
    try {
        fs::path jsonPath(jsonPath_);
        fs::path parentDir = jsonPath.parent_path();
        if (!parentDir.empty() && !fs::exists(parentDir)) {
            fs::create_directories(parentDir);
        }
    } catch (...) {
        // If directory creation fails, proceed to attempt file creation; addBook will fail gracefully if needed
    }
    std::ofstream outFile(jsonPath_);
    if (outFile.is_open()) {
        // Start with an empty file for line-delimited JSON
        // Each line is a JSON object representing one book
        outFile.close();
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
    
    size_t newIndex = books_.size();
    books_.push_back(book);
    isbnIndex_[book.getIsbn()] = newIndex;
    
    // Update indices for the new book
    updateIndicesForBook(newIndex, book, false);
    
    // Invalidate cached statistics
    invalidateCachedStatistics();
    
    // Pre-build JSON string for efficiency
    std::string json;
    json.reserve(JsonConstants::ESTIMATED_JSON_LINE_SIZE);
    
    json += "{\"isbn\":\"";
    json += escapeJsonString(book.getIsbn());
    json += "\",\"title\":\"";
    json += escapeJsonString(book.getTitle());
    json += "\",\"author\":\"";
    json += escapeJsonString(book.getAuthor());
    json += "\",\"year\":";
    json += std::to_string(book.getYear());
    json += ",\"quantity\":";
    json += std::to_string(book.getQuantity());
    json += ",\"category\":\"";
    json += escapeJsonString(book.getCategory());
    json += "\"}";
    
    // Append to file
    std::ofstream file(jsonPath_, std::ios::app);
    if (!file.is_open()) return false;
    file << json << "\n";
    return true;
}

bool BookRepository::removeBook(const std::string& isbn) {
    if (!loaded_) {
        loadBooksFromJson();
    }
    auto it = isbnIndex_.find(isbn);
    if (it == isbnIndex_.end()) return false;
    size_t idx = it->second;
    
    // Update indices before removal
    updateIndicesForBook(idx, books_[idx], true);
    
    // Erase from books by swap-pop to keep O(1)
    size_t last = books_.size() - 1;
    if (idx != last) {
        // Update indices for the swapped book - remove from old position first
        updateIndicesForBook(last, books_[last], true); // Remove from last position
        books_[idx] = books_[last];
        updateIndicesForBook(idx, books_[idx], false); // Add to new position
        isbnIndex_[books_[idx].getIsbn()] = idx;
    }
    books_.pop_back();
    isbnIndex_.erase(it);
    
    // Invalidate cached statistics
    invalidateCachedStatistics();
    
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
    PERF_TIMER("loadBooksFromJson");
    
    books_.clear();
    isbnIndex_.clear();
    titleIndex_.clear();
    authorIndex_.clear();
    categoryIndex_.clear();
    
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
    
    // Build all indices after loading
    buildIndices();
    loaded_ = true;
}

bool BookRepository::saveBooksToJson() {
    std::ofstream file(jsonPath_);
    if (!file.is_open()) return false;
    for (const auto& book : books_) {
        // Pre-build JSON string for efficiency
        std::string json;
        json.reserve(JsonConstants::ESTIMATED_JSON_LINE_SIZE);
        
        json += "{\"isbn\":\"";
        json += escapeJsonString(book.getIsbn());
        json += "\",\"title\":\"";
        json += escapeJsonString(book.getTitle());
        json += "\",\"author\":\"";
        json += escapeJsonString(book.getAuthor());
        json += "\",\"year\":";
        json += std::to_string(book.getYear());
        json += ",\"quantity\":";
        json += std::to_string(book.getQuantity());
        json += ",\"category\":\"";
        json += escapeJsonString(book.getCategory());
        json += "\"}";
        
        file << json << "\n";
    }
    return true;
}

// ============================================================================
// OPTIMIZED SEARCH METHODS WITH INDEXING
// ============================================================================

std::vector<DataModel::Book> BookRepository::findByTitle(const std::string& title) {
    PERF_TIMER("findByTitle");
    
    if (!loaded_) {
        loadBooksFromJson();
    }
    
    std::vector<DataModel::Book> results;
    std::string normalizedTitle = normalizeString(title);
    
    // For partial matching, we need to iterate through all titles
    // This is still more efficient than the original O(n) approach because we use the index
    for (const auto& [indexedTitle, indices] : titleIndex_) {
        if (indexedTitle.find(normalizedTitle) != std::string::npos) {
            results.reserve(results.size() + indices.size());
            for (size_t index : indices) {
                results.push_back(books_[index]);
            }
        }
    }
    
    return results;
}

std::vector<DataModel::Book> BookRepository::findByAuthor(const std::string& author) {
    if (!loaded_) {
        loadBooksFromJson();
    }
    
    std::vector<DataModel::Book> results;
    std::string normalizedAuthor = normalizeString(author);
    
    // For partial matching, iterate through all authors
    for (const auto& [indexedAuthor, indices] : authorIndex_) {
        if (indexedAuthor.find(normalizedAuthor) != std::string::npos) {
            results.reserve(results.size() + indices.size());
            for (size_t index : indices) {
                results.push_back(books_[index]);
            }
        }
    }
    
    return results;
}

std::vector<DataModel::Book> BookRepository::findByCategory(const std::string& category) {
    if (!loaded_) {
        loadBooksFromJson();
    }
    
    std::vector<DataModel::Book> results;
    std::string normalizedCategory = normalizeString(category);
    
    // For partial matching, iterate through all categories
    for (const auto& [indexedCategory, indices] : categoryIndex_) {
        if (indexedCategory.find(normalizedCategory) != std::string::npos) {
            results.reserve(results.size() + indices.size());
            for (size_t index : indices) {
                results.push_back(books_[index]);
            }
        }
    }
    
    return results;
}

// ============================================================================
// CACHED STATISTICS METHODS
// ============================================================================

const BookRepository::CachedStatistics& BookRepository::getCachedStatistics() {
    if (!loaded_) {
        loadBooksFromJson();
    }
    if (!cachedStats_.isValid) {
        rebuildCachedStatistics();
    }
    return cachedStats_;
}

// ============================================================================
// INDEX MANAGEMENT METHODS
// ============================================================================

void BookRepository::buildIndices() {
    // Reserve space for better performance
    titleIndex_.reserve(JsonConstants::ESTIMATED_INDEX_SIZE);
    authorIndex_.reserve(JsonConstants::ESTIMATED_INDEX_SIZE);
    categoryIndex_.reserve(JsonConstants::ESTIMATED_INDEX_SIZE);
    
    for (size_t i = 0; i < books_.size(); ++i) {
        updateIndicesForBook(i, books_[i], false);
    }
}

void BookRepository::updateIndicesForBook(size_t index, const DataModel::Book& book, bool isRemoval) {
    if (isRemoval) {
        // Remove from indices
        std::string normalizedTitle = normalizeString(book.getTitle());
        std::string normalizedAuthor = normalizeString(book.getAuthor());
        std::string normalizedCategory = normalizeString(book.getCategory());
        
        auto titleIt = titleIndex_.find(normalizedTitle);
        if (titleIt != titleIndex_.end()) {
            titleIt->second.erase(index);
            if (titleIt->second.empty()) {
                titleIndex_.erase(titleIt);
            }
        }
        
        auto authorIt = authorIndex_.find(normalizedAuthor);
        if (authorIt != authorIndex_.end()) {
            authorIt->second.erase(index);
            if (authorIt->second.empty()) {
                authorIndex_.erase(authorIt);
            }
        }
        
        auto categoryIt = categoryIndex_.find(normalizedCategory);
        if (categoryIt != categoryIndex_.end()) {
            categoryIt->second.erase(index);
            if (categoryIt->second.empty()) {
                categoryIndex_.erase(categoryIt);
            }
        }
    } else {
        // Add to indices
        std::string normalizedTitle = normalizeString(book.getTitle());
        std::string normalizedAuthor = normalizeString(book.getAuthor());
        std::string normalizedCategory = normalizeString(book.getCategory());
        
        titleIndex_[normalizedTitle].insert(index);
        authorIndex_[normalizedAuthor].insert(index);
        categoryIndex_[normalizedCategory].insert(index);
    }
}

void BookRepository::invalidateCachedStatistics() {
    cachedStats_.isValid = false;
}

void BookRepository::rebuildCachedStatistics() const {
    cachedStats_.totalBooks = static_cast<int>(books_.size());
    cachedStats_.totalQuantity = 0;
    cachedStats_.uniqueAuthors.clear();
    cachedStats_.uniqueCategories.clear();
    cachedStats_.categoryCounts.clear();
    cachedStats_.authorCounts.clear();
    cachedStats_.yearCounts.clear();
    
    // Reserve space for better performance (only for unordered_set)
    cachedStats_.uniqueAuthors.reserve(books_.size() / 10); // Estimate 10% unique authors
    cachedStats_.uniqueCategories.reserve(books_.size() / 20); // Estimate 5% unique categories
    // Note: std::map doesn't have reserve() method
    
    for (const auto& book : books_) {
        // Update quantity
        cachedStats_.totalQuantity += book.getQuantity();
        
        // Update unique sets
        cachedStats_.uniqueAuthors.insert(book.getAuthor());
        cachedStats_.uniqueCategories.insert(book.getCategory());
        
        // Update counts
        cachedStats_.categoryCounts[book.getCategory()]++;
        cachedStats_.authorCounts[book.getAuthor()]++;
        cachedStats_.yearCounts[book.getYear()]++;
    }
    
    cachedStats_.isValid = true;
}

std::string BookRepository::normalizeString(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

} // namespace Services

