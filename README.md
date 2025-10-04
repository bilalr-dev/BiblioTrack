# BiblioTrack - Layered Library Management System (v2.04)

A clean, well-structured C++ library management system organized in separate layers for maintainability and scalability. Enhanced with comprehensive category management and advanced browsing features.

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
- ✅ CSV file persistence with automatic creation
- ✅ Clean layered architecture with separation of concerns

## Layered Architecture

The project is organized into three distinct layers:

### 📊 **DataModel Layer** (`src/datamodel/`)
Contains the core data structures and entities.
- `Book.hpp` - Book entity with properties (ISBN, title, author, year, quantity, category) and basic operations

### 🔧 **Services Layer** (`src/services/`)
Contains business logic and data access components.
- `BookRepository.*` - Data persistence and CSV file operations with category support
- `LibraryService.*` - Business logic, validation, search operations, and category management

### 🚀 **Launcher Layer** (`src/launcher/`)
Contains the presentation layer and application entry point.
- `LibraryApp.*` - User interface and menu system with category browsing features
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
│   └── books.csv                 # Data storage (auto-created)
├── build/                        # Build directory
└── CMakeLists.txt                # Build configuration
```

## Architecture Benefits

### 🎯 **Separation of Concerns**
- **DataModel**: Pure data structures, no business logic
- **Services**: Business rules and data operations
- **Launcher**: User interface and application flow

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

### Build Instructions

```bash
# Create and enter build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run the application
./bibliotrack
```

## Usage

The application provides a simple menu-driven interface:

1. **Add Book** - Enter book details with validation (includes category assignment)
2. **Delete Book** - Remove a book by ISBN with confirmation
3. **List All Books** - Display all books in a formatted table with categories
4. **Search Books** - Search by ISBN (exact), title (partial), author (partial), or category (partial)
5. **Browse by Category** - View category statistics or filter books by specific categories
6. **Exit** - Close the application

## Data Storage

Books are stored in `data/books.csv` with the following format:
```
ISBN,Title,Author,Year,Quantity,Category
978-0134685991,Effective Modern C++,Scott Meyers,2014,5,Programming
fr-1255-289d,batman,ali mansoor,2025,99,Fiction
```

The file and directory are automatically created on first run. The system supports backward compatibility with older CSV files that don't include the Category column (defaults to "General").

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
- **Better Compatibility**: Enhanced CSV file format compliance
- **Maintainable Data**: Cleaner data storage for long-term system health

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

This architecture provides a solid foundation for a library management system that can grow and evolve while maintaining clean, maintainable code.