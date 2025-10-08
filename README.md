# BiblioTrack - Secure Library Management System (v2.06)

A clean, well-structured C++ library management system with secure authentication, organized in separate layers for maintainability and scalability. Enhanced with comprehensive category management, advanced browsing features, and enterprise-grade security.

## Features

- ✅ Add new books with ISBN, title, author, year, quantity, and category
- ✅ Delete books by ISBN with confirmation
- ✅ List all books in a formatted table with category display
- ✅ Search books by ISBN (exact match)
- ✅ Search books by title (partial, case-insensitive)
- ✅ Search books by author (partial, case-insensitive)
- ✅ Search books by category (partial, case-insensitive)
- ✅ Browse books by category with filtering options
- ✅ View category statistics and distribution
- ✅ JSON file persistence (line-delimited) with automatic creation
- ✅ Secure user authentication system with encrypted credential storage (v2.06)
- ✅ Clean layered architecture with separation of concerns
- ✅ Enterprise-grade security with config directory isolation

## Layered Architecture

The project is organized into three distinct layers:

### 📊 **DataModel Layer** (`src/datamodel/`)
Contains the core data structures and entities.
- `Book.hpp` - Book entity with properties (ISBN, title, author, year, quantity, category) and basic operations

### 🔧 **Services Layer** (`src/services/`)
Contains business logic, data access, and security components.
- `BookRepository.*` - Data persistence and JSON file operations with category support
- `LibraryService.*` - Business logic, validation, search operations, and category management
- `AuthenticationService.*` - Complete authentication flow with user interaction and secure credential management (v2.06)

### 🚀 **Launcher Layer** (`src/launcher/`)
Contains the presentation layer and application entry point.
- `LibraryApp.*` - User interface and menu system with category browsing features (authentication delegated to services)
- `main.cpp` - Application entry point

## Project Structure

```
BiblioTrack/
├── src/
│   ├── datamodel/
│   │   └── Book.hpp              # Book entity
│   ├── services/
│   │   ├── BookRepository.*      # Data access layer
│   │   └── LibraryService.*      # Business logic layer
│   └── launcher/
│       ├── LibraryApp.*          # Application layer
│       └── main.cpp              # Entry point
├── data/
│   └── books.json                # Data storage (auto-created)
├── config/                        # 🔐 SECURE CONFIGURATION
│   ├── credentials_template.json # Template for admin credentials
│   └── credentials.json          # Admin credentials (git-ignored, secure)
├── build/                        # Build directory
└── CMakeLists.txt                # Build configuration
```

## Architecture Benefits

### 🎯 **Separation of Concerns**
- **DataModel**: Pure data structures, no business logic
- **Services**: Business rules, data operations, and complete authentication flow
- **Launcher**: User interface and application flow (delegates authentication to services)

### 🔄 **Dependency Flow**
```
Launcher → Services → DataModel
```
- Each layer only depends on the layer below it
- Clean, unidirectional dependencies
- Easy to test and maintain

### 📈 **Scalability**
- Easy to add new features in the appropriate layer (demonstrated by category feature extension)
- Simple to replace components (e.g., CSV → Database)
- Clear boundaries for team development
- Backward compatibility maintained during feature additions

## Building and Running

### Prerequisites
- C++17 compatible compiler
- CMake 3.12 or higher
- Unix-like system (for secure file permissions)

### Build Instructions

```bash
# Create and enter build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Setup secure credentials file (required for authentication)
cp ../config/credentials_template.json ../config/credentials.json
# Edit config/credentials.json with your desired admin credentials
# Set secure file permissions (recommended for production)
chmod 600 ../config/credentials.json

# Run the application
./bibliotrack
```

## Usage

The application now requires authentication before access:

1. **Authentication Menu** - Choose to login or exit the program
2. **Login** - Enter admin credentials (username/password from credentials.json)
   - Invalid credentials will prompt to try again or exit
   - Multiple login attempts allowed
3. **Add Book** - Enter book details with validation (includes category assignment)
4. **Delete Book** - Remove a book by ISBN with confirmation
5. **List All Books** - Display all books in a formatted table with categories
6. **Search Books** - Search by ISBN (exact), title (partial), author (partial), or category (partial)
7. **Browse by Category** - View category statistics or filter books by specific categories
8. **Exit** - Close the application

## Data Storage

As of v2.05, books are stored in `data/books.json` as one JSON object per line (line-delimited JSON).

