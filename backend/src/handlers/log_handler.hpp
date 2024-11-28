// include/handlers/log_handler.hpp
#pragma once

#include <crow.h>
#include <nlohmann/json.hpp>
#include "../database/database.hpp"
#include "../models/log_entry.hpp"

class LogHandler {
private:
    LogDatabase& database;

public:
    LogHandler(LogDatabase& db) : database(db) {}

    // GET logs endpoint
    crow::response getLogs(const crow::request& req) {
        try {
            // Parse query parameters
            auto page = req.url_params.get("page") ? 
                std::stoi(req.url_params.get("page")) : 1;
            auto limit = req.url_params.get("limit") ? 
                std::stoi(req.url_params.get("limit")) : 50;
            auto level = req.url_params.get("level") ? 
                std::string(req.url_params.get("level")) : "";
            auto service = req.url_params.get("service") ? 
                std::string(req.url_params.get("service")) : "";

            // Retrieve logs
            auto logs = database.getLogs(page, limit, level, service);
            
            // Convert logs to JSON
            nlohmann::json response;
            response["logs"] = nlohmann::json::array();
            for (const auto& log : logs) {
                response["logs"].push_back(log.toJson());
            }
            response["page"] = page;
            response["limit"] = limit;

            return crow::response(200, response.dump());
        } catch (const std::exception& e) {
            return crow::response(500, e.what());
        }
    }

    // POST log endpoint
    crow::response addLog(const crow::request& req) {
        try {
            // Parse JSON body
            auto json = nlohmann::json::parse(req.body);
            
            // Create log entry
            LogEntry log = LogEntry::fromJson(json);
            
            // Generate unique ID if not provided
            if (log.id.empty()) {
                log.id = generateUniqueId();
            }

            // Insert log
            database.insertLog(log);

            return crow::response(201);
        } catch (const std::exception& e) {
            return crow::response(400, e.what());
        }
    }

    // GET log statistics
    crow::response getLogStatistics(const crow::request& req) {
        try {
            auto stats = database.getLogStatistics();
            return crow::response(200, stats.dump());
        } catch (const std::exception& e) {
            return crow::response(500, e.what());
        }
    }

private:
    // Generate a unique ID for log entries
    std::string generateUniqueId() {
        // Simple implementation - replace with a more robust UUID generator
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count();
        return std::to_string(timestamp);
    }
};