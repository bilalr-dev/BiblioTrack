# BiblioTrack v2.09 Testing Suite

This directory contains comprehensive testing suites for the BiblioTrack Library Management System, including unit tests, integration tests, and smoke tests.

## 📋 Test Suites Overview

### 1. **Unit Tests** (`UnitTests.cpp`)
Tests individual components in isolation to verify their correctness.

**Coverage:**
- **Book Data Model**: Constructors, getters, setters, edge cases
- **LibraryService**: Validation methods, business logic, search operations
- **BookRepository**: Basic operations, persistence, data integrity
- **AuthenticationService**: Credential management, file handling
- **Edge Cases**: Large datasets, special characters, boundary values

**Key Test Categories:**
- ✅ Book default and parameterized constructors
- ✅ Input validation (ISBN, title, author, year, quantity)
- ✅ Search functionality (ISBN, title, author, category)
- ✅ Category management and statistics
- ✅ Authentication with valid/invalid credentials
- ✅ Data persistence across service instances
- ✅ Special character handling
- ✅ Boundary value testing

### 2. **Integration Tests** (`IntegrationTests.cpp`)
Tests component interactions and complete workflows.

**Coverage:**
- **LibraryService + BookRepository**: Complete data flow integration
- **Authentication + LibraryService**: Security integration
- **Complete Workflows**: End-to-end library management operations
- **Concurrent Operations**: Multiple simultaneous operations
- **Error Handling**: Integration-level error scenarios
- **Data Integrity**: Cross-component data consistency

**Key Test Categories:**
- ✅ Service-repository integration
- ✅ Data persistence across service instances
- ✅ Authentication-library service integration
- ✅ Complete library management workflows
- ✅ Concurrent operations handling
- ✅ Error handling and recovery
- ✅ Data integrity verification

### 3. **Smoke Tests** (`SmokeTests.cpp`)
Comprehensive end-to-end tests for critical functionality and edge cases.

**Coverage:**
- **Basic Operations**: Add, list, search, delete books
- **Search Functionality**: All search types and edge cases
- **Category Management**: Statistics and filtering
- **Authentication System**: Security and access control
- **Performance**: Large dataset handling
- **Edge Cases**: Special characters, boundary values
- **Statistics & Analytics (v2.08)**: Comprehensive statistics dashboard testing
- **Performance & Optimization (v2.09)**: Advanced algorithm optimization testing
- **Data Persistence**: Cross-session data integrity

**Key Test Categories:**
- ✅ Basic book operations (CRUD)
- ✅ Duplicate ISBN handling
- ✅ Input validation and error handling
- ✅ Search functionality (all types)
- ✅ Category management and statistics
- ✅ Authentication system security
- ✅ Performance with large datasets (1000+ books)
- ✅ Special character handling
- ✅ Boundary value testing
- ✅ Data persistence verification

### 4. **Test Runner** (`TestRunner.cpp`)
Utility to run all test suites with comprehensive reporting.

**Features:**
- Run individual test suites or all tests
- Build test suites before running
- Detailed test results and timing
- Summary reporting with pass/fail counts
- Command-line interface with options

## 🚀 Running Tests

### Prerequisites

**All Platforms:**
- C++17 compatible compiler
- CMake 3.12 or higher
- Built BiblioTrack project
- Credentials file setup (see Setup section below)

**Platform-Specific Requirements:**
- **Windows**: Visual Studio 2019+ or MinGW-w64
- **macOS**: Xcode Command Line Tools or Clang
- **Linux**: GCC 7+ or Clang 6+

### Step-by-Step Testing Instructions

#### 🪟 **Windows (Visual Studio)**

**Step 1: Build All Test Targets**
```cmd
# Navigate to build directory
cd cmake-build-debug

# Build all test executables
cmake --build . --config Release --target UnitTests IntegrationTests SmokeTests PerformanceTests TestRunner

# Or build all targets at once
cmake --build . --config Release --target ALL_BUILD
```

