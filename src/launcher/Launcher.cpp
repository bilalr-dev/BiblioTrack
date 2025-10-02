#include "launcher/Launcher.hpp"
#include <iostream>
#include <iomanip>
#include <limits>

using namespace LibraryApp::Launcher;
using LibraryApp::DataModel::Book;

int AppLauncher::run() {
	while (true) {
		showMenu();
		std::cout << "Enter your choice: ";
		int choice = 0;
		if (!(std::cin >> choice)) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid input. Try again.\n";
			continue;
		}
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		switch (choice) {
			case 1: handleAddBook(); break;
			case 2: handleDeleteBook(); break;
			case 3: handleViewStock(); break;
			case 4: handleSearchBooks(); break;
			case 5: return 0;
			default: std::cout << "Invalid choice. Try again.\n"; break;
		}
	}
}

void AppLauncher::clearScreen() const {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

void AppLauncher::showMenu() const {
	std::cout << "\n=== Library Management System v1.1 ===\n";
	std::cout << "1. Add Book\n";
	std::cout << "2. Delete Book\n";
	std::cout << "3. View Current Stock\n";
	std::cout << "4. Search Books\n";
	std::cout << "5. Exit\n";
}

void AppLauncher::handleAddBook() {
	std::string isbn, title, author;
	unsigned int year = 0, qty = 0;
	std::cout << "ISBN: "; std::getline(std::cin, isbn);
	std::cout << "Title: "; std::getline(std::cin, title);
	std::cout << "Author: "; std::getline(std::cin, author);
	std::cout << "Year: "; std::cin >> year;
	std::cout << "Quantity: "; std::cin >> qty;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	Book b(isbn, title, author, static_cast<std::uint16_t>(year), static_cast<std::uint32_t>(qty));
	auto err = libraryService_->addBook(b);
	if (err) {
		std::cout << "Error: " << *err << "\n";
	} else {
		std::cout << "Book added successfully.\n";
	}
}

void AppLauncher::handleDeleteBook() {
	std::string isbn;
	std::cout << "Enter ISBN to delete: ";
	std::getline(std::cin, isbn);
	std::string confirm;
	std::cout << "Are you sure? (y/N): ";
	std::getline(std::cin, confirm);
	if (!(confirm == "y" || confirm == "Y" || confirm == "yes" || confirm == "YES")) {
		std::cout << "Cancelled.\n";
		return;
	}
	auto err = libraryService_->deleteBookByIsbn(isbn);
	if (err) {
		std::cout << "Error: " << *err << "\n";
	} else {
		std::cout << "Book deleted.\n";
	}
}

void AppLauncher::handleViewStock() {
	auto books = libraryService_->listBooks();
	displayBooks(books);
}

void AppLauncher::handleSearchBooks() {
	std::cout << "\n=== Search Books ===\n";
	std::cout << "1. Search by ISBN\n";
	std::cout << "2. Search by Title\n";
	std::cout << "3. Search by Author\n";
	std::cout << "4. Back to main menu\n";
	std::cout << "Enter your choice: ";
	
	int choice = 0;
	if (!(std::cin >> choice)) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Invalid input.\n";
		return;
	}
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	
	std::string searchTerm;
	std::vector<Book> results;
	
	switch (choice) {
		case 1: {
			std::cout << "Enter ISBN: ";
			std::getline(std::cin, searchTerm);
			auto book = searchService_->searchByIsbn(searchTerm);
			if (book) {
				results.push_back(*book);
			}
			break;
		}
		case 2: {
			std::cout << "Enter title (or part of title): ";
			std::getline(std::cin, searchTerm);
			results = searchService_->searchByTitle(searchTerm);
			break;
		}
		case 3: {
			std::cout << "Enter author name (or part of name): ";
			std::getline(std::cin, searchTerm);
			results = searchService_->searchByAuthor(searchTerm);
			break;
		}
		case 4:
			return;
		default:
			std::cout << "Invalid choice.\n";
			return;
	}
	
	std::cout << "\nSearch Results:\n";
	if (results.empty()) {
		std::cout << "No books found.\n";
	} else {
		displayBooks(results);
	}
}

void AppLauncher::displayBooks(const std::vector<Book>& books) const {
	if (books.empty()) {
		std::cout << "No books to display.\n";
		return;
	}
	
	// header
	std::cout << std::left
		<< std::setw(16) << "ISBN"
		<< std::setw(30) << "Title"
		<< std::setw(24) << "Author"
		<< std::setw(8) << "Year"
		<< std::setw(10) << "Quantity" << "\n";
	std::cout << std::string(16+30+24+8+10, '-') << "\n";
	for (const auto& b : books) {
		std::cout << std::left
			<< std::setw(16) << b.getIsbn().substr(0, 15)
			<< std::setw(30) << b.getTitle().substr(0, 29)
			<< std::setw(24) << b.getAuthor().substr(0, 23)
			<< std::setw(8) << b.getYear()
			<< std::setw(10) << b.getQuantity() << "\n";
	}
}