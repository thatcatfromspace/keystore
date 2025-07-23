#include "http_mode.h"
#include "crow.h"
#include "spdlog/spdlog.h"
#include <chrono>
#include <csignal>
#include <sstream>

bool stringToBool(const std::string& str) {
	bool b;
	std::istringstream(str) >> std::boolalpha >> b;
	return b;
}

HttpMode::HttpMode(std::shared_ptr<KvStore> kv_store)
    : kv_store_(std::move(kv_store)), app_(nullptr) {
}

void HttpMode::run() {
	status_ = ModeStatus::Running;
	spdlog::info("[HttpMode] Starting HTTP server");
	app_ = std::make_unique<crow::SimpleApp>();
	// Launch server thread
	worker_threads_.emplace_back([this]() { this->serverLoop(); });

	while (!terminate_) {
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	if (app_)
		app_->stop();
	status_ = ModeStatus::Stopping;
}

void HttpMode::cleanup() {
	spdlog::info("[HttpMode] Cleaning up HTTP server");
	terminate_ = true;
	if (app_)
		app_->stop();
	for (auto& t : worker_threads_) {
		if (t.joinable())
			t.join();
	}
	app_.reset();
	status_ = ModeStatus::Stopped;
}

void HttpMode::handleSignal(int signal) {
	spdlog::info("[HttpMode] Signal {} received, shutting down...", signal);
	terminate_ = true;
	if (app_)
		app_->stop();
}

ModeStatus HttpMode::getStatus() const {
	return status_;
}

void HttpMode::serverLoop() {
	auto& app = *app_;
	app.loglevel(crow::LogLevel::Warning);

	CROW_ROUTE(app, "/")([]() {
		return "Stellis vagamur";
	});

	CROW_ROUTE(app, "/set").methods(crow::HTTPMethod::POST)([this](const crow::request& req) {
		auto body = req.get_body_params();
		auto key = body.get("key"), value = body.get("value");
		bool is_persistent = stringToBool(body.get("is_persistent"));
		kv_store_->set(key, value, is_persistent);
		return crow::json::wvalue{
		    {"key", key},
		    {"value", value},
		    {"is_persistent", is_persistent}};
	});

	CROW_ROUTE(app, "/get").methods(crow::HTTPMethod::GET)([this](const crow::request& req) {
		auto key = req.url_params.get("key");
		return crow::json::wvalue{
		    {"key", key},
		    {"value", kv_store_->get(key)}};
	});

	CROW_ROUTE(app, "/delete").methods(crow::HTTPMethod::DELETE)([this](const crow::request& req) {
		auto key = req.url_params.get("key");
		kv_store_->del(key);
		return crow::json::wvalue{
		    {"key", key}};
	});

	CROW_ROUTE(app, "/exists").methods(crow::HTTPMethod::GET)([this](const crow::request& req) {
		auto key = req.url_params.get("key");
		return crow::json::wvalue{
		    {"key", key},
		    {"exists", kv_store_->exists(key)}};
	});

	spdlog::info("[HttpMode] Starting HTTPS server on port 3000");
	app.port(3000).multithreaded().run();
	spdlog::info("[HttpMode] Server shutting down gracefully...");
	terminate_ = true;
}