**Step 2: Setup Credentials**
```cmd
# Copy credentials template
copy ..\config\credentials_template.json ..\config\credentials.json

# Verify credentials file exists
dir ..\config\credentials.json
```

**Step 3: Run Individual Test Suites**
```cmd
# Run unit tests
.\Release\UnitTests.exe

# Run integration tests
.\Release\IntegrationTests.exe

# Run smoke tests
.\Release\SmokeTests.exe

# Run performance tests
.\Release\PerformanceTests.exe
```

**Step 4: Use Test Runner**
```cmd
# Run all tests
.\Release\TestRunner.exe --all

# Run specific test suite
.\Release\TestRunner.exe --suite UnitTests

# Build and run all tests
.\Release\TestRunner.exe --build

# List available test suites
.\Release\TestRunner.exe --list

# Show help
.\Release\TestRunner.exe --help
```

#### 🍎 **macOS**

**Step 1: Build All Test Targets**
```bash
# Navigate to build directory
cd cmake-build-debug

# Build all test executables
cmake --build . --target UnitTests IntegrationTests SmokeTests PerformanceTests TestRunner -j $(nproc)

# Or build all targets at once
cmake --build . --target ALL_BUILD -j $(nproc)
```

**Step 2: Setup Credentials**
```bash
# Copy credentials template
cp ../config/credentials_template.json ../config/credentials.json

# Set secure file permissions (recommended)
chmod 600 ../config/credentials.json

# Verify credentials file exists
ls -la ../config/credentials.json
```

**Step 3: Run Individual Test Suites**
```bash
# Run unit tests
./UnitTests

# Run integration tests
./IntegrationTests

# Run smoke tests
./SmokeTests

# Run performance tests
./PerformanceTests
```

**Step 4: Use Test Runner**
```bash
# Run all tests
./TestRunner --all

# Run specific test suite
./TestRunner --suite UnitTests

# Build and run all tests
./TestRunner --build

# List available test suites
./TestRunner --list

# Show help
./TestRunner --help
```

#### 🐧 **Linux (Ubuntu/Debian)**

**Step 1: Build All Test Targets**
```bash
# Navigate to build directory
cd cmake-build-debug

# Build all test executables
cmake --build . --target UnitTests IntegrationTests SmokeTests PerformanceTests TestRunner -j $(nproc)

# Or build all targets at once
cmake --build . --target ALL_BUILD -j $(nproc)
```

**Step 2: Setup Credentials**
```bash
# Copy credentials template
cp ../config/credentials_template.json ../config/credentials.json

# Set secure file permissions (recommended)
chmod 600 ../config/credentials.json

# Verify credentials file exists
ls -la ../config/credentials.json
```

**Step 3: Run Individual Test Suites**
```bash
# Run unit tests
./UnitTests

# Run integration tests
./IntegrationTests

# Run smoke tests
./SmokeTests

# Run performance tests
./PerformanceTests
```

**Step 4: Use Test Runner**
```bash
# Run all tests
./TestRunner --all

# Run specific test suite
./TestRunner --suite UnitTests

# Build and run all tests
./TestRunner --build

# List available test suites
./TestRunner --list

# Show help
./TestRunner --help
```

#### 🐧 **Linux (CentOS/RHEL/Fedora)**

**Step 1: Build All Test Targets**
```bash
# Navigate to build directory
cd cmake-build-debug

# Build all test executables
cmake --build . --target UnitTests IntegrationTests SmokeTests PerformanceTests TestRunner -j $(nproc)

# Or build all targets at once
cmake --build . --target ALL_BUILD -j $(nproc)
```

**Step 2-4: Follow Ubuntu/Debian steps above**

### 🔧 **Troubleshooting Test Issues**

#### **Common Issues and Solutions:**

