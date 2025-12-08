#include "HeaterController.h"
#include <fmt/core.h>
#include <cstdlib> 

// Add definition of your processing function here

Task<HttpResponsePtr> HeaterController::getById(const HttpRequestPtr req, const std::string& id)
{
	HttpResponsePtr resp = nullptr;
	if (id.empty())
	{
		LOG_ERROR << "Id parameter is empty";
		co_return responses::wrongRequestResponse("Id parameter is required");
	}
	if (!uuids::uuid::is_valid_uuid(id))
	{
		LOG_DEBUG << "Invalid UUID format for id: " << id;
		co_return responses::wrongRequestResponse(fmt::format("Invalid UUID format for id: {}", id));
	}
	// Here you would typically call a facade or service to get the data by ID
	// For demonstration, we'll just return a dummy JSON response
	Json::Value jsonResponse;
	jsonResponse["id"] = id;
	jsonResponse["status"] = "active"; // Dummy data
	co_return responses::jsonOkResponse(jsonResponse);
}

Task<HttpResponsePtr> HeaterController::status(const HttpRequestPtr req)
{
	HttpResponsePtr resp = nullptr;
	auto json = req->getJsonObject();
	LOG_INFO << "Received JSON: " << (json ? json->toStyledString() : "null");
	// Here you would typically call a facade or service to get the status
	// For demonstration, we'll just return a dummy status response
	Json::Value jsonResponse;
	jsonResponse["status"] = "operational"; // Dummy status
	co_return responses::jsonOkResponse(jsonResponse);
}