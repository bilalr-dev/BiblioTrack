#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include "datamodel/Book.hpp"

namespace LibraryApp {
namespace DataModel {

class Library {
public:
	Library() = default;

	bool hasBook(const std::string& isbn) const {
		return books_.find(isbn) != books_.end();
	}

	void addOrUpdateBook(const Book& book) {
		books_[book.getIsbn()] = book;
	}

	bool removeBook(const std::string& isbn) {
		return books_.erase(isbn) > 0;
	}

	std::vector<Book> listBooks() const {
		std::vector<Book> result;
		result.reserve(books_.size());
		for (const auto& kv : books_) {
			result.push_back(kv.second);
		}
		return result;
	}

private:
	std::unordered_map<std::string, Book> books_;
};

} // namespace DataModel
} // namespace LibraryApp

