#include "https_mode.h"
#include "crow.h"
#include "spdlog/spdlog.h"
#include <csignal>
#include <sstream>

bool stringToBool(const std::string& str) {
	bool b;
	std::istringstream(str) >> std::boolalpha >> b;
	return b;
}

void runHttpsMode(std::shared_ptr<KVStore> kv_store) {
	crow::SimpleApp app;
	app.loglevel(crow::LogLevel::Warning);

	CROW_ROUTE(app, "/")([]() {
		return "Stellis vagamur";
	});

	CROW_ROUTE(app, "/set").methods(crow::HTTPMethod::POST)([kv_store](const crow::request& req) {
		auto body = req.get_body_params();
		auto key = body.get("key"), value = body.get("value");
		bool is_persistent = stringToBool(body.get("is_persistent"));
		kv_store->set(key, value, is_persistent);
		return crow::json::wvalue{
		    {"key", key},
		    {"value", value},
		    {"is_persistent", is_persistent}};
	});

	CROW_ROUTE(app, "/get").methods(crow::HTTPMethod::GET)([kv_store](const crow::request& req) {
		auto key = req.url_params.get("key");
		return crow::json::wvalue{
		    {"key", key},
		    {"value", kv_store->get(key)}};
	});

	CROW_ROUTE(app, "/delete").methods(crow::HTTPMethod::DELETE)([kv_store](const crow::request& req) {
		auto key = req.url_params.get("key");
		kv_store->del(key);
		return crow::json::wvalue{
		    {"key", key}};
	});

	CROW_ROUTE(app, "/exists").methods(crow::HTTPMethod::GET)([kv_store](const crow::request& req) {
		auto key = req.url_params.get("key");
		return crow::json::wvalue{
		    {"key", key},
		    {"exists", kv_store->exists(key)}};
	});

	std::signal(SIGTERM, [](int) {
		spdlog::info("SIGTERM received, stopping server...");
	});

	spdlog::info("[Keystore] Starting TCP server on port 3000");

	app.port(3000).multithreaded().run();

	spdlog::info("[Keystore] Server shutting down gracefully...");
}
