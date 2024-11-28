// include/database/database.hpp
#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include <memory>
#include <numeric>
#include <boost/algorithm/string/join.hpp>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "../models/log_entry.hpp"

class LogDatabase {
private:
    sqlite3* db;
    std::string dbPath;

    // Prepare SQL statement
    sqlite3_stmt* prepareStatement(const std::string& query) {
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement: " + 
                std::string(sqlite3_errmsg(db)));
        }
        return stmt;
    }

public:
    LogDatabase(const std::string& path = "logs.db") : dbPath(path), db(nullptr) {
        open();
        createTable();
    }

    ~LogDatabase() {
        if (db) sqlite3_close(db);
    }

    // Open database connection
    void open() {
        if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
            throw std::runtime_error("Cannot open database: " + 
                std::string(sqlite3_errmsg(db)));
        }
    }

    // Create logs table if not exists
    void createTable() {
        const char* createTableSQL = R"(
            CREATE TABLE IF NOT EXISTS logs (
                id TEXT PRIMARY KEY,
                message TEXT NOT NULL,
                level TEXT NOT NULL,
                timestamp INTEGER NOT NULL,
                service TEXT,
                component TEXT
            );
            CREATE INDEX IF NOT EXISTS idx_logs_timestamp ON logs(timestamp);
            CREATE INDEX IF NOT EXISTS idx_logs_level ON logs(level);
        )";

        char* errMsg = nullptr;
        if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::string error(errMsg);
            sqlite3_free(errMsg);
            throw std::runtime_error("SQL error: " + error);
        }
    }

    // Insert a log entry
    void insertLog(const LogEntry& log) {
        const std::string insertSQL = 
            "INSERT INTO logs (id, message, level, timestamp, service, component) "
            "VALUES (?, ?, ?, ?, ?, ?)";
        
        sqlite3_stmt* stmt = prepareStatement(insertSQL);
        
        sqlite3_bind_text(stmt, 1, log.id.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, log.message.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, LogEntry::levelToString(log.level).c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 4, std::chrono::system_clock::to_time_t(log.timestamp));
        sqlite3_bind_text(stmt, 5, log.service.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, log.component.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw std::runtime_error("Failed to insert log: " + 
                std::string(sqlite3_errmsg(db)));
        }
        sqlite3_finalize(stmt);
    }

    // Retrieve logs with filtering and pagination
    std::vector<LogEntry> getLogs(
        int page = 1, 
        int pageSize = 50, 
        const std::string& level = "", 
        const std::string& service = ""
    ) {
        std::vector<LogEntry> logs;
        std::string query = "SELECT * FROM logs ";
        
        // Build WHERE clause
        std::vector<std::string> conditions;
        if (!level.empty()) conditions.push_back("level = ?");
        if (!service.empty()) conditions.push_back("service = ?");
        
        if (!conditions.empty()) {
            query += "WHERE " + boost::algorithm::join(conditions, " AND ") + " ";
        }
        
        // Add pagination
        query += "ORDER BY timestamp DESC LIMIT ? OFFSET ?";

        sqlite3_stmt* stmt = prepareStatement(query);
        
        // Bind parameters
        int paramIndex = 1;
        if (!level.empty()) 
            sqlite3_bind_text(stmt, paramIndex++, level.c_str(), -1, SQLITE_STATIC);
        if (!service.empty()) 
            sqlite3_bind_text(stmt, paramIndex++, service.c_str(), -1, SQLITE_STATIC);
        
        sqlite3_bind_int(stmt, paramIndex++, pageSize);
        sqlite3_bind_int(stmt, paramIndex, (page - 1) * pageSize);

        // Execute query and fetch results
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            LogEntry log;
            log.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            log.message = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            log.level = LogEntry::stringToLevel(
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))
            );
            log.timestamp = std::chrono::system_clock::from_time_t(
                sqlite3_column_int64(stmt, 3)
            );
            log.service = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            log.component = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            
            logs.push_back(log);
        }
        
        sqlite3_finalize(stmt);
        return logs;
    }

    // Get log statistics
    nlohmann::json getLogStatistics() {
        nlohmann::json stats;
        
        const char* statsQuery = R"(
            SELECT 
                level, 
                COUNT(*) as count, 
                MIN(timestamp) as oldest, 
                MAX(timestamp) as newest 
            FROM logs 
            GROUP BY level
        )";

        sqlite3_stmt* stmt = prepareStatement(statsQuery);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string level = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            int count = sqlite3_column_int(stmt, 1);
            
            stats[level] = {
                {"count", count},
                {"oldest", sqlite3_column_int64(stmt, 2)},
                {"newest", sqlite3_column_int64(stmt, 3)}
            };
        }
        
        sqlite3_finalize(stmt);
        return stats;
    }
};