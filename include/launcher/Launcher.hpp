#pragma once

#include <memory>
#include "services/ILibraryService.hpp"
#include "services/ISearchService.hpp"

namespace LibraryApp {
namespace Launcher {

class AppLauncher {
public:
	AppLauncher(std::shared_ptr<Services::ILibraryService> libraryService,
	           std::shared_ptr<Services::ISearchService> searchService)
		: libraryService_(std::move(libraryService)), 
		  searchService_(std::move(searchService)) {}

	int run();

private:
	std::shared_ptr<Services::ILibraryService> libraryService_;
	std::shared_ptr<Services::ISearchService> searchService_;

	void clearScreen() const;
	void showMenu() const;
	void handleAddBook();
	void handleDeleteBook();
	void handleViewStock();
	void handleSearchBooks();
	
	// Helper methods for display
	void displayBooks(const std::vector<DataModel::Book>& books) const;
};

} // namespace Launcher
} // namespace LibraryApp

