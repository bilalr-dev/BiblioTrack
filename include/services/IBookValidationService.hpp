#pragma once

#include <string>
#include <optional>
#include <cstdint>

namespace LibraryApp {
namespace Services {

class IBookValidationService {
public:
	virtual ~IBookValidationService() = default;
	virtual std::optional<std::string> validateIsbn(const std::string& isbn) const = 0;
	virtual std::optional<std::string> validateTitle(const std::string& title) const = 0;
	virtual std::optional<std::string> validateAuthor(const std::string& author) const = 0;
	virtual std::optional<std::string> validateYear(std::uint16_t year) const = 0;
	virtual std::optional<std::string> validateQuantity(std::uint32_t quantity) const = 0;
};

} // namespace Services
} // namespace LibraryApp

