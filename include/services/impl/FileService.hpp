#pragma once

#include "services/IFileService.hpp"
#include <mutex>

namespace LibraryApp {
namespace Services {

class FileService : public IFileService {
public:
	bool ensureFileWithHeaders(const std::string& path, const std::string& headersCsv) override;
private:
	std::mutex fileMutex_;
};

} // namespace Services
} // namespace LibraryApp

