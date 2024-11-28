// include/models/log_entry.hpp
#pragma once

#include <string>
#include <chrono>
#include <nlohmann/json.hpp>

enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

struct LogEntry {
    std::string id;
    std::string message;
    LogLevel level;
    std::chrono::system_clock::time_point timestamp;
    std::string service;
    std::string component;

    // Convert LogLevel to string
    static std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE: return "TRACE";
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::CRITICAL: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }

    // Convert string to LogLevel
    static LogLevel stringToLevel(const std::string& levelStr) {
        if (levelStr == "TRACE") return LogLevel::TRACE;
        if (levelStr == "DEBUG") return LogLevel::DEBUG;
        if (levelStr == "INFO") return LogLevel::INFO;
        if (levelStr == "WARNING") return LogLevel::WARNING;
        if (levelStr == "ERROR") return LogLevel::ERROR;
        if (levelStr == "CRITICAL") return LogLevel::CRITICAL;
        return LogLevel::INFO; // Default
    }

    // JSON serialization
    nlohmann::json toJson() const {
        return {
            {"id", id},
            {"message", message},
            {"level", levelToString(level)},
            {"timestamp", std::chrono::system_clock::to_time_t(timestamp)},
            {"service", service},
            {"component", component}
        };
    }

    // JSON deserialization
    static LogEntry fromJson(const nlohmann::json& j) {
        LogEntry entry;
        entry.id = j.value("id", "");
        entry.message = j.value("message", "");
        entry.level = stringToLevel(j.value("level", "INFO"));
        
        // Handle timestamp conversion
        if (j.contains("timestamp")) {
            auto timestamp = j["timestamp"].get<std::time_t>();
            entry.timestamp = std::chrono::system_clock::from_time_t(timestamp);
        } else {
            entry.timestamp = std::chrono::system_clock::now();
        }
        
        entry.service = j.value("service", "");
        entry.component = j.value("component", "");
        
        return entry;
    }
};