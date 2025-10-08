#include "AuthenticationService.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace Services {

AuthenticationService::AuthenticationService() : initialized_(false) {
    initialized_ = loadCredentialsFromJson();
}

bool AuthenticationService::authenticate(const std::string& username, const std::string& password) {
    if (!initialized_) {
        std::cerr << "Authentication service not properly initialized" << std::endl;
        return false;
    }
    
    // Compare credentials (case-sensitive)
    return (username == adminLogin_ && password == adminPassword_);
}

bool AuthenticationService::isInitialized() const {
    return initialized_;
}

bool AuthenticationService::runAuthenticationFlow() {
    if (!initialized_) {
        std::cout << "Authentication service failed to initialize.\n";
        return false;
    }
    
    showWelcomeScreen();
    
    while (true) {
        showLoginOptions();
        int choice = getUserChoice();
        
        switch (choice) {
            case 1: {
                if (attemptLogin()) {
                    std::cout << "✓ Access granted. Welcome to BiblioTrack!\n\n";
                    return true;
                } else {
                    std::cout << "✗ Invalid credentials. Please try again.\n\n";
                }
                break;
            }
            case 2: {
                std::cout << "Goodbye! Thank you for using BiblioTrack.\n";
                return false;
            }
            default: {
                std::cout << "Invalid choice. Please select 1 or 2.\n\n";
            }
        }
    }
}

void AuthenticationService::showWelcomeScreen() {
    std::cout << "\n=== BiblioTrack Authentication ===\n";
    std::cout << "Welcome to BiblioTrack Library Management System!\n";
}

void AuthenticationService::showLoginOptions() {
    std::cout << "\n--- Login Options ---\n";
    std::cout << "1. Login\n";
    std::cout << "2. Exit\n";
    std::cout << "Choose an option (1-2): ";
}

bool AuthenticationService::attemptLogin() {
    std::string username, password;
    
    std::cout << "\n--- Login ---\n";
    std::cout << "Username: ";
    std::getline(std::cin, username);
    
    std::cout << "Password: ";
    std::getline(std::cin, password);
    
    return authenticate(username, password);
}

int AuthenticationService::getUserChoice() {
    int choice;
    std::cin >> choice;
    std::cin.ignore(); // Clear the newline character
    return choice;
}

bool AuthenticationService::loadCredentialsFromJson() {
    try {
        // Try multiple possible locations for credentials.json with secure config directory
        std::vector<std::string> possiblePaths = {
            "config/credentials.json",           // Secure config directory
            "../config/credentials.json",        // Parent config directory (for build/ subdirectory)
            "../../config/credentials.json",     // Two levels up (for nested build directories)
            "credentials.json",                  // Fallback: current directory
            "../credentials.json",               // Fallback: parent directory
            "../../credentials.json"             // Fallback: two levels up
        };
        
        std::ifstream file;
        std::string credentialsPath;
        
        for (const auto& path : possiblePaths) {
            file.open(path);
            if (file.is_open()) {
                credentialsPath = path;
                break;
            }
        }
        
        if (!file.is_open()) {
            std::cerr << "Error: credentials.json file not found in any of these locations:" << std::endl;
            for (const auto& path : possiblePaths) {
                std::cerr << "  - " << path << std::endl;
            }
            std::cerr << "Please copy config/credentials_template.json to config/credentials.json" << std::endl;
            std::cerr << "For security, credentials are stored in the config/ directory" << std::endl;
            return false;
        }
        
        // Read entire file into string
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string json = buffer.str();
        file.close();
        
        // Extract admin credentials from JSON
        adminLogin_ = extractJsonString(json, "username");
        adminPassword_ = extractJsonString(json, "password");
        
        // Validate that credentials were found and are not empty
        if (adminLogin_.empty() || adminPassword_.empty()) {
            std::cerr << "Error: Could not extract username or password from credentials.json" << std::endl;
            std::cerr << "Please ensure credentials.json has valid admin username and password" << std::endl;
            return false;
        }
        
        std::cout << "Authentication service initialized successfully (using " << credentialsPath << ")" << std::endl;
        
        // Security check: warn if credentials are in insecure location
        if (credentialsPath.find("config/") == std::string::npos) {
            std::cout << "Warning: Credentials found in non-secure location. Consider moving to config/ directory." << std::endl;
        }
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading credentials: " << e.what() << std::endl;
        std::cerr << "Please ensure credentials.json exists and is valid JSON" << std::endl;
        return false;
    }
}

std::string AuthenticationService::extractJsonString(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t pos = json.find(searchKey);
    if (pos == std::string::npos) return "";
    
    // Find the colon after the key
    pos = json.find(':', pos);
    if (pos == std::string::npos) return "";
    
    // Find the opening quote
    pos = json.find('"', pos);
    if (pos == std::string::npos) return "";
    pos++; // Skip the opening quote
    
    // Find the closing quote
    size_t end = json.find('"', pos);
    if (end == std::string::npos) return "";
    
    return json.substr(pos, end - pos);
}

} // namespace Services
