#include "Response.h"

HttpResponsePtr responses::wrongRequestResponse(const std::string& reason, const drogon::HttpStatusCode statusCode)
{
	auto resp = drogon::HttpResponse::newHttpResponse();
	resp->setStatusCode(statusCode);
	resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
	resp->setBody(reason);
	return resp;
}

HttpResponsePtr responses::JsonOkResponse(const Json::Value& json, const drogon::HttpStatusCode statusCode)
{
	auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
	resp->setStatusCode(statusCode);
	resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
	resp->setStatusCode(statusCode);
	return resp;
}

HttpResponsePtr responses::notFoundResponse(const std::string& reason, const drogon::HttpStatusCode statusCode)
{
	return wrongRequestResponse(reason, statusCode);
}

HttpResponsePtr responses::internalServerErrorResponse(const std::string& reason, const drogon::HttpStatusCode statusCode)
{
	return wrongRequestResponse(reason, statusCode);
}




