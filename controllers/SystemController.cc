#include "SystemController.h"
#include <drogon/drogon.h>
#include <json/json.h>
#include <fstream>
#include <sstream>

Task<HttpResponsePtr> SystemController::health(const HttpRequestPtr req)
{
	co_return responses::sizeOkResponse(1);
}

Task<HttpResponsePtr> SystemController::openapi(const HttpRequestPtr req)
{
	// Cesta k openapi souboru – uprav, pokud ho máš jinde
	const std::string specPath = "openapi.json";

	std::ifstream file(specPath);
	if (!file.is_open())
	{
		co_return responses::internalServerErrorResponse(
			"Could not open OpenAPI specification file"
		);
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	const std::string content = buffer.str();
	Json::Value jsonData(content);

	co_return responses::jsonOkResponse(
		jsonData
	);
}