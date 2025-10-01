#include "repository/CsvBookRepository.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

using namespace LibraryApp::Repository;
using LibraryApp::DataModel::Book;

namespace {
static const char* HEADERS = "ISBN,Title,Author,Year,Quantity";

static bool hasHeaders(std::ifstream& in) {
	std::string firstLine;
	std::streampos pos = in.tellg();
	if (!std::getline(in, firstLine)) {
		in.clear();
		in.seekg(pos);
		return false;
	}
	in.clear();
	in.seekg(pos);
	return firstLine == HEADERS;
}
}

CsvBookRepository::CsvBookRepository(std::string csvPath)
	: csvPath_(std::move(csvPath)) {}

std::vector<Book> CsvBookRepository::findAll() {
	std::lock_guard<std::mutex> lock(ioMutex_);
	auto rows = readCsvLinesUnlocked();
	std::vector<Book> books;
	for (size_t i = 1; i < rows.size(); ++i) {
		const auto& r = rows[i];
		if (r.size() != 5) continue; // skip corrupted row
		try {
			std::uint16_t year = static_cast<std::uint16_t>(std::stoi(r[3]));
			std::uint32_t qty = static_cast<std::uint32_t>(std::stoul(r[4]));
			books.emplace_back(r[0], r[1], r[2], year, qty);
		} catch (...) {
			continue; // skip corrupted row
		}
	}
	return books;
}

std::optional<Book> CsvBookRepository::findByIsbn(const std::string& isbn) {
	std::lock_guard<std::mutex> lock(ioMutex_);
	auto rows = readCsvLinesUnlocked();
	for (size_t i = 1; i < rows.size(); ++i) {
		const auto& r = rows[i];
		if (r.size() == 5 && r[0] == isbn) {
			try {
				std::uint16_t year = static_cast<std::uint16_t>(std::stoi(r[3]));
				std::uint32_t qty = static_cast<std::uint32_t>(std::stoul(r[4]));
				return Book(r[0], r[1], r[2], year, qty);
			} catch (...) {
				return std::nullopt;
			}
		}
	}
	return std::nullopt;
}

bool CsvBookRepository::insert(const Book& book) {
	std::lock_guard<std::mutex> lock(ioMutex_);
	auto rows = readCsvLinesUnlocked();
	// If empty file or missing, write header first
	if (rows.empty()) {
		std::ofstream out(csvPath_, std::ios::app);
		if (!out.is_open()) return false;
		out << HEADERS << "\n";
		out.flush();
	}
	// Duplicate check
	for (size_t i = 1; i < rows.size(); ++i) {
		if (rows[i].size() >= 1 && rows[i][0] == book.getIsbn()) {
			return false;
		}
	}
	std::vector<std::string> fields = {
		book.getIsbn(), book.getTitle(), book.getAuthor(),
		std::to_string(book.getYear()), std::to_string(book.getQuantity())
	};
	return appendCsvLineUnlocked(fields);
}

bool CsvBookRepository::removeByIsbn(const std::string& isbn) {
	std::lock_guard<std::mutex> lock(ioMutex_);
	auto rows = readCsvLinesUnlocked();
	if (rows.empty()) return false;
	bool removed = false;
	std::vector<std::vector<std::string>> out;
	out.reserve(rows.size());
	// keep header
	if (!rows.empty()) out.push_back(rows[0]);
	for (size_t i = 1; i < rows.size(); ++i) {
		if (rows[i].size() >= 1 && rows[i][0] == isbn) {
			removed = true;
			continue;
		}
		out.push_back(rows[i]);
	}
	if (!removed) return false;
	return writeCsvLinesUnlocked(out);
}

std::vector<std::vector<std::string>> CsvBookRepository::readCsvLinesUnlocked() {
	std::vector<std::vector<std::string>> rows;
	std::ifstream in(csvPath_);
	if (!in.good()) {
		return rows; // file missing handled upstream
	}
	// Validate header if present or add if first line is headers
	if (!hasHeaders(in)) {
		// If file has content but no headers, treat first line as data; we'll not enforce rewrite here
	}
	std::string line;
	while (std::getline(in, line)) {
		rows.push_back(parseCsvLine(line));
	}
	return rows;
}

bool CsvBookRepository::writeCsvLinesUnlocked(const std::vector<std::vector<std::string>>& rows) {
	std::ofstream out(csvPath_, std::ios::trunc);
	if (!out.is_open()) {
		return false;
	}
	// Ensure header
	if (rows.empty()) {
		out << HEADERS << "\n";
		out.flush();
		return static_cast<bool>(out);
	}
	// If first row isn't header, write header explicitly
	if (rows[0].size() != 5 || rows[0][0] != "ISBN") {
		out << HEADERS << "\n";
	}
	for (size_t i = 0; i < rows.size(); ++i) {
		if (i == 0 && rows[0].size() == 5 && rows[0][0] == "ISBN") {
			// write header as-is
			out << buildCsvLine(rows[0]) << "\n";
			continue;
		}
		out << buildCsvLine(rows[i]) << "\n";
	}
	out.flush();
	return static_cast<bool>(out);
}

bool CsvBookRepository::appendCsvLineUnlocked(const std::vector<std::string>& fields) {
	std::ofstream out(csvPath_, std::ios::app);
	if (!out.is_open()) return false;
	out << buildCsvLine(fields) << "\n";
	out.flush();
	return static_cast<bool>(out);
}

std::vector<std::string> CsvBookRepository::parseCsvLine(const std::string& line) {
	std::vector<std::string> result;
	std::string field;
	bool inQuotes = false;
	for (size_t i = 0; i < line.size(); ++i) {
		char c = line[i];
		if (inQuotes) {
			if (c == '"') {
				if (i + 1 < line.size() && line[i + 1] == '"') {
					field += '"';
					++i;
				} else {
					inQuotes = false;
				}
			} else {
				field += c;
			}
		} else {
			if (c == '"') {
				inQuotes = true;
			} else if (c == ',') {
				result.push_back(field);
				field.clear();
			} else {
				field += c;
			}
		}
	}
	result.push_back(field);
	return result;
}

std::string CsvBookRepository::buildCsvLine(const std::vector<std::string>& fields) {
	std::ostringstream oss;
	for (size_t i = 0; i < fields.size(); ++i) {
		std::string v = fields[i];
		bool needQuotes = v.find(',') != std::string::npos || v.find('"') != std::string::npos || v.find('\n') != std::string::npos;
		if (needQuotes) {
			std::string escaped;
			escaped.reserve(v.size());
			for (char c : v) {
				if (c == '"') escaped += '"';
				escaped += c;
			}
			oss << '"' << escaped << '"';
		} else {
			oss << v;
		}
		if (i + 1 < fields.size()) oss << ',';
	}
	return oss.str();
}