## Security Features (v2.06)

### 🔐 **Secure Authentication System**
- **Isolated Storage**: Credentials stored in dedicated `config/` directory
- **File Permissions**: Secure access with `chmod 600` (owner read/write only)
- **Git Protection**: Credentials automatically ignored by version control
- **Template System**: Safe setup with `credentials_template.json`
- **Security Monitoring**: Warnings for insecure credential locations

### 🛡️ **Authentication Flow**
- **Default Credentials**: username=`admin`, password=`admin123`
- **Production Ready**: Change credentials for production use
- **User-Friendly**: Multiple login attempts with clear options
- **Visual Feedback**: Success/failure indicators (✓/✗)
- **Easy Exit**: Option to quit at any time

Example lines in `books.json` (numeric ISBNs):
```
{"isbn":"9780134685991","title":"Effective Modern C++","author":"Scott Meyers","year":2014,"quantity":5,"category":"Programming"}
{"isbn":"1255289001","title":"batman","author":"ali mansoor","year":2025,"quantity":99,"category":"Fiction"}
```

## 🎯 **Category Management Features (v2.01+)**

### **Enhanced Book Organization**
- **Category Assignment**: Assign categories when adding new books with helpful suggestions from existing categories
- **Smart Defaults**: Books without specified categories automatically default to "General"
- **Category Suggestions**: System displays existing categories when adding new books for consistency

### **Advanced Browsing & Analytics**
- **Category Statistics**: View distribution of books across all categories with counts and totals
- **Category Filtering**: Browse books by selecting from available categories
- **Enhanced Search**: Search functionality extended to include category-based filtering
- **Visual Organization**: All book displays now include category information in formatted tables

### **Backward Compatibility**
- **Legacy Support**: Seamlessly handles existing CSV files without category data
- **Gradual Migration**: Old books are automatically assigned "General" category
- **Non-Breaking**: All existing functionality remains unchanged

## 🔧 **Data Quality & Input Sanitization (v2.04)**

### **Enhanced CSV Handling**
- **Proper CSV Escaping**: Books with commas, quotes, or newlines in titles/authors are properly escaped
- **Robust Parsing**: System correctly handles quoted fields and escaped characters in CSV files
- **Data Integrity**: Prevents CSV corruption from special characters in book data

### **Input Sanitization**
- **Whitespace Cleanup**: Automatic trimming of leading/trailing spaces and normalization of excessive whitespace
- **Data Consistency**: Ensures clean, consistent data storage and retrieval
- **User Experience**: Handles user input gracefully without breaking the system

### **Improved Reliability**
- **Error Prevention**: Reduces data corruption issues from malformed input
- **Better Compatibility**: Enhanced JSON file format compliance
- **Maintainable Data**: Cleaner data storage for long-term system health

### **Performance Note (v2.05)**
- Repository loads data once into memory and maintains an in-memory index by ISBN
- Provides O(1) ISBN lookups and reduces repeated disk reads
- Efficient writes: append on add, rewrite only when deleting

### **Security Note (v2.06)**
- Credentials stored in isolated `config/` directory for enhanced security
- File permissions and git protection prevent accidental credential exposure
- Enterprise-grade authentication system with user-friendly interface

## Design Principles

### ✅ **Clean Architecture**
- Clear layer separation with defined responsibilities
- Dependency inversion (depends on abstractions)
- Single responsibility principle

### ✅ **SOLID Principles**
- **S**ingle Responsibility: Each class has one reason to change
- **O**pen/Closed: Open for extension, closed for modification
- **L**iskov Substitution: Components are easily replaceable
- **I**nterface Segregation: Focused, cohesive interfaces
- **D**ependency Inversion: Depend on abstractions, not concretions

### ✅ **Maintainability**
- Readable code with clear naming conventions
- Minimal coupling between layers
- Easy to extend and modify

This architecture provides a solid foundation for a secure library management system that can grow and evolve while maintaining clean, maintainable code and enterprise-grade security.

## Version History

- **v2.06**: Added secure authentication system with config directory isolation and enterprise-grade security
- **v2.05**: JSON storage implementation, improved performance with in-memory caching
- **v2.04**: Added comprehensive category management and browsing features
- **v2.03**: Enhanced search functionality with partial matching
- **v2.02**: Improved user interface and error handling
- **v2.01**: Added ISBN validation and data persistence
- **v2.00**: Initial layered architecture implementation