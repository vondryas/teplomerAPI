#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpResponse.h>

using namespace drogon;

namespace responses
{
	HttpResponsePtr wrongRequestResponse(const std::string& reason,
		const drogon::HttpStatusCode statusCode = drogon::k400BadRequest);

	HttpResponsePtr jsonOkResponse(const Json::Value& json,
		const drogon::HttpStatusCode statusCode = drogon::k200OK);

	HttpResponsePtr notFoundResponse(const std::string& reason,
		const drogon::HttpStatusCode statusCode = drogon::k404NotFound);

	HttpResponsePtr internalServerErrorResponse(const std::string& reason,
		const drogon::HttpStatusCode statusCode = drogon::k500InternalServerError);

	HttpResponsePtr sizeOkResponse(const size_t value,
		const drogon::HttpStatusCode statusCode = drogon::k200OK);

	HttpResponsePtr unauthorizedResponse(const std::string& reason,
		const drogon::HttpStatusCode statusCode = drogon::k401Unauthorized);

	HttpResponsePtr forbiddenResponse(const std::string& reason,
		const drogon::HttpStatusCode statusCode = drogon::k403Forbidden);
}