#include "PerformanceMonitor.hpp"
#include <algorithm>
#include <iomanip>

namespace Utils {

void PerformanceMonitor::recordOperation(const std::string& operationName, Duration duration) {
    auto& stats = operationStats_[operationName];
    
    stats.totalTime += duration;
    stats.callCount++;
    stats.minTime = std::min(stats.minTime, duration);
    stats.maxTime = std::max(stats.maxTime, duration);
    stats.avgTime = Duration(stats.totalTime.count() / stats.callCount);
}

const PerformanceMonitor::OperationStats& PerformanceMonitor::getStats(const std::string& operationName) const {
    static OperationStats emptyStats;
    auto it = operationStats_.find(operationName);
    return (it != operationStats_.end()) ? it->second : emptyStats;
}

const std::unordered_map<std::string, PerformanceMonitor::OperationStats>& PerformanceMonitor::getAllStats() const {
    return operationStats_;
}

void PerformanceMonitor::printReport() const {
    if (operationStats_.empty()) {
        std::cout << "No performance data recorded.\n";
        return;
    }
    
    std::cout << "\n=== Performance Report ===\n";
    std::cout << std::left << std::setw(25) << "Operation"
              << std::setw(10) << "Calls"
              << std::setw(12) << "Total (ms)"
              << std::setw(12) << "Avg (ms)"
              << std::setw(12) << "Min (ms)"
              << std::setw(12) << "Max (ms)" << "\n";
    std::cout << std::string(83, '-') << "\n";
    
    for (const auto& [operation, stats] : operationStats_) {
        std::cout << std::left << std::setw(25) << operation
                  << std::setw(10) << stats.callCount
                  << std::setw(12) << stats.totalTime.count()
                  << std::setw(12) << stats.avgTime.count()
                  << std::setw(12) << stats.minTime.count()
                  << std::setw(12) << stats.maxTime.count() << "\n";
    }
    std::cout << "\n";
}

void PerformanceMonitor::clear() {
    operationStats_.clear();
}

PerformanceMonitor::ScopedTimer PerformanceMonitor::timeOperation(const std::string& operationName) {
    return ScopedTimer(*this, operationName);
}

// Global performance monitor instance
PerformanceMonitor g_performanceMonitor;

} // namespace Utils
