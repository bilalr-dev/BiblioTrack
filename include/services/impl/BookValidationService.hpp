#pragma once

#include "services/IBookValidationService.hpp"

namespace LibraryApp {
namespace Services {

class BookValidationService : public IBookValidationService {
public:
	std::optional<std::string> validateIsbn(const std::string& isbn) const override;
	std::optional<std::string> validateTitle(const std::string& title) const override;
	std::optional<std::string> validateAuthor(const std::string& author) const override;
	std::optional<std::string> validateYear(std::uint16_t year) const override;
	std::optional<std::string> validateQuantity(std::uint32_t quantity) const override;
};

} // namespace Services
} // namespace LibraryApp