**1. Test Executables Not Found**
```bash
# Make sure you built the test targets
cmake --build . --target UnitTests IntegrationTests SmokeTests PerformanceTests

# Check if executables exist
ls -la UnitTests IntegrationTests SmokeTests PerformanceTests  # Linux/macOS
dir UnitTests.exe IntegrationTests.exe SmokeTests.exe PerformanceTests.exe  # Windows
```

**2. Credentials File Missing**
```bash
# Create credentials file from template
cp ../config/credentials_template.json ../config/credentials.json  # Linux/macOS
copy ..\config\credentials_template.json ..\config\credentials.json  # Windows
```

**3. Permission Denied (Linux/macOS)**
```bash
# Make test executables executable
chmod +x UnitTests IntegrationTests SmokeTests PerformanceTests TestRunner
```

**4. Test Failures**
```bash
# Run individual tests to isolate issues
./UnitTests  # Check unit test failures
./IntegrationTests  # Check integration test failures
./SmokeTests  # Check smoke test failures
```

### 🚀 **Quick Test Commands (All Platforms)**

For experienced users, here's the minimal command sequence:

```bash
# 1. Build all tests
cmake --build cmake-build-debug --target UnitTests IntegrationTests SmokeTests PerformanceTests

# 2. Setup credentials
cp config/credentials_template.json config/credentials.json

# 3. Run all tests
cd cmake-build-debug
./TestRunner --all  # Linux/macOS
# or
.\Release\TestRunner.exe --all  # Windows
```

### 📋 **Test Execution Examples**

#### **Running Specific Test Categories:**
```bash
# Run only unit tests
./UnitTests

# Run only integration tests
./IntegrationTests

# Run only smoke tests
./SmokeTests

# Run only performance tests
./PerformanceTests
```

#### **Using Test Runner Options:**
```bash
# Run all tests with verbose output
./TestRunner --all --verbose

# Run specific test suite
./TestRunner --suite UnitTests

# Build and run all tests
./TestRunner --build --all

# List all available test suites
./TestRunner --list
```

## 📊 Test Coverage

### **Unit Test Coverage**
- **Book Data Model**: 100% method coverage
- **LibraryService**: 100% public method coverage
- **BookRepository**: 100% public method coverage
- **AuthenticationService**: 100% public method coverage
- **Edge Cases**: Comprehensive boundary testing

### **Integration Test Coverage**
- **Component Interactions**: All service-repository combinations
- **Data Flow**: Complete CRUD operations
- **Authentication Flow**: Login-logout scenarios
- **Error Propagation**: Cross-component error handling
- **Concurrent Operations**: Multi-threaded scenarios

### **Smoke Test Coverage**
- **End-to-End Workflows**: Complete user scenarios
- **Performance**: Large dataset operations
- **Edge Cases**: Special characters, boundary values
- **Data Integrity**: Cross-session persistence
- **Security**: Authentication and authorization

## 🧪 Test Categories

### **Functional Tests**
- ✅ Book management (add, delete, list, search)
- ✅ Category management and statistics
- ✅ Authentication and security
- ✅ Data persistence and integrity

### **Performance Tests**
- ✅ Large dataset operations (1000+ books)
- ✅ Search performance optimization
- ✅ Memory usage optimization
- ✅ Category statistics performance

### **Edge Case Tests**
- ✅ Empty and null inputs
- ✅ Special characters in data
- ✅ Boundary values (min/max years, quantities)
- ✅ Invalid data formats
- ✅ Missing files and permissions

### **Integration Tests**
- ✅ Service layer interactions
- ✅ Data persistence across sessions
- ✅ Authentication integration
- ✅ Error handling propagation

### **Security Tests**
- ✅ Authentication with valid/invalid credentials
- ✅ Missing credential files
- ✅ File permission handling
- ✅ Input sanitization

## 📈 Test Results Interpretation

