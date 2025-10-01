#pragma once

#include <vector>
#include <optional>
#include <string>
#include "datamodel/Book.hpp"

namespace LibraryApp {
namespace Repository {

class IBookRepository {
public:
	virtual ~IBookRepository() = default;

	virtual std::vector<DataModel::Book> findAll() = 0;
	virtual std::optional<DataModel::Book> findByIsbn(const std::string& isbn) = 0;
	virtual bool insert(const DataModel::Book& book) = 0; // false if exists
	virtual bool removeByIsbn(const std::string& isbn) = 0; // false if not found
};

} // namespace Repository
} // namespace LibraryApp

