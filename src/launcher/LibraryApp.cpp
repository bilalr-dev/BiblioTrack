#include "LibraryApp.hpp"
#include <iostream>
#include <iomanip>
#include <limits>

namespace Launcher {

LibraryApp::LibraryApp() {
    repository_ = std::make_shared<Services::BookRepository>("data/books.jsonl");
    service_ = std::make_shared<Services::LibraryService>(repository_);
    authService_ = std::make_shared<Services::AuthenticationService>();
}

void LibraryApp::run() {
    std::cout << "=== BiblioTrack - Library Management System 2.09 ===\n";
    
    // Authenticate user before allowing access
    if (!authService_->runAuthenticationFlow()) {
        return; // User chose to exit
    }
    
    while (true) {
        showMenu();
        int choice = getChoice();
        
        switch (choice) {
            case 1: addBook(); break;
            case 2: deleteBook(); break;
            case 3: listBooks(); break;
            case 4: searchBooks(); break;
            case 5: browseByCategory(); break;
            case 6: displayLibraryStatistics(); break;
            case 7: 
                std::cout << "Goodbye!\n";
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

void LibraryApp::showMenu() {
    std::cout << "\n--- Main Menu ---\n";
    std::cout << "1. Add Book\n";
    std::cout << "2. Delete Book\n";
    std::cout << "3. List All Books\n";
    std::cout << "4. Search Books\n";
    std::cout << "5. Browse by Category\n";
    std::cout << "6. View Statistics\n";
    std::cout << "7. Exit\n";
    std::cout << "Choose an option (1-7): ";
}

int LibraryApp::getChoice() {
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
}

void LibraryApp::addBook() {
    std::cout << "\n--- Add New Book ---\n";
    
    std::string isbn, title, author, category;
    int year, quantity;
    
    std::cout << "ISBN: ";
    std::getline(std::cin, isbn);
    std::cout << "Title: ";
    std::getline(std::cin, title);
    std::cout << "Author: ";
    std::getline(std::cin, author);
    std::cout << "Year: ";
    std::cin >> year;
    std::cout << "Quantity: ";
    std::cin >> quantity;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    // Show existing categories as suggestions
    auto categories = service_->getAllCategories();
    if (!categories.empty()) {
        std::cout << "Existing categories: ";
        for (size_t i = 0; i < categories.size(); ++i) {
            std::cout << categories[i];
            if (i < categories.size() - 1) std::cout << ", ";
        }
        std::cout << "\n";
    }
    
    std::cout << "Category [General]: ";
    std::getline(std::cin, category);
    if (category.empty()) {
        category = "General";
    }
    
    DataModel::Book book(isbn, title, author, year, quantity, category);
    std::string result = service_->addBook(book);
    
    if (result.empty()) {
        std::cout << "✓ Book added successfully!\n";
    } else {
        std::cout << "✗ Error: " << result << "\n";
    }
}

void LibraryApp::deleteBook() {
    std::cout << "\n--- Delete Book ---\n";
    
    std::string isbn;
    std::cout << "Enter ISBN to delete: ";
    std::getline(std::cin, isbn);
    
    std::string confirm;
    std::cout << "Are you sure? (y/N): ";
    std::getline(std::cin, confirm);
    
    if (confirm == "y" || confirm == "Y") {
        std::string result = service_->deleteBook(isbn);
        if (result.empty()) {
            std::cout << "✓ Book deleted successfully!\n";
        } else {
            std::cout << "✗ Error: " << result << "\n";
        }
    } else {
        std::cout << "Delete cancelled.\n";
    }
}

void LibraryApp::listBooks() {
    std::cout << "\n--- All Books ---\n";
    auto books = service_->listBooks();
    displayBooks(books);
}

void LibraryApp::searchBooks() {
    std::cout << "\n--- Search Books ---\n";
    std::cout << "1. Search by ISBN\n";
    std::cout << "2. Search by Title\n";
    std::cout << "3. Search by Author\n";
    std::cout << "4. Search by Category\n";
    std::cout << "Choose search type (1-4): ";
    
    int choice = getChoice();
    std::string searchTerm;
    
    switch (choice) {
        case 1: {
            std::cout << "Enter ISBN: ";
            std::getline(std::cin, searchTerm);
            auto book = service_->searchByIsbn(searchTerm);
            if (book) {
                std::vector<DataModel::Book> results = {*book};
                displayBooks(results);
            } else {
                std::cout << "No book found with ISBN: " << searchTerm << "\n";
            }
            break;
        }
        case 2: {
            std::cout << "Enter title (or part of it): ";
            std::getline(std::cin, searchTerm);
            auto results = service_->searchByTitle(searchTerm);
            displayBooks(results);
            break;
        }
        case 3: {
            std::cout << "Enter author name (or part of it): ";
            std::getline(std::cin, searchTerm);
            auto results = service_->searchByAuthor(searchTerm);
            displayBooks(results);
            break;
        }
        case 4: {
            std::cout << "Enter category (or part of it): ";
            std::getline(std::cin, searchTerm);
            auto results = service_->searchByCategory(searchTerm);
            displayBooks(results);
            break;
        }
        default:
            std::cout << "Invalid search option.\n";
    }
}

void LibraryApp::displayBooks(const std::vector<DataModel::Book>& books) {
    if (books.empty()) {
        std::cout << "No books found.\n";
        return;
    }
    
    std::cout << "\n";
    std::cout << std::left << std::setw(15) << "ISBN"
              << std::setw(25) << "Title"
              << std::setw(20) << "Author"
              << std::setw(6) << "Year"
              << std::setw(6) << "Qty"
              << std::setw(15) << "Category" << "\n";
    std::cout << std::string(87, '-') << "\n";
    
    for (const auto& book : books) {
        std::cout << std::left << std::setw(15) << book.getIsbn().substr(0, 14)
                  << std::setw(25) << book.getTitle().substr(0, 24)
                  << std::setw(20) << book.getAuthor().substr(0, 19)
                  << std::setw(6) << book.getYear()
                  << std::setw(6) << book.getQuantity()
                  << std::setw(15) << book.getCategory().substr(0, 14) << "\n";
    }
    std::cout << "\nTotal: " << books.size() << " book(s)\n";
}

void LibraryApp::browseByCategory() {
    std::cout << "\n--- Browse by Category ---\n";
    std::cout << "1. View Category Statistics\n";
    std::cout << "2. Filter by Specific Category\n";
    std::cout << "Choose option (1-2): ";
    
    int choice = getChoice();
    
    switch (choice) {
        case 1: {
            displayCategoryStatistics();
            break;
        }
        case 2: {
            auto categories = service_->getAllCategories();
            if (categories.empty()) {
                std::cout << "No categories found.\n";
                return;
            }
            
            std::cout << "\nAvailable categories:\n";
            for (size_t i = 0; i < categories.size(); ++i) {
                std::cout << (i + 1) << ". " << categories[i] << "\n";
            }
            
            std::cout << "Choose category (1-" << categories.size() << "): ";
            int catChoice = getChoice();
            
            if (catChoice >= 1 && catChoice <= static_cast<int>(categories.size())) {
                std::string selectedCategory = categories[catChoice - 1];
                std::cout << "\n--- Books in category: " << selectedCategory << " ---\n";
                auto results = service_->searchByCategory(selectedCategory);
                displayBooks(results);
            } else {
                std::cout << "Invalid category choice.\n";
            }
            break;
        }
        default:
            std::cout << "Invalid option.\n";
    }
}

void LibraryApp::displayCategoryStatistics() {
    std::cout << "\n--- Category Statistics ---\n";
    auto stats = service_->getCategoryStatistics();
    
    if (stats.empty()) {
        std::cout << "No books found.\n";
        return;
    }
    
    std::cout << std::left << std::setw(20) << "Category" << std::setw(10) << "Count" << "\n";
    std::cout << std::string(30, '-') << "\n";
    
    int totalBooks = 0;
    for (const auto& pair : stats) {
        std::cout << std::left << std::setw(20) << pair.first 
                  << std::setw(10) << pair.second << "\n";
        totalBooks += pair.second;
    }
    
    std::cout << std::string(30, '-') << "\n";
    std::cout << std::left << std::setw(20) << "Total Books:" << std::setw(10) << totalBooks << "\n";
}

void LibraryApp::displayLibraryStatistics() {
    std::cout << "\n=== Library Statistics Dashboard ===\n";
    
    auto stats = service_->getLibraryStatistics();
    
    if (stats.totalBooks == 0) {
        std::cout << "No books found in the library.\n";
        return;
    }
    
    // Collection Overview
    std::cout << "\n📊 Collection Overview:\n";
    std::cout << "   Total Books: " << stats.totalBooks << "\n";
    std::cout << "   Total Quantity: " << stats.totalQuantity << "\n";
    std::cout << "   Unique Authors: " << stats.uniqueAuthors << "\n";
    std::cout << "   Categories: " << stats.totalCategories << "\n";
    
    // Category Analytics
    std::cout << "\n📈 Category Analytics:\n";
    if (!stats.mostPopularCategory.empty()) {
        std::cout << "   Most Popular: " << stats.mostPopularCategory << "\n";
    }
    if (!stats.categoryWithHighestQuantity.empty()) {
        std::cout << "   Highest Quantity: " << stats.categoryWithHighestQuantity << "\n";
    }
    std::cout << "   Average per Category: " << std::fixed << std::setprecision(1) 
              << stats.averageBooksPerCategory << " books\n";
    
    // Author Analytics
    std::cout << "\n👥 Author Analytics:\n";
    if (!stats.mostProlificAuthor.empty()) {
        std::cout << "   Most Prolific: " << stats.mostProlificAuthor << "\n";
    }
    
    // Show top 5 authors
    auto authorStats = service_->getAuthorStatistics();
    if (!authorStats.empty()) {
        std::vector<std::pair<std::string, int>> sortedAuthors(authorStats.begin(), authorStats.end());
        std::sort(sortedAuthors.begin(), sortedAuthors.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });
        
        std::cout << "   Top Authors: ";
        int count = 0;
        for (const auto& author : sortedAuthors) {
            if (count >= 5) break;
            if (count > 0) std::cout << ", ";
            std::cout << author.first << " (" << author.second << ")";
            count++;
        }
        std::cout << "\n";
    }
    
    // Publication Analytics
    std::cout << "\n📅 Publication Analytics:\n";
    if (stats.oldestYear > 0 && stats.newestYear > 0) {
        std::cout << "   Year Range: " << stats.oldestYear << " - " << stats.newestYear << "\n";
    }
    
    // Calculate decade distribution
    auto yearStats = service_->getYearStatistics();
    if (!yearStats.empty()) {
        std::map<int, int> decadeStats;
        for (const auto& yearPair : yearStats) {
            int decade = (yearPair.first / 10) * 10;
            decadeStats[decade] += yearPair.second;
        }
        
        if (!decadeStats.empty()) {
            auto maxDecade = std::max_element(decadeStats.begin(), decadeStats.end(),
                [](const auto& a, const auto& b) { return a.second < b.second; });
            std::cout << "   Most Common Decade: " << maxDecade->first << "s (" 
                      << maxDecade->second << " books)\n";
        }
    }
    
    // Quick Insights
    std::cout << "\n💡 Quick Insights:\n";
    std::cout << "   Average books per category: " << std::fixed << std::setprecision(1) 
              << stats.averageBooksPerCategory << "\n";
    std::cout << "   Average quantity per book: " << std::fixed << std::setprecision(1) 
              << stats.averageQuantityPerBook << "\n";
    
    if (!stats.oldestBookTitle.empty()) {
        std::cout << "   Oldest Book: " << stats.oldestBookTitle << " (" << stats.oldestYear << ")\n";
    }
    if (!stats.newestBookTitle.empty()) {
        std::cout << "   Newest Book: " << stats.newestBookTitle << " (" << stats.newestYear << ")\n";
    }
    
    std::cout << "\n";
}


} // namespace Launcher

