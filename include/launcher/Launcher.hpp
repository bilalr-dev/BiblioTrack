#pragma once

#include <memory>
#include "services/ILibraryService.hpp"

namespace LibraryApp {
namespace Launcher {

class AppLauncher {
public:
	explicit AppLauncher(std::shared_ptr<Services::ILibraryService> service)
		: service_(std::move(service)) {}

	int run();

private:
	std::shared_ptr<Services::ILibraryService> service_;

	void clearScreen() const;
	void showMenu() const;
	void handleAddBook();
	void handleDeleteBook();
	void handleViewStock();
};

} // namespace Launcher
} // namespace LibraryApp

