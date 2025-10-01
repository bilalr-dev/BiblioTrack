#include "services/impl/BookValidationService.hpp"

using namespace LibraryApp::Services;

std::optional<std::string> BookValidationService::validateIsbn(const std::string& isbn) const {
	if (isbn.empty()) return std::string("ISBN cannot be empty");
	if (isbn.size() > 64) return std::string("ISBN too long (max 64)");
	for (char c : isbn) {
		if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '-' )) {
			return std::string("ISBN may contain only letters, digits, or '-'");
		}
	}
	return std::nullopt;
}

std::optional<std::string> BookValidationService::validateTitle(const std::string& title) const {
	if (title.empty()) return std::string("Title cannot be empty");
	return std::nullopt;
}

std::optional<std::string> BookValidationService::validateAuthor(const std::string& author) const {
	if (author.empty()) return std::string("Author cannot be empty");
	return std::nullopt;
}

std::optional<std::string> BookValidationService::validateYear(std::uint16_t year) const {
	if (year < 1000 || year > 2025) return std::string("Year must be between 1000 and 2025");
	return std::nullopt;
}

std::optional<std::string> BookValidationService::validateQuantity(std::uint32_t quantity) const {
	if (quantity == 0) return std::string("Quantity must be a positive integer");
	return std::nullopt;
}
