# BiblioTrack - Layered Library Management System

A clean, well-structured C++ library management system organized in separate layers for maintainability and scalability.

## Features

- ✅ Add new books with ISBN, title, author, year, and quantity
- ✅ Delete books by ISBN with confirmation
- ✅ List all books in a formatted table
- ✅ Search books by ISBN (exact match)
- ✅ Search books by title (partial, case-insensitive)
- ✅ Search books by author (partial, case-insensitive)
- ✅ CSV file persistence with automatic creation
- ✅ Clean layered architecture with separation of concerns

## Layered Architecture

The project is organized into three distinct layers:

### 📊 **DataModel Layer** (`src/datamodel/`)
Contains the core data structures and entities.
- `Book.hpp` - Book entity with properties and basic operations

### 🔧 **Services Layer** (`src/services/`)
Contains business logic and data access components.
- `BookRepository.*` - Data persistence and CSV file operations
- `LibraryService.*` - Business logic, validation, and search operations

### 🚀 **Launcher Layer** (`src/launcher/`)
Contains the presentation layer and application entry point.
- `LibraryApp.*` - User interface and menu system
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
- Easy to add new features in the appropriate layer
- Simple to replace components (e.g., CSV → Database)
- Clear boundaries for team development

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

1. **Add Book** - Enter book details with validation
2. **Delete Book** - Remove a book by ISBN with confirmation
3. **List All Books** - Display all books in a formatted table
4. **Search Books** - Search by ISBN (exact), title (partial), or author (partial)
5. **Exit** - Close the application

## Data Storage

Books are stored in `data/books.csv` with the following format:
```
ISBN,Title,Author,Year,Quantity
978-0134685991,Effective Modern C++,Scott Meyers,2014,5
```

The file and directory are automatically created on first run.

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