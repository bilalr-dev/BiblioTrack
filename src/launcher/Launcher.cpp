#include "launcher/Launcher.hpp"
#include <iostream>
#include <iomanip>

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
			case 4: return 0;
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
	std::cout << "\n=== Library Management System v1.0 ===\n";
	std::cout << "1. Add Book\n";
	std::cout << "2. Delete Book\n";
	std::cout << "3. View Current Stock\n";
	std::cout << "4. Exit\n";
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
	auto err = service_->addBook(b);
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
	auto err = service_->deleteBookByIsbn(isbn);
	if (err) {
		std::cout << "Error: " << *err << "\n";
	} else {
		std::cout << "Book deleted.\n";
	}
}

void AppLauncher::handleViewStock() {
	auto books = service_->listBooks();
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
