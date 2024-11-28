// src/main.cpp
#define CROW_ENABLE_CORS
#include <crow.h>
#include <nlohmann/json.hpp>
#include "database/database.hpp"
#include "handlers/log_handler.hpp"
#include <crow/middlewares/cors.h> 
int main() {
    // Initialize database
    LogDatabase database("logs.db");

    // Create log handler
    LogHandler logHandler(database);

    // Create Crow application with CORS
    crow::App<crow::CORSHandler> app;

    // Configure CORS
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors
        .global()
        .methods("GET"_method, "POST"_method)
        .origin("*");

    // Define routes
    CROW_ROUTE(app, "/api/logs")
        .methods("GET"_method)
        ([&logHandler](const crow::request& req) {
            return logHandler.getLogs(req);
        });

    CROW_ROUTE(app, "/api/logs")
        .methods("POST"_method)
        ([&logHandler](const crow::request& req) {
            return logHandler.addLog(req);
        });

    CROW_ROUTE(app, "/api/logs/statistics")
        .methods("GET"_method)
        ([&logHandler](const crow::request& req) {
            return logHandler.getLogStatistics(req);
        });

    // Start server
    app.port(8080)
       .multithreaded()
       .run();

    return 0;
}