#pragma once
#include <string>

namespace Services {

/**
 * AuthenticationService handles complete user authentication flow for BiblioTrack v2.09
 * Provides secure login verification, user interaction, and authentication flow management
 */
class AuthenticationService {
public:
    /**
     * Constructor - loads credentials from JSON file
     */
    AuthenticationService();
    
    /**
     * Runs the complete authentication flow with user interaction
     * Handles login attempts, retries, and exit options
     * @return true if user successfully authenticates, false if user chooses to exit
     */
    bool runAuthenticationFlow();
    
    /**
     * Authenticates user credentials against stored admin credentials
     * @param username The username to verify
     * @param password The password to verify
     * @return true if credentials match admin credentials, false otherwise
     */
    bool authenticate(const std::string& username, const std::string& password);
    
    /**
     * Checks if authentication service is properly initialized
     * @return true if credentials file was loaded successfully, false otherwise
     */
    bool isInitialized() const;

private:
    std::string adminLogin_;
    std::string adminPassword_;
    bool initialized_;
    
    /**
     * Loads credentials from JSON file
     * @return true if loading was successful, false otherwise
     */
    bool loadCredentialsFromJson();
    
    /**
     * Simple JSON string extraction for credentials
     * @param json The JSON string to parse
     * @param key The key to extract
     * @return The extracted value or empty string if not found
     */
    std::string extractJsonString(const std::string& json, const std::string& key);
    
    /**
     * Displays the authentication welcome screen
     */
    void showWelcomeScreen();
    
    /**
     * Displays login options menu
     */
    void showLoginOptions();
    
    /**
     * Attempts to login with user input
     * @return true if login successful, false otherwise
     */
    bool attemptLogin();
    
    /**
     * Gets user choice from input
     * @return The user's choice as integer
     */
    int getUserChoice();
};

} // namespace Services
