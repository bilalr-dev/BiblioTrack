## Library Management System v1.0 (C++17)

A clean-architecture C++17 console application for managing a simple library book inventory. Designed for scalability and future features.

### Architecture

Layers (namespaces under `LibraryApp`):
- Launcher: CLI entry point.
- Data Model: Core domain entities (`Book`, `Library`).
- Repository: Data access via the Repository pattern (`IBookRepository`, CSV implementation).
- Services: Business logic and validation (`ILibraryService`, `IBookValidationService`, `IFileService`).

The design emphasizes SOLID principles, dependency inversion (interfaces), and loose coupling. The service layer depends on abstractions (`IBookRepository`), enabling easy swaps to other storage backends (e.g., JSON, SQLite).

### Build & Run

Requirements: CMake 3.12+, C++17 compiler

```bash
cd /Users/bilalrahaoui/Documents/BiblioTrack
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
./bin/library_app
```

Notes:
- `DATA_DIR` is compiled in as `${CMAKE_SOURCE_DIR}/data`, so CSV is read/written under `data/books.csv` in the project root regardless of where you run the binary from.
- On first run, `data/books.csv` will be auto-created with headers.

### CLI

```
=== Library Management System v1.0 ===
1. Add Book
2. Delete Book
3. View Current Stock
4. Exit
Enter your choice:
```

### CSV Specification

- File: `data/books.csv`
- Headers: `ISBN,Title,Author,Year,Quantity`
- Commas and quotes in fields supported. Quotes inside values are doubled per CSV rules.

### Validation Rules

- ISBN must be unique
- Year between 1000–2025
- Quantity is a positive integer
- Title and Author cannot be empty

### Roadmap

- v2.0: search by ISBN/Title/Author, borrowing/returning, members
- v3.0: due dates, fines, multiple storage formats (JSON, SQLite), configs

### Scalability Considerations

- Dependency inversion allows swapping repository backends
- Thread-safe CSV operations via internal synchronization
- Modular services to extend business rules without touching persistence
- Clear boundaries for future features (search, members, borrowing) without modifying existing contracts

