#pragma once
#include <chrono>
#include <string>
#include <iostream>
#include <unordered_map>

namespace Utils {

/**
 * Performance monitoring utility for measuring and tracking execution times
 * Provides automatic timing, statistics, and performance reporting
 */
class PerformanceMonitor {
public:
    using TimePoint = std::chrono::high_resolution_clock::time_point;
    using Duration = std::chrono::milliseconds;
    
    struct OperationStats {
        Duration totalTime{0};
        size_t callCount = 0;
        Duration minTime{Duration::max()};
        Duration maxTime{Duration::min()};
        Duration avgTime{0};
    };
    
    /**
     * RAII timer that automatically records execution time when destroyed
     */
    class ScopedTimer {
    public:
        ScopedTimer(PerformanceMonitor& monitor, const std::string& operationName)
            : monitor_(monitor), operationName_(operationName), startTime_(std::chrono::high_resolution_clock::now()) {}
        
        ~ScopedTimer() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<Duration>(endTime - startTime_);
            monitor_.recordOperation(operationName_, duration);
        }
        
    private:
        PerformanceMonitor& monitor_;
        std::string operationName_;
        TimePoint startTime_;
    };
    
    /**
     * Record an operation's execution time
     */
    void recordOperation(const std::string& operationName, Duration duration);
    
    /**
     * Get statistics for a specific operation
     */
    const OperationStats& getStats(const std::string& operationName) const;
    
    /**
     * Get all operation statistics
     */
    const std::unordered_map<std::string, OperationStats>& getAllStats() const;
    
    /**
     * Print performance report to console
     */
    void printReport() const;
    
    /**
     * Clear all recorded statistics
     */
    void clear();
    
    /**
     * Create a scoped timer for automatic timing
     */
    ScopedTimer timeOperation(const std::string& operationName);

private:
    std::unordered_map<std::string, OperationStats> operationStats_;
};

// Global performance monitor instance
extern PerformanceMonitor g_performanceMonitor;

// Convenience macros for performance monitoring
#define PERF_TIMER(operation) Utils::PerformanceMonitor::ScopedTimer _timer(Utils::g_performanceMonitor, operation)
#define PERF_RECORD(operation, duration) Utils::g_performanceMonitor.recordOperation(operation, duration)

} // namespace Utils
