#include "services/impl/FileService.hpp"
#include <fstream>
#include <sys/stat.h>

using namespace LibraryApp::Services;

bool FileService::ensureFileWithHeaders(const std::string& path, const std::string& headersCsv) {
	std::lock_guard<std::mutex> lock(fileMutex_);
	// Check existence
	struct stat buffer{};
	bool exists = (stat(path.c_str(), &buffer) == 0);
	if (!exists) {
		std::ofstream out(path, std::ios::out | std::ios::trunc);
		if (!out.is_open()) return false;
		out << headersCsv << "\n";
		return static_cast<bool>(out);
	}
	// If exists but empty, write headers
	{
		std::ifstream in(path);
		if (!in.is_open()) return false;
		in.seekg(0, std::ios::end);
		if (in.tellg() == 0) {
			in.close();
			std::ofstream out(path, std::ios::out | std::ios::trunc);
			if (!out.is_open()) return false;
			out << headersCsv << "\n";
			return static_cast<bool>(out);
		}
	}
	return true;
}
