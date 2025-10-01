#pragma once

#include <string>

namespace LibraryApp {
namespace Services {

class IFileService {
public:
	virtual ~IFileService() = default;
	virtual bool ensureFileWithHeaders(const std::string& path, const std::string& headersCsv) = 0;
};

} // namespace Services
} // namespace LibraryApp

