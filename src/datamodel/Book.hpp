#pragma once
#include <string>

namespace DataModel {

class Book {
public:
    Book() = default;
    Book(const std::string& isbn, const std::string& title, const std::string& author, 
         int year, int quantity, const std::string& category = "General")
        : isbn_(isbn), title_(title), author_(author), year_(year), quantity_(quantity), category_(category) {}

    // Getters
    const std::string& getIsbn() const { return isbn_; }
    const std::string& getTitle() const { return title_; }
    const std::string& getAuthor() const { return author_; }
    int getYear() const { return year_; }
    int getQuantity() const { return quantity_; }
    const std::string& getCategory() const { return category_; }

    // Setters
    void setTitle(const std::string& title) { title_ = title; }
    void setAuthor(const std::string& author) { author_ = author; }
    void setYear(int year) { year_ = year; }
    void setQuantity(int quantity) { quantity_ = quantity; }
    void setCategory(const std::string& category) { category_ = category; }

private:
    std::string isbn_;
    std::string title_;
    std::string author_;
    int year_ = 0;
    int quantity_ = 0;
    std::string category_ = "General";
};

} // namespace DataModel
