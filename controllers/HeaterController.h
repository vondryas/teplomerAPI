#pragma once

#include <drogon/HttpController.h>
#include <controllers/responses/Response.h>
#include <stduuid/uuid.h>
#include <filters/FilterAliases.h>

using namespace drogon;

class HeaterController : public drogon::HttpController<HeaterController>
{
public:
	METHOD_LIST_BEGIN
		// use METHOD_ADD to add your custom processing function here;
		// METHOD_ADD(HeatherController::get, "/{2}/{1}", Get); // path is /HeatherController/{arg2}/{arg1}
		// METHOD_ADD(HeatherController::your_method_name, "/{1}/{2}/list", Get); // path is /HeatherController/{arg1}/{arg2}/list
		// ADD_METHOD_TO(HeatherController::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list
		ADD_METHOD_TO(HeaterController::getById, "heater/{1}", Get);
	ADD_METHOD_TO(HeaterController::status, "heater", Post);

	METHOD_LIST_END
		// your declaration of processing function maybe like this:
		// void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
		// void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
		Task<HttpResponsePtr> getById(const HttpRequestPtr req, const std::string& id);
	Task<HttpResponsePtr> status(const HttpRequestPtr req);

};
