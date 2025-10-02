# BiblioTrack - Library Management System

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/CMake-3.12%2B-green.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen.svg)](BUILD)

A professional, scalable library management system built with Clean Architecture principles in C++17. Designed for rapid development and enterprise-grade extensibility.

## 🎯 Project Overview

**Current Version:** v1.1 (Search Feature Added) ✅  
**Target Version:** v3.0 (Complete Library System)  
**Development Timeline:** 3 weeks (October 2-23, 2025)  
**Architecture:** Clean Architecture with SOLID principles  

BiblioTrack provides a complete library management solution with book inventory, member management, and borrowing system capabilities.

## 🏗️ Architecture

### Clean Architecture Layers

```
┌─────────────────────────────────────────┐
│              Presentation               │
│         (CLI Interface)                 │
├─────────────────────────────────────────┤
│              Services                   │
│    (Business Logic & Validation)       │
├─────────────────────────────────────────┤
│             Repository                  │
│        (Data Access Layer)             │
├─────────────────────────────────────────┤
│             Entities                    │
│         (Domain Models)                 │
└─────────────────────────────────────────┘
```

### Namespace Organization
```cpp
LibraryApp::
├── Entities::          // Book, Member, Transaction
├── Repository::        // IBookRepository, CsvBookRepository
├── Services::          // ILibraryService, ISearchService
└── Presentation::      // CLI, MenuSystem
```

### Key Design Principles
- **Dependency Inversion:** Services depend on abstractions, not implementations
- **Single Responsibility:** Each class has one reason to change
- **Open/Closed:** Open for extension, closed for modification
- **Interface Segregation:** Focused, cohesive interfaces
- **Loose Coupling:** Easy to test, maintain, and extend

## 🚀 Quick Start

### Prerequisites
- **CMake 3.12+**
- **C++17 compatible compiler** (GCC 7+, Clang 5+, MSVC 2017+)
- **Git** (for version control)

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/your-username/BiblioTrack.git
cd BiblioTrack

# Create build directory
mkdir -p build && cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Run the application
./bin/library_app
```

### Development Build
```bash
# For development with debug symbols and tests
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build .
ctest  # Run unit tests
```

## 📋 Features by Version

### v1.0 - Foundation (Week 1) ✅
- **Book Management:** Complete CRUD operations
- **Clean Architecture:** Modular, testable design
- **CSV Storage:** Reliable data persistence
- **Console Interface:** Intuitive menu-driven UI
- **Validation Framework:** Data integrity and business rules

### v1.1 - Search Feature Added ✅
- **Search Functionality:** Basic search by ISBN, Title, Author
- **Simple Integration:** Added as menu option 4 to existing interface
- **Minimal Changes:** All v1.0 functionality preserved unchanged

### v3.0 - Complete Library System (Week 3) 📋
- **Member Management:** Registration and profile management
- **Borrowing System:** Check-out and return workflows
- **Due Date Tracking:** Automated overdue detection
- **Transaction History:** Complete audit trail
- **Integration Testing:** Production-ready reliability

### v4.0+ - Advanced Features (Future) 🔮
- **Fine Management:** Automated calculation and tracking
- **Reporting & Analytics:** Usage statistics and insights
- **Notification System:** Due date reminders and alerts
- **Multi-Storage:** JSON and SQLite backend support

## 🖥️ User Interface

### Main Menu (v1.0)
```
=== BiblioTrack Library Management System v1.0 ===
1. Add Book
2. Delete Book  
3. View Current Stock
4. Search Books
5. Exit
Enter your choice: _
```

### Enhanced Menu (v2.0+)
```
=== BiblioTrack Library Management System v2.0 ===
📚 BOOK MANAGEMENT
1. Add Book                    2. Delete Book
3. View All Books             4. Search Books

👥 MEMBER MANAGEMENT (v3.0)
5. Register Member            6. View Members
7. Search Members             8. Update Member

📖 BORROWING SYSTEM (v3.0)
9. Borrow Book               10. Return Book
11. View Transactions        12. Overdue Books

⚙️  SYSTEM
13. Export Data              14. Import Data
15. System Settings          16. Exit

Enter your choice: _
```

## 💾 Data Management

### File Structure
```
BiblioTrack/
├── data/
│   ├── books.csv           # Book inventory
│   ├── members.csv         # Member database (v3.0)
│   ├── transactions.csv    # Borrowing history (v3.0)
│   └── config.ini          # System configuration
├── logs/
│   └── application.log     # System logs
└── backups/
    └── [timestamp]/        # Automated backups
```

### CSV Specifications

#### Books (books.csv)
```csv
ISBN,Title,Author,Year,Quantity
978-0134685991,"Effective Modern C++","Scott Meyers",2014,3
978-0321563842,"The C++ Programming Language","Bjarne Stroustrup",2013,2
```

#### Members (members.csv) - v3.0
```csv
MemberID,Name,Email,Phone,Status,RegisterDate,ExpiryDate
M001,"John Doe","john.doe@email.com","555-0123","Active","2025-10-01","2026-10-01"
```

#### Transactions (transactions.csv) - v3.0
```csv
TransactionID,MemberID,ISBN,BorrowDate,DueDate,ReturnDate,Status
T001,"M001","978-0134685991","2025-10-15","2025-10-29","","Active"
```

## 🔧 Configuration

### System Settings (config.ini)
```ini
[Library]
Name=City Public Library
MaxBooksPerMember=5
DefaultBorrowPeriod=14

