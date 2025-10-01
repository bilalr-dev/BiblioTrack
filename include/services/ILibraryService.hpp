#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include "datamodel/Book.hpp"

namespace LibraryApp {
namespace Services {

class ILibraryService {
public:
	virtual ~ILibraryService() = default;
	virtual std::optional<std::string> addBook(const DataModel::Book& book) = 0; // error message if any
	virtual std::optional<std::string> deleteBookByIsbn(const std::string& isbn) = 0;
	virtual std::vector<DataModel::Book> listBooks() = 0;
};

} // namespace Services
} // namespace LibraryApp

