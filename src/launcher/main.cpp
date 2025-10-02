#include "LibraryApp.hpp"
#include <iostream>

int main() {
    try {
        Launcher::LibraryApp app;
        app.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}