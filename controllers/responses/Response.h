#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpResponse.h>

using namespace drogon;

namespace responses
{
	HttpResponsePtr wrongRequestResponse(const std::string& reason,
		const drogon::HttpStatusCode statusCode = drogon::k400BadRequest);

	HttpResponsePtr JsonOkResponse(const Json::Value& json,
		const drogon::HttpStatusCode statusCode = drogon::k200OK);

	HttpResponsePtr notFoundResponse(const std::string& reason,
		const drogon::HttpStatusCode statusCode = drogon::k404NotFound);

	HttpResponsePtr internalServerErrorResponse(const std::string& reason,
		const drogon::HttpStatusCode statusCode = drogon::k500InternalServerError);
}