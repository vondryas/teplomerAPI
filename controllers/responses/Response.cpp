#include "Response.h"

void responses::wrongRequestResponse(const std::string& reason, const HttpResponseCallback& callback, const drogon::HttpStatusCode statusCode)
{
	auto resp = drogon::HttpResponse::newHttpResponse();
	resp->setStatusCode(statusCode);
	resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
	resp->setBody(reason);
	callback(resp);
}


void responses::jsonResponse(const Json::Value& json, const HttpResponseCallback& callback, const drogon::HttpStatusCode statusCode)
{
	auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
	resp->setStatusCode(statusCode);
	resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
	resp->setBody(jsonToString(json));
	callback(resp);
}

void responses::textResponse(const std::string& text, const HttpResponseCallback& callback, const drogon::HttpStatusCode statusCode)
{
	auto resp = drogon::HttpResponse::newHttpResponse();
	resp->setStatusCode(statusCode);
	resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
	resp->setBody(text);
	callback(resp);
}

