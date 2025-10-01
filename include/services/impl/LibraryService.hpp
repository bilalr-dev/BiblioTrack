#pragma once

#include <memory>
#include "services/ILibraryService.hpp"
#include "services/IBookValidationService.hpp"
#include "repository/IBookRepository.hpp"

namespace LibraryApp {
namespace Services {

class LibraryService : public ILibraryService {
public:
	LibraryService(std::shared_ptr<Repository::IBookRepository> repo,
	               std::shared_ptr<IBookValidationService> validator)
		: repo_(std::move(repo)), validator_(std::move(validator)) {}

	std::optional<std::string> addBook(const DataModel::Book& book) override;
	std::optional<std::string> deleteBookByIsbn(const std::string& isbn) override;
	std::vector<DataModel::Book> listBooks() override;

private:
	std::shared_ptr<Repository::IBookRepository> repo_;
	std::shared_ptr<IBookValidationService> validator_;
};

} // namespace Services
} // namespace LibraryApp

