# BiblioTrack - Secure Library Management System (v2.09)

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
- ✅ High-performance algorithms with optimized memory usage (v2.07)
- ✅ Comprehensive statistics dashboard with analytics (v2.08)
- ✅ Advanced algorithm optimizations with O(1) indexing and caching (v2.09)
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
- `LibraryApp.*` - User interface and menu system with category browsing and statistics dashboard (authentication delegated to services)
- `main.cpp` - Application entry point

## Project Structure

```
BiblioTrack/
├── src/
│   ├── datamodel/
│   │   └── Book.hpp              # Book entity
│   ├── services/
│   │   ├── AuthenticationService.* # Authentication layer
│   │   ├── BookRepository.*      # Data access layer
│   │   └── LibraryService.*      # Business logic layer
│   └── launcher/
│       ├── LibraryApp.*          # Application layer
│       └── main.cpp              # Entry point
├── tests/
│   └── SmokeTests.cpp            # Basic functionality tests
├── config/                        # 🔐 SECURE CONFIGURATION
│   └── credentials_template.json # Template for admin credentials
├── cmake-build-debug/            # Build directory (auto-created)
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
- Simple to replace components (e.g., JSON → Database)
- Clear boundaries for team development
- Backward compatibility maintained during feature additions

## Building and Running

### Prerequisites

**All Platforms:**
- C++17 compatible compiler
- CMake 3.12 or higher

**Platform-Specific Requirements:**
- **Windows**: Visual Studio 2019+ or MinGW-w64
- **macOS**: Xcode Command Line Tools or Clang
- **Linux**: GCC 7+ or Clang 6+

### Step-by-Step Build Instructions

#### 🪟 **Windows (Visual Studio)**

**Step 1: Install Prerequisites**
```cmd
# Install Visual Studio 2022 Community (free) with C++ workload
# Or install Build Tools for Visual Studio 2022
# Download from: https://visualstudio.microsoft.com/downloads/
```

**Step 2: Open Developer Command Prompt**
```cmd
# Open "Developer Command Prompt for VS 2022" or "x64 Native Tools Command Prompt"
# Or use PowerShell with Visual Studio environment
```

**Step 3: Build the Project**
```cmd
# Navigate to project directory
cd C:\path\to\BiblioTrack

# Create build directory
mkdir cmake-build-debug
cd cmake-build-debug

# Configure with Visual Studio generator
cmake -G "Visual Studio 17 2022" -A x64 ..

# Build the project
cmake --build . --config Release --target BiblioTrack

# Build all targets (including tests)
cmake --build . --config Release --target ALL_BUILD
```

**Step 4: Setup Credentials**
```cmd
# Copy credentials template
copy ..\config\credentials_template.json ..\config\credentials.json

# Edit credentials.json with your desired admin username/password
# Default: username="admin", password="admin123"
```

**Step 5: Run the Application**
```cmd
# Run from build directory
.\Release\BiblioTrack.exe

# Or run from project root
..\cmake-build-debug\Release\BiblioTrack.exe
```

#### 🍎 **macOS**

**Step 1: Install Prerequisites**
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install CMake (if not already installed)
# Option 1: Using Homebrew
brew install cmake

# Option 2: Download from https://cmake.org/download/
```

**Step 2: Build the Project**
```bash
# Navigate to project directory
cd /path/to/BiblioTrack

# Create build directory
mkdir cmake-build-debug
cd cmake-build-debug

# Configure the project
cmake ..

# Build the project
cmake --build . --target BiblioTrack -j $(nproc)

# Build all targets (including tests)
cmake --build . --target ALL_BUILD -j $(nproc)
```

**Step 3: Setup Credentials**
```bash
# Copy credentials template
cp ../config/credentials_template.json ../config/credentials.json

# Set secure file permissions (recommended)
chmod 600 ../config/credentials.json

# Edit credentials.json with your desired admin username/password
# Default: username="admin", password="admin123"
```

**Step 4: Run the Application**
```bash
# Run from build directory
./BiblioTrack

# Or run from project root
./cmake-build-debug/BiblioTrack
```

#### 🐧 **Linux (Ubuntu/Debian)**

**Step 1: Install Prerequisites**
```bash
# Update package list
sudo apt update

# Install build essentials and CMake
sudo apt install build-essential cmake git

# Verify installation
gcc --version
cmake --version
```

**Step 2: Build the Project**
```bash
# Navigate to project directory
cd /path/to/BiblioTrack

# Create build directory
mkdir cmake-build-debug
cd cmake-build-debug

# Configure the project
cmake ..

# Build the project
cmake --build . --target BiblioTrack -j $(nproc)

# Build all targets (including tests)
cmake --build . --target ALL_BUILD -j $(nproc)
```

**Step 3: Setup Credentials**
```bash
# Copy credentials template
cp ../config/credentials_template.json ../config/credentials.json

# Set secure file permissions (recommended)
chmod 600 ../config/credentials.json

# Edit credentials.json with your desired admin username/password
# Default: username="admin", password="admin123"
```

**Step 4: Run the Application**
```bash
# Run from build directory
./BiblioTrack

# Or run from project root
./cmake-build-debug/BiblioTrack
```

#### 🐧 **Linux (CentOS/RHEL/Fedora)**

**Step 1: Install Prerequisites**
```bash
# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install cmake3 git

# Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git

# Create symlink for cmake3 (CentOS/RHEL)
sudo ln -s /usr/bin/cmake3 /usr/bin/cmake
```

**Step 2-4: Follow Ubuntu/Debian steps above**

### 🔧 **Troubleshooting Build Issues**

#### **Common Issues and Solutions:**

