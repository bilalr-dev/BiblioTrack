#include "services/impl/BookValidationService.hpp"

using namespace LibraryApp::Services;

std::optional<std::string> BookValidationService::validateIsbn(const std::string& isbn) const {
	if (isbn.empty()) return std::string("ISBN cannot be empty");
	// Basic length check; format rules can be expanded later
	if (isbn.size() > 32) return std::string("ISBN too long (max 32)");
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
	if (year < 1000 || year > 2024) return std::string("Year must be between 1000 and 2024");
	return std::nullopt;
}

std::optional<std::string> BookValidationService::validateQuantity(std::uint32_t quantity) const {
	if (quantity == 0) return std::string("Quantity must be a positive integer");
	return std::nullopt;
}
