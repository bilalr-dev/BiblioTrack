#include "services/impl/LibraryService.hpp"

using namespace LibraryApp::Services;
using LibraryApp::DataModel::Book;

std::optional<std::string> LibraryService::addBook(const Book& book) {
	if (auto e = validator_->validateIsbn(book.getIsbn())) return e;
	if (auto e = validator_->validateTitle(book.getTitle())) return e;
	if (auto e = validator_->validateAuthor(book.getAuthor())) return e;
	if (auto e = validator_->validateYear(book.getYear())) return e;
	if (auto e = validator_->validateQuantity(book.getQuantity())) return e;

	if (repo_->findByIsbn(book.getIsbn())) {
		return std::string("ISBN already exists");
	}
	bool ok = repo_->insert(book);
	if (!ok) return std::string("Failed to persist book (permission or IO error)");
	return std::nullopt;
}

std::optional<std::string> LibraryService::deleteBookByIsbn(const std::string& isbn) {
	if (isbn.empty()) return std::string("ISBN cannot be empty");
	bool ok = repo_->removeByIsbn(isbn);
	if (!ok) return std::string("No book found with given ISBN");
	return std::nullopt;
}

std::vector<Book> LibraryService::listBooks() {
	return repo_->findAll();
}