**1. CMake Not Found**
```bash
# Windows: Add CMake to PATH or use full path
# macOS: Install via Homebrew or download from cmake.org
# Linux: Install via package manager
```

**2. Compiler Not Found**
```bash
# Windows: Install Visual Studio or Build Tools
# macOS: Run xcode-select --install
# Linux: Install build-essential package
```

**3. Permission Denied (Linux/macOS)**
```bash
# Make sure you have write permissions to the directory
# Or run with sudo (not recommended for development)
```

**4. CMake Cache Issues**
```bash
# Clean and rebuild
rm -rf cmake-build-debug
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
cmake --build . --target BiblioTrack
```

### 🚀 **Quick Start (All Platforms)**

For experienced users, here's the minimal command sequence:

```bash
# 1. Create build directory and configure
mkdir cmake-build-debug && cd cmake-build-debug
cmake ..

# 2. Build the project
cmake --build . --target BiblioTrack

# 3. Setup credentials
cp ../config/credentials_template.json ../config/credentials.json

# 4. Run the application
./BiblioTrack  # Linux/macOS
# or
.\Release\BiblioTrack.exe  # Windows
```

### 🛠️ **IDE Setup**

#### **CLion (Recommended)**
1. Open CLion and select "Open or Import"
2. Navigate to the BiblioTrack project directory
3. CLion will automatically detect the CMakeLists.txt
4. Configure CMake settings:
   - Build type: Debug or Release
   - CMake options: `-DCMAKE_BUILD_TYPE=Debug`
5. Build and run using the IDE interface

#### **Visual Studio Code**
1. Install C++ extension pack
2. Install CMake Tools extension
3. Open the BiblioTrack project folder
4. Configure CMake (Ctrl+Shift+P → "CMake: Configure")
5. Build and run using the IDE interface

#### **Visual Studio (Windows)**
1. Open Visual Studio
2. Select "Open a local folder"
3. Navigate to the BiblioTrack project directory
4. Visual Studio will detect CMakeLists.txt
5. Build and run using the IDE interface

#### **Xcode (macOS)**
1. Install Xcode from App Store
2. Open Terminal and navigate to project
3. Generate Xcode project: `cmake -G Xcode ..`
4. Open generated .xcodeproj file
5. Build and run using Xcode interface

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
8. **View Statistics** - Comprehensive library analytics dashboard (v2.08)
9. **Exit** - Close the application

## Data Storage

As of v2.09, books are stored in `data/books.jsonl` as one JSON object per line (line-delimited JSON). The data file is automatically created when the first book is added.

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

Example lines in `books.jsonl` (numeric ISBNs):
```
{"isbn":"9780134685991","title":"Effective Modern C++","author":"Scott Meyers","year":2014,"quantity":5,"category":"Programming"}
{"isbn":"1255289001","title":"batman","author":"John Cena","year":2025,"quantity":99,"category":"Fiction"}
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
- **Legacy Support**: Seamlessly handles existing JSON files with automatic category assignment
- **Gradual Migration**: Old books are automatically assigned "General" category
- **Non-Breaking**: All existing functionality remains unchanged

## 🔧 **Data Quality & Input Sanitization (v2.04)**

### **Enhanced JSON Handling**
- **Proper JSON Escaping**: Books with quotes, newlines, or special characters in titles/authors are properly escaped
- **Robust Parsing**: System correctly handles escaped characters and special characters in JSON files
- **Data Integrity**: Prevents JSON corruption from special characters in book data

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

### **Performance Optimizations (v2.07)**
- **Algorithm Improvements**: O(n²) → O(n) category collection using unordered_set
- **Memory Optimization**: Pre-allocated containers with estimated sizes
- **String Operations**: Optimized sanitization with single-pass processing
- **JSON Operations**: Efficient string building with reserved capacity
- **Search Performance**: Enhanced with early size checks and const references
- **Caching**: Static current year calculation to avoid repeated system calls

### **Statistics Dashboard (v2.08)**
- **Collection Overview**: Total books, quantity, authors, and categories
- **Category Analytics**: Most popular categories and quantity distribution
- **Author Analytics**: Most prolific authors and top contributors
- **Publication Analytics**: Year ranges, decade distribution, and trends
- **Quick Insights**: Averages, oldest/newest books, and collection metrics
- **Performance Optimized**: Efficient single-pass data analysis
- **Real-time Updates**: Statistics reflect current library state

### **Advanced Algorithm Optimizations (v2.09)**
- **O(1) Indexing**: Advanced hash-based indexing for title, author, and category searches
- **Intelligent Caching**: Pre-computed statistics with automatic cache invalidation
- **Move Semantics**: Optimized string operations using C++11 move semantics
- **Memory Efficiency**: Pre-allocated containers and optimized data structures
- **Performance Monitoring**: Built-in performance measurement utilities
- **Search Optimization**: Partial matching with case-insensitive normalization
- **Data Integrity**: Robust index management during CRUD operations

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

- **v2.09**: Advanced algorithm optimization release - O(1) indexing, intelligent caching, move semantics, performance monitoring, and JSONL format standardization
- **v2.08**: Statistics dashboard release - Comprehensive library analytics, collection insights, and performance metrics
- **v2.07**: Performance optimization release - O(n²) → O(n) algorithms, memory optimization, enhanced testing
- **v2.06**: Added secure authentication system with config directory isolation and enterprise-grade security
- **v2.05**: JSON storage implementation, improved performance with in-memory caching
- **v2.04**: Added comprehensive category management and browsing features
- **v2.03**: Enhanced search functionality with partial matching
- **v2.02**: Improved user interface and error handling
- **v2.01**: Added ISBN validation and data persistence
- **v2.00**: Initial layered architecture implementation