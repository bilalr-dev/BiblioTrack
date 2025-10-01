#pragma once

#include <mutex>
#include <string>
#include <vector>
#include <optional>
#include "repository/IBookRepository.hpp"

namespace LibraryApp {
namespace Repository {

class CsvBookRepository : public IBookRepository {
public:
	explicit CsvBookRepository(std::string csvPath);
	std::vector<DataModel::Book> findAll() override;
	std::optional<DataModel::Book> findByIsbn(const std::string& isbn) override;
	bool insert(const DataModel::Book& book) override;
	bool removeByIsbn(const std::string& isbn) override;

private:
	std::string csvPath_;
	mutable std::mutex ioMutex_;

	std::vector<std::vector<std::string>> readCsvLinesUnlocked();
	bool writeCsvLinesUnlocked(const std::vector<std::vector<std::string>>& rows);

	static std::vector<std::string> parseCsvLine(const std::string& line);
	static std::string buildCsvLine(const std::vector<std::string>& fields);
};

} // namespace Repository
} // namespace LibraryApp