[Files]
DataDirectory=./data
BackupDirectory=./backups
LogLevel=INFO

[Validation]
MinYear=1000
MaxYear=2025
RequireISBN=true
```

## 🧪 Testing

### Running Tests
```bash
# Build with tests enabled
cmake .. -DBUILD_TESTS=ON
cmake --build .

# Run all tests
ctest

# Run specific test suite
./tests/unit_tests
./tests/integration_tests
```

### Test Coverage
- **Unit Tests:** >85% code coverage
- **Integration Tests:** End-to-end workflow validation
- **Performance Tests:** Large dataset handling
- **Error Scenario Tests:** Comprehensive error handling

## 📊 Performance Specifications

### System Requirements
- **Memory:** 50MB base + 1MB per 1000 books
- **Storage:** 10KB per book + transaction history
- **CPU:** Single-core sufficient for <10,000 books
- **OS:** Cross-platform (Windows, macOS, Linux)

### Performance Targets
- **Search Response:** <100ms for 10,000 books
- **CRUD Operations:** <50ms per operation
- **Startup Time:** <2 seconds
- **Data Export:** 1000 books/second

## 🔒 Data Validation Rules

### Book Validation
- **ISBN:** Must be unique, valid format (10 or 13 digits)
- **Title:** Required, 1-200 characters
- **Author:** Required, 1-100 characters  
- **Year:** Range 1000-2025
- **Quantity:** Non-negative integer

### Member Validation (v3.0)
- **Member ID:** Auto-generated, unique format (M001, M002...)
- **Name:** Required, 2-50 characters
- **Email:** Valid email format, unique
- **Phone:** Valid phone number format
- **Status:** Active, Suspended, or Expired

### Business Rules (v3.0)
- **Borrowing Limit:** Maximum 5 books per member
- **Loan Period:** 14 days default (configurable)
- **Renewal:** Up to 2 renewals if no holds
- **Overdue Grace:** 3-day grace period before fines

## 🛠️ Development Workflow

### Project Structure
```
BiblioTrack/
├── include/                 # Header files
│   ├── entities/           # Domain models
│   ├── repository/         # Data access interfaces
│   ├── services/           # Business logic interfaces
│   └── presentation/       # UI components
├── src/                    # Implementation files
│   ├── entities/
│   ├── repository/
│   ├── services/
│   └── presentation/
├── tests/                  # Test suites
├── docs/                   # Documentation
├── data/                   # Runtime data
└── CMakeLists.txt         # Build configuration
```

### Coding Standards
- **Style:** Google C++ Style Guide
- **Documentation:** Doxygen comments for public APIs
- **Testing:** Test-driven development (TDD)
- **Review:** All code requires peer review
- **CI/CD:** Automated build and test pipeline

## 📈 Development Timeline

### Week 1: Foundation Sprint (Oct 2-8, 2025)
- [x] Project setup and clean architecture
- [x] Book entity and repository pattern  
- [x] Basic CRUD operations
- [x] Console interface and validation

### Week 2: Enhancement Sprint (Oct 9-15, 2025)
- [x] Search service implementation
- [x] Advanced CLI features
- [x] Data export/import
- [x] Performance optimization

### Week 3: Integration Sprint (Oct 16-23, 2025)
- [ ] Member management system
- [ ] Borrowing and return functionality
- [ ] Due date tracking
- [ ] System integration and testing

## 🤝 Contributing

### Getting Started
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Follow coding standards and add tests
4. Commit changes (`git commit -m 'Add amazing feature'`)
5. Push to branch (`git push origin feature/amazing-feature`)
6. Open a Pull Request

### Development Guidelines
- Write unit tests for new functionality
- Update documentation for API changes
- Follow existing code style and patterns
- Ensure all tests pass before submitting PR

## 📚 Documentation

- **[API Documentation](docs/api.md)** - Complete API reference
- **[User Manual](docs/user-guide.md)** - End-user documentation
- **[Developer Guide](docs/developer-guide.md)** - Development setup and patterns
- **[Architecture Decision Records](docs/adr/)** - Design decisions and rationale

## 🐛 Troubleshooting

### Common Issues

**Build Errors:**
```bash
# Clear build cache
rm -rf build/
mkdir build && cd build
cmake .. && make clean && make
```

**Data File Issues:**
```bash
# Reset data files
rm -rf data/
mkdir data
# Application will recreate with defaults
```

**Permission Errors:**
```bash
# Fix file permissions
chmod 755 bin/library_app
chmod -R 644 data/
```

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Clean Architecture** concepts by Robert C. Martin
- **C++ Core Guidelines** by Bjarne Stroustrup and Herb Sutter
- **Modern C++** best practices from the community

## 📞 Support

- **Issues:** [GitHub Issues](https://github.com/your-username/BiblioTrack/issues)
- **Discussions:** [GitHub Discussions](https://github.com/your-username/BiblioTrack/discussions)
- **Email:** support@bibliotrack.dev

---

**BiblioTrack** - Professional Library Management Made Simple