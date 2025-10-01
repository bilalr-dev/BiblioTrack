#include <iostream>
#include <memory>
#include <string>

#include "repository/CsvBookRepository.hpp"
#include "services/impl/BookValidationService.hpp"
#include "services/impl/FileService.hpp"
#include "services/impl/LibraryService.hpp"
#include "launcher/Launcher.hpp"

int main() {
	const std::string csvPath = std::string(DATA_DIR) + "/books.csv";
	const std::string headers = std::string("ISBN,Title,Author,Year,Quantity");

	auto fileService = std::make_shared<LibraryApp::Services::FileService>();
	if (!fileService->ensureFileWithHeaders(csvPath, headers)) {
		std::cerr << "Error: cannot create or access CSV file at " << csvPath << "\n";
		return 1;
	}

	std::cout << "Data file: " << csvPath << "\n";

	auto repo = std::make_shared<LibraryApp::Repository::CsvBookRepository>(csvPath);
	auto validator = std::make_shared<LibraryApp::Services::BookValidationService>();
	auto libraryService = std::make_shared<LibraryApp::Services::LibraryService>(repo, validator);

	LibraryApp::Launcher::AppLauncher app(libraryService);
	return app.run();
}
