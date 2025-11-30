#include "Cors.h"

using namespace drogon;


void setupCors()
{
	// ===== WHITELIST ORIGINÙ =====
	static const std::vector<std::string> allowedOrigins = {
		"https://teplomer-frontend.onrender.com",    // produkèní frontend
		"http://localhost:4200",                      // Angular dev
	};

	auto isAllowed = [&](const std::string& origin) {
		return std::find(allowedOrigins.begin(),
			allowedOrigins.end(),
			origin) != allowedOrigins.end();
		};

	// ===== OPTIONS PRE-FLIGHT =====
	drogon::app().registerSyncAdvice(
		[=](const drogon::HttpRequestPtr& req) -> drogon::HttpResponsePtr {
			if (req->method() != drogon::HttpMethod::Options)
				return {};

			const auto& origin = req->getHeader("Origin");

			// Origin není ? není to CORS ? ignorujeme
			if (origin.empty())
				return {};

			// Pokud origin není povolený ? nevracíme CORS hlavièky
			if (!isAllowed(origin))
				return {};

			auto resp = drogon::HttpResponse::newHttpResponse();
			resp->setStatusCode(drogon::k204NoContent);

			resp->addHeader("Access-Control-Allow-Origin", origin);
			resp->addHeader("Vary", "Origin");

			// Povolit metody podle požadavku
			const auto& requestMethod =
				req->getHeader("Access-Control-Request-Method");
			if (!requestMethod.empty())
				resp->addHeader("Access-Control-Allow-Methods", requestMethod);
			else
				resp->addHeader("Access-Control-Allow-Methods",
					"GET,POST,PUT,DELETE,OPTIONS");

			// Povolit požadované hlavièky (Content-Type, Authorization apod.)
			const auto& requestHeaders =
				req->getHeader("Access-Control-Request-Headers");
			if (!requestHeaders.empty())
				resp->addHeader("Access-Control-Allow-Headers", requestHeaders);
			else
				resp->addHeader("Access-Control-Allow-Headers", "Content-Type,Authorization");

			resp->addHeader("Access-Control-Allow-Credentials", "true");
			resp->addHeader("Access-Control-Max-Age", "600");

			return std::move(resp);
		});


	// ===== Normální GET/POST odpovìdi — pøidání CORS hlavièek =====
	drogon::app().registerPostHandlingAdvice(
		[=](const drogon::HttpRequestPtr& req,
			const drogon::HttpResponsePtr& resp) {

				const auto& origin = req->getHeader("Origin");
				if (origin.empty()) {
					LOG_INFO << "CORS: No Origin header present";
					return;
				}

				// Pokud origin není whitelisted ? nevracíme nic
				if (!isAllowed(origin)) {
					LOG_INFO << "CORS: Origin " << origin << " not allowed";
					return;
				}

				// Povolit jen konkrétní origin
				resp->addHeader("Access-Control-Allow-Origin", origin);
				resp->addHeader("Vary", "Origin");
				resp->addHeader("Access-Control-Allow-Credentials", "true");

				// Pokud FE poslal Access-Control-Request-Method, pøepošleme zpìt
				const auto& reqMethod =
					req->getHeader("Access-Control-Request-Method");
				if (!reqMethod.empty())
					resp->addHeader("Access-Control-Allow-Methods", reqMethod);

				// Pokud FE poslal Access-Control-Request-Headers, pøepošleme zpìt
				const auto& reqHeaders =
					req->getHeader("Access-Control-Request-Headers");
				if (!reqHeaders.empty())
					resp->addHeader("Access-Control-Allow-Headers", reqHeaders);
		});
}