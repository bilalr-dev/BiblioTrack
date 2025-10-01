#pragma once

#include <string>
#include <cstdint>

namespace LibraryApp {
namespace DataModel {

class Book {
public:
	Book() = default;
	Book(std::string isbn,
	     std::string title,
	     std::string author,
	     std::uint16_t year,
	     std::uint32_t quantity)
	    : isbn_(std::move(isbn)),
	      title_(std::move(title)),
	      author_(std::move(author)),
	      year_(year),
	      quantity_(quantity) {}

	const std::string& getIsbn() const { return isbn_; }
	const std::string& getTitle() const { return title_; }
	const std::string& getAuthor() const { return author_; }
	std::uint16_t getYear() const { return year_; }
	std::uint32_t getQuantity() const { return quantity_; }

	void setTitle(const std::string& title) { title_ = title; }
	void setAuthor(const std::string& author) { author_ = author; }
	void setYear(std::uint16_t year) { year_ = year; }
	void setQuantity(std::uint32_t quantity) { quantity_ = quantity; }

private:
	std::string isbn_;
	std::string title_;
	std::string author_;
	std::uint16_t year_ {0};
	std::uint32_t quantity_ {0};
};

} // namespace DataModel
} // namespace LibraryApp

