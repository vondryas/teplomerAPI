#include "Response.h"

HttpResponsePtr responses::okResponse(const std::string& body, const drogon::HttpStatusCode statusCode)
{
	auto resp = drogon::HttpResponse::newHttpResponse();
	resp->setStatusCode(statusCode);
	resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
	resp->setBody(body);
	return resp;
}

HttpResponsePtr responses::booleanOkResponse(const bool value, const drogon::HttpStatusCode statusCode)
{
	auto resp = drogon::HttpResponse::newHttpResponse();
	resp->setStatusCode(statusCode);
	resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
	resp->setBody(value ? "true" : "false");
	return resp;
}

HttpResponsePtr responses::wrongRequestResponse(const std::string& reason, const drogon::HttpStatusCode statusCode)
{
	auto resp = drogon::HttpResponse::newHttpResponse();
	resp->setStatusCode(statusCode);
	resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
	resp->setBody(reason);
	return resp;
}

HttpResponsePtr responses::jsonOkResponse(const Json::Value& json, const drogon::HttpStatusCode statusCode)
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

HttpResponsePtr responses::sizeOkResponse(const size_t value, const drogon::HttpStatusCode statusCode)
{
	auto resp = drogon::HttpResponse::newHttpResponse();
	resp->setStatusCode(statusCode);
	resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
	resp->setBody(std::to_string(value));
	return resp;
}

HttpResponsePtr responses::unauthorizedResponse(const std::string& reason, const drogon::HttpStatusCode statusCode)
{
	return wrongRequestResponse(reason, statusCode);
}

HttpResponsePtr responses::forbiddenResponse(const std::string& reason, const drogon::HttpStatusCode statusCode)
{
	return wrongRequestResponse(reason, statusCode);
}