### **Success Criteria**
- All unit tests pass (individual component correctness)
- All integration tests pass (component interaction correctness)
- All smoke tests pass (end-to-end functionality)
- Performance tests meet timing requirements
- No memory leaks or crashes

### **Statistics & Analytics Testing (v2.08)**

#### **Unit Test Coverage**
- ✅ **Basic Statistics**: Total books, quantity, authors, categories
- ✅ **Category Analytics**: Most popular, highest quantity, averages
- ✅ **Author Analytics**: Most prolific, book counts, top authors
- ✅ **Year Analytics**: Oldest/newest, year ranges, decade distribution
- ✅ **Edge Cases**: Single book, empty library, same values
- ✅ **Boundary Values**: Minimum/maximum valid years (1000-2024)
- ✅ **Special Characters**: Unicode, emojis, symbols, quotes
- ✅ **Performance**: 1000+ books processed in <10ms
- ✅ **Consistency**: Multiple calls return identical results

#### **Integration Test Coverage**
- ✅ **Cross-Component**: Statistics work with CRUD operations
- ✅ **Data Persistence**: Statistics reflect across service instances
- ✅ **Workflow Integration**: Complete library management with statistics
- ✅ **Performance Integration**: 500 books in <5ms

#### **Smoke Test Coverage**
- ✅ **End-to-End**: Complete statistics dashboard functionality
- ✅ **Edge Cases**: Empty library and single book scenarios
- ✅ **Performance**: 200 books in <1ms

### **Performance Benchmarks**
- **1000 books addition**: < 1000ms
- **Search operations**: < 10ms
- **Category statistics**: < 50ms
- **Statistics calculation**: < 10ms (1000 books)
- **Data persistence**: < 100ms

### **Error Handling**
- Invalid inputs properly rejected
- Error messages are clear and helpful
- System remains stable after errors
- Data integrity maintained after failures

## 🔧 Test Development Guidelines

### **Adding New Tests**
1. **Unit Tests**: Test individual methods and edge cases
2. **Integration Tests**: Test component interactions
3. **Smoke Tests**: Test complete user workflows
4. **Edge Cases**: Test boundary conditions and error scenarios

### **Test Structure**
```cpp
static void test_feature_name() {
    std::cout << "Testing feature name...\n";
    
    // Setup
    cleanTestData();
    // ... setup code ...
    
    // Test
    // ... test code ...
    assert(condition);
    
    // Cleanup
    // ... cleanup code ...
    
    std::cout << "✓ Feature name test passed\n";
}
```

### **Best Practices**
- Use descriptive test names
- Clean up test data after each test
- Test both success and failure scenarios
- Include performance measurements where relevant
- Document complex test scenarios
- Use assertions for verification
- Provide clear error messages

## 🐛 Troubleshooting

### **Common Issues**

**Tests fail to build:**
- Ensure all source files are included in CMakeLists.txt
- Check for missing includes or dependencies
- Verify C++17 standard is enabled

**Tests fail to run:**
- Ensure test data files are cleaned up
- Check file permissions for data/config directories
- Verify credentials.json exists for authentication tests

**Performance tests fail:**
- Check system resources (memory, CPU)
- Verify timing thresholds are appropriate
- Consider system load during testing

**Integration tests fail:**
- Ensure all services are properly initialized
- Check data persistence between test runs
- Verify component dependencies are correct

### **Debug Mode**
Run tests with debug output:
```bash
# Enable debug output
export DEBUG=1
./cmake-build-debug/UnitTests
```

## 📝 Test Maintenance

### **Regular Tasks**
- Update tests when adding new features
- Review and update performance benchmarks
- Add edge cases for new functionality
- Maintain test data consistency
- Update documentation for new test scenarios

### **Test Data Management**
- Use `cleanTestData()` function for cleanup
- Create isolated test environments
- Avoid dependencies between tests
- Use meaningful test data

This comprehensive testing suite ensures BiblioTrack v2.09 maintains high quality, reliability, and performance across all components and use cases.
