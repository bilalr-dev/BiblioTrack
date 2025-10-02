#pragma once
#include <string>

namespace DataModel {

class Book {
public:
    Book() = default;
    Book(const std::string& isbn, const std::string& title, const std::string& author, 
         int year, int quantity)
        : isbn_(isbn), title_(title), author_(author), year_(year), quantity_(quantity) {}

    // Getters
    const std::string& getIsbn() const { return isbn_; }
    const std::string& getTitle() const { return title_; }
    const std::string& getAuthor() const { return author_; }
    int getYear() const { return year_; }
    int getQuantity() const { return quantity_; }

    // Setters
    void setTitle(const std::string& title) { title_ = title; }
    void setAuthor(const std::string& author) { author_ = author; }
    void setYear(int year) { year_ = year; }
    void setQuantity(int quantity) { quantity_ = quantity; }

private:
    std::string isbn_;
    std::string title_;
    std::string author_;
    int year_ = 0;
    int quantity_ = 0;
};

} // namespace DataModel
